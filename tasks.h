#ifndef TASKS_H
#define TASKS_H

#include <QRunnable>
#include <QUdpSocket>
#include <QTime>
#include <QReadWriteLock>
#include <QFile>
#include <mutex>
#include <condition_variable>

#include "hcommon.h"

class Task_makeToken : public QRunnable
{
public:
    Task_makeToken(QHostAddress *serverIP_inp,
                   QByteArray msg_inp,
                   QHostAddress clientIP_inp,
                   quint16 clientPort_inp,
                   QMap<QByteArray, uint> *pCredentialsMap_inp,
                   QReadWriteLock *pCredentialsMapLock_inp);

    void run() override;

private:
    QHostAddress *serverIP;
    QByteArray msg;
    QHostAddress clientIP;
    quint16 clientPort;
    QMap<QByteArray, uint> *pCredentialsMap;
    QReadWriteLock *pCredentialsMapLock;
};



class Task_logMsg : public QRunnable
{
public:
    Task_logMsg(QHostAddress *serverIP_inp,
                   QByteArray msg_inp,
                   QHostAddress clientIP_inp,
                   quint16 clientPort_inp,
                   QMap<QByteArray, uint> *pCredentialsMap_inp,
                   QReadWriteLock *pCredentialsMapLock_inp,
                   QFile *pLogFile_inp,
                   std::mutex *pWriteAllowedLock_inp,
                   std::condition_variable *pThreadAllowedToWrite_inp,
                   int* pNumWriteAllowed_inp,
                   std::mutex *pNumWriteAllowedLock_inp,
                   int currThreadNum_inp);

    void run() override;

private:
    QHostAddress *serverIP;
    QByteArray msg;
    QHostAddress clientIP;
    quint16 clientPort;
    QMap<QByteArray, uint> *pCredentialsMap;
    QReadWriteLock *pCredentialsMapLock;
    QFile *pLogFile;
    std::mutex *pWriteAllowedLock;
    std::condition_variable *pThreadAllowedToWrite;
    int* pNumWriteAllowed;
    std::mutex *pNumWriteAllowedLock;
    int currThreadNum;

    void writeToLog(QByteArray toWrite);
    void exitWithoutLogging();
};

#endif // TASKS_H
