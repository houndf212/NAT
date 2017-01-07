#include "udpsocket.h"

UdpSocket::UdpSocket(QObject *parent)
    :QObject(parent)
    ,socket(0)
{

}

void UdpSocket::setSocket(QUdpSocket *s)
{
    Q_ASSERT(socket==0);
    socket = s;
    socket->setParent(this);
    connect(socket, &QUdpSocket::readyRead, this, &UdpSocket::sig_packet);
}

bool UdpSocket::hasPacket() const
{
    return socket->hasPendingDatagrams();
}

DataPacket UdpSocket::nextPacket()
{
    Q_ASSERT(hasPacket());
    DataPacket p;
    QByteArray buffer(socket->pendingDatagramSize(), 0);
    socket->readDatagram(buffer.data(), buffer.size(), &p.host.addr, &p.host.port);
    p.packet = Packet::fromData(buffer);
    return p;
}

quint64 UdpSocket::sendPacket(const Host &host, const Packet &p)
{
    Q_ASSERT(p.isValid());
    return socket->writeDatagram(p.toData(), host.addr, host.port);
}

