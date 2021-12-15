#include "server.h"

Server::Server(QString ip)
{
    server_ip.setAddress(ip);
    pCredentialsMap = std::make_shared<QMap<QByteArray, uint>>(credentialsMap);
    pLogQueue = std::make_shared<std::queue<QString>>(logQueue);
}

void Server::start()
{
    QUdpSocket auth_rcv_sock;
    QUdpSocket logRecord_rcv_sock;
    qDebug() << server_ip.toString() << portForAuthorization << portForLogRecord;
    bool setupSuccess = auth_rcv_sock.bind(server_ip, portForAuthorization, QUdpSocket::DontShareAddress) &&
         logRecord_rcv_sock.bind(server_ip, portForLogRecord, QUdpSocket::DontShareAddress);

    if (setupSuccess)
    {
        printf("Initiation successful\n");
        Logger logger(pLogQueue, &logQueueLock, &logQueueMtx, &logQueueChanged);
        logger.start();

        while (true)
        {
            if (auth_rcv_sock.hasPendingDatagrams())
            {
                QByteArray received;
                received.resize(auth_rcv_sock.pendingDatagramSize());
                QHostAddress client_ip;
                quint16 client_port = 0;
                auth_rcv_sock.readDatagram(received.data(), received.size(), &client_ip, &client_port);
                qDebug() << "8000 RCV from" << client_ip << client_port;//TO_DELETE
                threadPool.start(new Task_makeToken(&server_ip, received, client_ip, client_port,
                                                    pCredentialsMap, &credentialsMapLock));
            }

            if (logRecord_rcv_sock.hasPendingDatagrams())
            {
                QByteArray received;
                received.resize(logRecord_rcv_sock.pendingDatagramSize());
                QHostAddress client_ip;
                quint16 client_port = 0;
                logRecord_rcv_sock.readDatagram(received.data(), received.size(), &client_ip, &client_port);
                qDebug() << "8001 RCV from" << client_ip << client_port;//TO_DELETE
                threadPool.start(new Task_recordMsg(&server_ip, received, client_ip, client_port,
                                                    pCredentialsMap, &credentialsMapLock,
                                                    pLogQueue, &logQueueLock, &logQueueChanged));
            }
        }
    }
}
