#include "tasks.h"
#include <QThread>

void send(QByteArray msg, QHostAddress *serverIP, QHostAddress clientIP, quint16 clientPort)
{
    QUdpSocket xmt_sock;
    xmt_sock.bind(*serverIP, portForSending, QUdpSocket::ShareAddress);
    xmt_sock.connectToHost(clientIP, clientPort);
    if ( !xmt_sock.waitForConnected(1))
    {
        /*UDP connection timeout*/
        return;
    }

    xmt_sock.write(msg);
}

quint16 findWordPos(QByteArray qba, QByteArray word, quint16 begin, quint16 end)
{
    quint8 wordLength = word.length();
    quint16 pos = begin;
    while (pos < end)
    {
        if (qba.mid(pos, wordLength) == word)
        {
            return pos;
        }
        pos++;
    }
    return 0;
}

Task_makeToken::Task_makeToken(QHostAddress *serverIP_inp, QByteArray msg_inp,
                               QHostAddress clientIP_inp, quint16 clientPort_inp,
                               QMap<QByteArray, uint> *pCredentialsMap_inp,
                               QReadWriteLock *pCredentialsMapLock_inp)
    : serverIP(serverIP_inp), msg(msg_inp), clientIP(clientIP_inp), clientPort(clientPort_inp),
      pCredentialsMap(pCredentialsMap_inp), pCredentialsMapLock(pCredentialsMapLock_inp)
{
    /*NOTHING TO DO*/
}

void Task_makeToken::run()
{
    if ( ( !msg.isEmpty()) && (msg.size() <= authWordLength + maxLoginSize) && (msg.size() > authWordLength) )
    {
        if (msg.left(authWordLength) == authWord.toUtf8())
        {
            auto login = msg.mid(authWordLength, msg.size() - authWordLength);
            pCredentialsMapLock->lockForRead();
            if (pCredentialsMap->size() < maxConnects)
            {
                if ( !pCredentialsMap->contains(login))
                {
                    pCredentialsMapLock->unlock();
                    auto token = qHash(login, QDateTime::currentDateTime().toTime_t());

                    pCredentialsMapLock->lockForWrite();
                    pCredentialsMap->insert(login, token);
                    pCredentialsMapLock->unlock();

                    QByteArray toSend;
                    for (int i = tokenSize - 1; i >= 0; --i)
                        toSend.append((token & (0xFF << i*8)) >> i*8);

                    send(toSend, serverIP, clientIP, clientPort);
                }
                else
                {
                    pCredentialsMapLock->unlock();
                    /*Login already exists*/
                }
            }
            else
            {
                /*Amount of connects exceeds maximum*/
            }
        }
        else
        {
            /*Wrong message*/
        }
    }
    else
    {
        /*Wrong message size*/
    }
}



Task_recordMsg::Task_recordMsg(QHostAddress *serverIP_inp, QByteArray msg_inp,
                               QHostAddress clientIP_inp, quint16 clientPort_inp,
                               QMap<QByteArray, uint> *pCredentialsMap_inp,
                               QReadWriteLock *pCredentialsMapLock_inp,
                               QFile *pLogFile_inp,
                               std::mutex *pWriteAllowedLock_inp,
                               std::condition_variable *pThreadAllowedToWrite_inp,
                               int* pNumWriteAllowed_inp,
                               std::mutex *pNumWriteAllowedLock_inp,
                               int currThreadNum_inp)
    : serverIP(serverIP_inp), msg(msg_inp), clientIP(clientIP_inp), clientPort(clientPort_inp),
      pCredentialsMap(pCredentialsMap_inp), pCredentialsMapLock(pCredentialsMapLock_inp),
      pLogFile(pLogFile_inp), pWriteAllowedLock(pWriteAllowedLock_inp),
      pThreadAllowedToWrite(pThreadAllowedToWrite_inp),
      pNumWriteAllowed(pNumWriteAllowed_inp),
      pNumWriteAllowedLock(pNumWriteAllowedLock_inp),
      currThreadNum(currThreadNum_inp)
{
    /*NOTHING TO DO*/
}

void Task_recordMsg::run()
{
    if ( ( !msg.isEmpty()) || (msg.size() <= maxLoginSize) )
    {
        if (msg.left(loginWordLength) == loginWord.toUtf8())
        {
            auto tokenPos = findWordPos(msg,
                                        tokenWord.toUtf8(),
                                        loginWordLength + 1,
                                        loginWordLength + maxLoginSize);
            if (tokenPos)
            {
                auto login = msg.mid(loginWordLength, tokenPos - loginWordLength);
                auto token_qba = msg.mid(tokenPos + tokenWordLength, tokenSize);

                uint token = 0;
                for (int i = tokenSize - 1; i >= 0; --i)
                    token |= (quint8)token_qba[i] << (3-i)*8;

                pCredentialsMapLock->lockForRead();
                auto tokenIter = pCredentialsMap->find(login);
                if(tokenIter != pCredentialsMap->end())
                {
                    pCredentialsMapLock->unlock();

                    if (token == tokenIter.value())
                    {
                        auto toRecord = msg.mid(tokenPos + tokenWordLength +
                                                tokenSize + msgWordLength);


                        while (true)
                        {
                            pNumWriteAllowedLock->lock();
                            if (*pNumWriteAllowed == currThreadNum)
                            {
                                break;
                            }
                            pNumWriteAllowedLock->unlock();

                            std::unique_lock<std::mutex> lk(*pWriteAllowedLock);
                            pThreadAllowedToWrite->wait(lk);
                        }
                        writeToLog(toRecord);

                        (*pNumWriteAllowed)++;
                        if (*pNumWriteAllowed == 4)
                            *pNumWriteAllowed = 0;
                        //printf("\n%d - Made record. new active thread is: %d", currThreadNum, *pNumWriteAllowed);
                        pNumWriteAllowedLock->unlock();
                        pThreadAllowedToWrite->notify_all();

                        return;
                    }
                    else
                    {
                        /*Wrong token*/
                        send(errMsg, serverIP, clientIP, clientPort);
                    }
                }
                else
                {
                    pCredentialsMapLock->unlock();
                    /*No such token*/
                }
            }
            else
            {
                /*Wrong message structure*/
            }
        }
        else
        {
            /*Wrong message*/
        }
    }
    else
    {
        /*Wrong message size*/
    }
    exitWithoutWriting();
}


void Task_recordMsg::writeToLog(QByteArray toWrite)
{
    if ( !pLogFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        /*Failed to open logFile*/
        return;
    }

    toWrite.prepend(QTime::currentTime().toString("hh.mm.ss.zzz ").toUtf8());
    toWrite.append("\n");
    pLogFile->write(toWrite);

    pLogFile->close();
}

void Task_recordMsg::exitWithoutWriting()
{
//    printf("%d - exit without writing", currThreadNum);
    while (true)
    {
        pNumWriteAllowedLock->lock();
        if (*pNumWriteAllowed == currThreadNum)
        {
            //printf("\n%d - allowed to increment", currThreadNum);
            break;
        }

        pNumWriteAllowedLock->unlock();

        std::unique_lock<std::mutex> lk(*pWriteAllowedLock);
        pThreadAllowedToWrite->wait(lk);
    }
    (*pNumWriteAllowed)++;
    if (*pNumWriteAllowed == 4)
        *pNumWriteAllowed = 0;

    //printf("\n%d - No record made. new active thread is: %d", currThreadNum, *pNumWriteAllowed);

    pNumWriteAllowedLock->unlock();
    pThreadAllowedToWrite->notify_all();
}
