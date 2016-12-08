#include "udpserver.h"

UdpServer::UdpServer(QObject *parent)
    :QObject(parent)
{
    QUdpSocket* udp = new QUdpSocket;
    bool b  = udp->bind(2333);
    if (!b)
    {
        qDebug() <<"listen failed! -- " <<udp->errorString();
        exit(-1);
    }

    socket = new UdpSocket(this);
    socket->setSocket(udp);
    connect(socket, &UdpSocket::sig_packet, this, &UdpServer::onDataReady);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &UdpServer::onTimeout);
    timer->start(10*1000);
}

void UdpServer::onDataReady()
{
    while(socket->hasPacket())
    {
        DataPacket p = socket->nextPacket();
        qDebug() << "recv: "<<p.host << ", packet id: " << p.packet.type();
        hand_packet(p);
    }
}

void UdpServer::onTimeout()
{
    qDebug() << "========================";
    QDateTime d = QDateTime::currentDateTime().addMSecs(-2*timer->interval());
    QList<Host> lst;
    for (auto it=all_user.cbegin(); it!=all_user.cend(); ++it)
    {
        if (it.value().update_time < d)
            lst.append(it.key());
    }

    qDebug() << "time out host size: "<<lst.size();
    for (auto it=lst.cbegin(); it!=lst.cend(); ++it)
    {
        all_user.remove(*it);
    }

    qDebug() << "ask heart beat size: "<<all_user.size();
    for (auto it=all_user.cbegin(); it!=all_user.cend(); ++it)
    {
        ask_heart(it.key());
    }
}

void UdpServer::hand_packet(const DataPacket &p)
{
    if (!check_session(p))
    { return; }
    switch(p.packet.type())
    {
    case Packet::ASK_Disconnect:
        hand_disconnect(p);
        break;

    case Packet::REP_HeartBeat:
        all_user[p.host].update_time = QDateTime::currentDateTime();
        break;

    case Packet::REP_Name:
        all_user[p.host].user_name = p.packet.getValue("name").toString();
        break;

    case Packet::REP_Local_Addr:
        all_user[p.host].local_host.addr = p.packet.getValue("ip").toString();
        all_user[p.host].local_host.port = p.packet.getValue("port").toInt();
        break;
    case Packet::ASK_User_Addr:
        hand_ask_user(p);
        break;
    case Packet::ASK_Hole_User:
        hand_ask_hole(p);
        break;

    case Packet::None:
    default:
        return;
        break;
    }
}

bool UdpServer::check_session(const DataPacket &p)
{
    if (all_user.contains(p.host))
    { return true; }
    else
    {
        if (p.packet.type() == Packet::ASK_Connect)
        {
            hand_connect(p);
        }
        Packet sp(Packet::REP_NoSession);
        send_packet(p.host, sp);
        return false;
    }
}

void UdpServer::ask_heart(const Host &h)
{
    Packet p(Packet::ASK_HeartBeat);
    send_packet(h, p);
}

void UdpServer::hand_connect(const DataPacket& p)
{
    const Host& host = p.host;
    if (!all_user.contains(host))
    {
        all_user.insert(host, Host_Info());
        all_user[host].update_time = QDateTime::currentDateTime();
    }

    Packet p1(Packet::REP_Connect);
    p1.setVaue("msg", "welcome to server!");
    send_packet(host, p1);

    Packet p2(Packet::ASK_Name);
    send_packet(host, p2);

    Packet p3(Packet::ASK_Local_Addr);
    send_packet(host, p3);
}

void UdpServer::hand_disconnect(const DataPacket &p)
{
    all_user.remove(p.host);
}

void UdpServer::hand_ask_user(const DataPacket& p)
{
    QString name = p.packet.getValue("name").toString();
    Packet pk(Packet::REP_User_Addr);
    if (!name.isEmpty())
    {
        for (auto it=all_user.cbegin(); it!=all_user.cend(); ++it)
        {
            if (it.value().user_name == name)
            {
                pk.setVaue("name", it.value().user_name);
                pk.setVaue("remote_ip", it.key().addr.toString());
                pk.setVaue("remote_port", it.key().port);
                pk.setVaue("local_ip", it.value().local_host.addr.toString());
                pk.setVaue("local_port", it.value().local_host.port);
                break;
            }
        }
    }

    send_packet(p.host, pk);
}

void UdpServer::hand_ask_hole(const DataPacket& p)
{
    QString name = p.packet.getValue("name").toString();
    Host_Info info = all_user.value(p.host);
    Host host1;
    Host_Info info1;
    for (auto it=all_user.cbegin(); it!=all_user.cend(); ++it)
    {
        if (it.value().user_name == name)
        {
            host1 = it.key();
            info1 = it.value();
            break;
        }
    }
    if (info1.user_name != name)
        return;

    Packet p2(Packet::ASK_Hole_User);
    p2.setVaue("remote_ip", p.host.addr.toString());
    p2.setVaue("remote_port", p.host.port);
    p2.setVaue("local_ip", info.local_host.addr.toString());
    p2.setVaue("local_port", info.local_host.port);
    p2.setVaue("name", info1.user_name);

    Packet p1(Packet::ASK_Hole_User);
    p1.setVaue("remote_ip", host1.addr.toString());
    p1.setVaue("remote_port", host1.port);
    p1.setVaue("local_ip", info1.local_host.addr.toString());
    p1.setVaue("local_port", info1.local_host.port);
    p1.setVaue("name", info.user_name);

    send_packet(p.host, p1);
    send_packet(host1, p2);
}

void UdpServer::send_packet(const Host &h, Packet &p)
{
    Q_ASSERT(p.isValid());
    socket->sendPacket(h, p);
}
