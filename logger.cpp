#include "logger.h"

Logger::Logger(std::shared_ptr<std::queue<QString>> pLogQueue_inp,
               QReadWriteLock *pLogQueueLock_inp)
{
    pLogQueue = pLogQueue_inp;
    pLogQueueLock = pLogQueueLock_inp;
}

void Logger::run()
{
    while (true)
    {
        pLogQueueLock->lockForRead();
        if ( !pLogQueue->empty())
        {
            qDebug() << "TORECORD";
            qDebug() << pLogQueue->front();
        }
        QThread::msleep(5);
        pLogQueueLock->unlock();
    }
}
