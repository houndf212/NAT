#ifndef UDPSERVER_H
#define UDPSERVER_H
#include <QUdpSocket>
#include <QTimer>
#include "host_info.h"
#include "udpsocket.h"

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject* parent=0);
private:
    Q_SLOT void onDataReady();
    Q_SLOT void onTimeout();
    void send_packet(const Host& h, const Packet &p);
    void ask_heart(const Host& h);

    void hand_packet(const DataPacket& p);
    bool check_session(const DataPacket& p);
    void hand_connect(const DataPacket& p);
    void hand_disconnect(const DataPacket& p);
    void hand_ask_user(const DataPacket& p);
    void hand_ask_hole(const DataPacket& p);
private:
    UdpSocket *socket;
    QTimer* timer;
    QHash<Host, Host_Info> all_user;
};

#endif // UDPSERVER_H
