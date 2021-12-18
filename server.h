#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <mutex>
#include <QThreadPool>
#include <QReadWriteLock>
#include <QApplication>
#include <QFile>

#include "hcommon.h"
#include "tasks.h"

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QString ip);

    void start();

private:
    QHostAddress server_ip;
    QThreadPool threadPool;

    QMap<QByteArray, uint> credentialsMap;
    QReadWriteLock credentialsMapLock;

    QFile logFile;
    std::mutex logFileLock;
};

#endif // SERVER_H
