#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server("127.0.0.1");

    server.start();

    return a.exec();
}
