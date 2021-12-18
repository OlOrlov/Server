#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    printf("Set IP address for server: ");

    QRegExp ipControl("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\."
                      "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\."
                      "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\."
                      "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    QTextStream s(stdin);

    QString serverIP;
    printf("Set server IP: ");
    while (true)
    {
        serverIP = s.readLine();
        if (ipControl.exactMatch(serverIP))
            break;
        else
            printf("Incorrect IP\nSet server IP: ");
    }

    Server server(serverIP);

    server.start();

    return a.exec();
}
