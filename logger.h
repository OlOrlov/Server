#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <memory>

#include "hcommon.h"
#include "server.h"
#include "tasks.h"

class Logger : public QThread
{
public:
    Logger(std::shared_ptr<std::queue<QString>> pLogQueue_inp,
           QReadWriteLock *pLogQueueLock_inp);
    void run();

private:
    std::shared_ptr<std::queue<QString>> pLogQueue;
    QReadWriteLock *pLogQueueLock;
};

#endif // LISTENER_H
