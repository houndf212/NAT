#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include <QUdpSocket>
#include <memory>
#include "host.h"
#include "packet.h"

struct DataPacket
{
    Host host;
    Packet packet;
};

class UdpSocket : public QObject
{
    Q_OBJECT
public:
    explicit UdpSocket(QObject* parent=0);
    // take ownership of s
    void setSocket(QUdpSocket* s);
    QUdpSocket* getQUdpSocket() const { return socket; }
    Q_SIGNAL void sig_packet();
    bool hasPacket() const;
    DataPacket nextPacket();
    quint64 sendPacket(const Host& host, const Packet& p);
private:
    QUdpSocket *socket;
};

#endif // UDPSOCKET_H
