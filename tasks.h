#ifndef TASKS_H
#define TASKS_H

#include <QRunnable>
#include <QUdpSocket>
#include <QTime>
#include <QReadWriteLock>
#include <queue>
#include <memory>

#include "hcommon.h"

class Task_makeToken : public QRunnable
{
public:
    Task_makeToken(QHostAddress *serverIP_inp, QByteArray msg_inp,
                   QHostAddress clientIP_inp, quint16 clientPort_inp,
                   std::shared_ptr<QHash<QByteArray, uint>> pCredentialsMap_inp,
                   QReadWriteLock *pCredentialsMapLock_inp);

    void run() override;

private:
    QHostAddress *serverIP;
    QByteArray msg;
    QHostAddress clientIP;
    quint16 clientPort;
    std::shared_ptr<QHash<QByteArray, uint>> pCredentialsMap;
    QReadWriteLock *pCredentialsMapLock;
};



class Task_recordMsg : public QRunnable
{
public:
    Task_recordMsg(QHostAddress *serverIP_inp, QByteArray msg_inp,
                   QHostAddress clientIP_inp, quint16 clientPort_inp,
                   std::shared_ptr<QHash<QByteArray, uint>> pCredentialsMap_inp,
                   QReadWriteLock *pCredentialsMapLock_inp,
                   std::shared_ptr<std::queue<QString>> pLogQueue_inp,
                   QReadWriteLock *pLogQueueLock_inp);

    void run() override;

private:
    QHostAddress *serverIP;
    QByteArray msg;
    QHostAddress clientIP;
    quint16 clientPort;
    std::shared_ptr<QHash<QByteArray, uint>> pCredentialsMap;
    QReadWriteLock *pCredentialsMapLock;
    std::shared_ptr<std::queue<QString>> pLogQueue;
    QReadWriteLock *pLogQueueLock;
};

#endif // TASKS_H