#include "server.h"

Server::Server(QString ip)
{
    server_ip.setAddress(ip);

    auto fileName = QApplication::applicationDirPath() + "/Log.txt";
    logFile.setFileName(fileName);
    logFile.resize(0);
}

void Server::start()
{
    QUdpSocket auth_rcv_sock;
    QUdpSocket logRecord_rcv_sock;
    qDebug() << "\nServer created on IP" << server_ip.toString() <<
                "\nPort for authorization:" << portForAuthorization <<
                "\nPort for writing to log:" << portForLogRecord;

    bool setupSuccess = auth_rcv_sock.bind(server_ip, portForAuthorization, QUdpSocket::DontShareAddress);
    setupSuccess &= logRecord_rcv_sock.bind(server_ip, portForLogRecord, QUdpSocket::DontShareAddress);

    if (setupSuccess)
    {
        printf("Initiation successful\n");

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
                                                        &credentialsMap,
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
                                                 &credentialsMap,
                                                 &credentialsMapLock,
                                                 &logFile,
                                                 &logFileLock));
            }
        }
    }
    else
    {
        qDebug() << "Failed to bind sockets." <<
                     auth_rcv_sock.errorString() <<
                     logRecord_rcv_sock.errorString();
    }
}
