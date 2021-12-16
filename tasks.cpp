#include "tasks.h"
#include <QThread>
bool send(QByteArray msg, QHostAddress *serverIP, QHostAddress clientIP, quint16 clientPort)
{
    QUdpSocket xmt_sock;
    xmt_sock.bind(*serverIP, portForSending, QUdpSocket::ShareAddress);
    xmt_sock.connectToHost(clientIP, clientPort);
    if ( !xmt_sock.waitForConnected(1))
    {
        qDebug()<<("UDP connection timeout");
        return false;
    }

    qint64 r1 = xmt_sock.write(msg);
    if ( r1 != msg.length() )
    {
        qDebug()<<("Msg send failure");
        return false;
    }
    return true;
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
                               std::shared_ptr<QMap<QByteArray, uint>> pCredentialsMap_inp,
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
                               std::shared_ptr<QMap<QByteArray, uint>> pCredentialsMap_inp,
                               QReadWriteLock *pCredentialsMapLock_inp,
                               std::shared_ptr<std::queue<QString>> pLogQueue_inp,
                               QReadWriteLock *pLogQueueLock_inp,
                               std::condition_variable *pLogQueueChanged_inp)
    : serverIP(serverIP_inp), msg(msg_inp), clientIP(clientIP_inp), clientPort(clientPort_inp),
      pCredentialsMap(pCredentialsMap_inp), pCredentialsMapLock(pCredentialsMapLock_inp),
      pLogQueue(pLogQueue_inp), pLogQueueLock(pLogQueueLock_inp), pLogQueueChanged(pLogQueueChanged_inp)
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
                        pLogQueueLock->lockForWrite();
                        pLogQueue->push(QString::fromUtf8(toRecord));
                        pLogQueueLock->unlock();

                        pLogQueueChanged->notify_one();
                    }
                    else
                    {
                        qDebug()<<"Wrong token";
                        /*Wrong token*/
                        send(errMsg, serverIP, clientIP, clientPort);
                    }
                }
                else
                {
                    qDebug()<<"No such token\n";
                    pCredentialsMapLock->unlock();
                    /*No such token*/
                }
            }
            else
            {
                qDebug()<<"Wrong message structure\n";
                /*Wrong message structure*/
            }
        }
        else
        {
            qDebug()<<"Wrong message\n";
            /*Wrong message*/
        }
    }
    else
    {
        qDebug()<<"Wrong message size\n";
        /*Wrong message size*/
    }
}
