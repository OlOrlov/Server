#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <memory>
#include <condition_variable>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <cassert>

#include "hcommon.h"
#include "server.h"
#include "tasks.h"

class Logger : public QThread
{
public:
    Logger(std::shared_ptr<std::queue<QString>> pLogQueue_inp,
           QReadWriteLock *pLogQueueLock_inp,
           std::mutex *logQueueMtx_inp,
           std::condition_variable *pLogQueueChanged_inp);
    ~Logger();
    void run();

private:
    QFile logFile;
    std::shared_ptr<std::queue<QString>> pLogQueue;
    QReadWriteLock *pLogQueueLock;
    std::mutex *logQueueMtx;
    std::condition_variable *pLogQueueChanged;
};

#endif // LISTENER_H
