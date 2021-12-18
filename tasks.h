#ifndef TASKS_H
#define TASKS_H

#include <QRunnable>
#include <QUdpSocket>
#include <QTime>
#include <QReadWriteLock>
#include <QFile>
#include <mutex>
#include "hcommon.h"

class Task_authorization : public QRunnable
{
public:
    Task_authorization(QHostAddress *serverIP_inp,
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
                std::mutex *pLogFileLock_inp);

    void run() override;

private:
    QHostAddress *serverIP;
    QByteArray msg;
    QHostAddress clientIP;
    quint16 clientPort;
    QMap<QByteArray, uint> *pCredentialsMap;
    QReadWriteLock *pCredentialsMapLock;
    QFile *pLogFile;
    std::mutex *pLogFileLock;

    void writeToLog(QByteArray toWrite);
};

#endif // TASKS_H
