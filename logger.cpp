#include "logger.h"

Logger::Logger(std::shared_ptr<std::queue<QString>> pLogQueue_inp,
               QReadWriteLock *pLogQueueLock_inp,
               std::mutex *logQueueMtx_inp,
               std::condition_variable *pLogQueueChanged_inp)
{
    auto fileName = QApplication::applicationDirPath() + "/Log.txt";
    logFile.setFileName(fileName);
    logFile.resize(0);

    pLogQueue = pLogQueue_inp;
    pLogQueueLock = pLogQueueLock_inp;
    logQueueMtx = logQueueMtx_inp;
    pLogQueueChanged = pLogQueueChanged_inp;
}

Logger::~Logger()
{
    if (logFile.isOpen())
        logFile.close();
}

void Logger::run()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(*logQueueMtx);
        pLogQueueChanged->wait(lk);

        assert(logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text) == true);

        pLogQueueLock->lockForWrite();
        while ( !pLogQueue->empty())
        {
            auto toRecord = pLogQueue->front();
            pLogQueue->pop();
            pLogQueueLock->unlock();

            logFile.write((QTime::currentTime().toString("hh.mm.ss.zzz") + " " + toRecord + "\n").toUtf8());

            pLogQueueLock->lockForWrite();
        }
        pLogQueueLock->unlock();
        logFile.close();
        lk.unlock();
    }
}
