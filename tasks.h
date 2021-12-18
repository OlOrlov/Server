#ifndef TASKS_H
#define TASKS_H

#include <QRunnable>
#include <QUdpSocket>
#include <QTime>
#include <QReadWriteLock>
#include <queue>
#include <memory>
#include <condition_variable>

#include "hcommon.h"

class Task_authorization : public QRunnable
{
public:
    Task_authorization(QHostAddress *serverIP_inp, QByteArray msg_inp,
                   QHostAddress clientIP_inp, quint16 clientPort_inp,
                   std::shared_ptr<QMap<QByteArray, uint>> pCredentialsMap_inp,
                   QReadWriteLock *pCredentialsMapLock_inp);

    void run() override;

private:
    QHostAddress *serverIP;
    QByteArray msg;
    QHostAddress clientIP;
    quint16 clientPort;
    std::shared_ptr<QMap<QByteArray, uint>> pCredentialsMap;
    QReadWriteLock *pCredentialsMapLock;
};



class Task_logMsg : public QRunnable
{
public:
    Task_logMsg(QHostAddress *serverIP_inp, QByteArray msg_inp,
                   QHostAddress clientIP_inp, quint16 clientPort_inp,
                   std::shared_ptr<QMap<QByteArray, uint>> pCredentialsMap_inp,
                   QReadWriteLock *pCredentialsMapLock_inp,
                   std::shared_ptr<std::queue<QString>> pLogQueue_inp,
                   QReadWriteLock *pLogQueueLock_inp,
                   std::condition_variable *pLogQueueChanged_inp);

    void run() override;

private:
    QHostAddress *serverIP;
    QByteArray msg;
    QHostAddress clientIP;
    quint16 clientPort;
    std::shared_ptr<QMap<QByteArray, uint>> pCredentialsMap;
    QReadWriteLock *pCredentialsMapLock;
    std::shared_ptr<std::queue<QString>> pLogQueue;
    QReadWriteLock *pLogQueueLock;
    std::condition_variable *pLogQueueChanged;
};

#endif // TASKS_H
