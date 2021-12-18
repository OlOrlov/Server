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

    qDebug() << "\nServer created on IP" << server_ip.toString() <<
                "\nPort for authorization:" << portForAuthorization <<
                "\nPort for writing to log:" << portForLogRecord;

    bool setupSuccess = auth_rcv_sock.bind(server_ip, portForAuthorization, QUdpSocket::DontShareAddress) &&
         logRecord_rcv_sock.bind(server_ip, portForLogRecord, QUdpSocket::DontShareAddress);

    if (setupSuccess)
    {
        printf("Initiation successful\n");
        Logger logger(pLogQueue, &logQueueLock, &logQueueMtx, &logQueueChanged);
        logger.start();

        /* * * LISTENING PORTS * * */
        while (true)
        {
            if (auth_rcv_sock.hasPendingDatagrams())
            {
                QByteArray received;
                received.resize(auth_rcv_sock.pendingDatagramSize());
                QHostAddress client_ip;
                quint16 client_port = 0;
                auth_rcv_sock.readDatagram(received.data(), received.size(), &client_ip, &client_port);
                threadPool.start(new Task_authorization(&server_ip,
                                                        received,
                                                        client_ip,
                                                        client_port,
                                                        pCredentialsMap,
                                                        &credentialsMapLock));
            }

            if (logRecord_rcv_sock.hasPendingDatagrams())
            {
                QByteArray received;
                received.resize(logRecord_rcv_sock.pendingDatagramSize());
                QHostAddress client_ip;
                quint16 client_port = 0;
                logRecord_rcv_sock.readDatagram(received.data(), received.size(), &client_ip, &client_port);

                threadPool.start(new Task_logMsg(&server_ip,
                                                 received,
                                                 client_ip,
                                                 client_port,
                                                 pCredentialsMap,
                                                 &credentialsMapLock,
                                                 pLogQueue,
                                                 &logQueueLock,
                                                 &logQueueChanged));
            }
        }
    }
}
