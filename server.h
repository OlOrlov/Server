#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>// TO_DELETE?
#include "hcommon.h"
#include "logger.h"
#include <mutex>// TO_DELETE?
#include <condition_variable>
#include <queue>
#include <QThreadPool>
#include <memory>
#include <QReadWriteLock>

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
    std::shared_ptr<QMap<QByteArray, uint>> pCredentialsMap;
    QReadWriteLock credentialsMapLock;

    std::queue<QString> logQueue;
    std::shared_ptr<std::queue<QString>> pLogQueue;
    QReadWriteLock logQueueLock;
    std::condition_variable logQueueChanged;
    std::mutex logQueueMtx;
};

#endif // SERVER_H
