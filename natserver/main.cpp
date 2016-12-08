#include <QCoreApplication>
#include "server.h"
#include "udpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    Server s;
    UdpServer us;
    return a.exec();
}
