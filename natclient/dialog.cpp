#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->btnAsk, &QPushButton::clicked, this, &Dialog::onBtnAsk);
    connect(ui->btnConnect, &QPushButton::clicked, this, &Dialog::onBtnCon);
    connect(ui->btnHole, &QPushButton::clicked, this, &Dialog::onBtnHole);
    connect(ui->btnSendDatagram, &QPushButton::clicked, this, &Dialog::onBtnSendDataGram);

    socket = new UdpSocket(this);
    socket->setSocket(new QUdpSocket);
    connect(socket, &UdpSocket::sig_packet, this, &Dialog::onUdpData);

    ui->txtName->setText("wc");
    ui->txtIP->setText("192.168.4.40");
    ui->numPort->setValue(2333);

    QList<QHostAddress> lst = QNetworkInterface::allAddresses();
    for (int i=0; i<lst.size(); ++i)
    {
        QHostAddress t = lst.at(i);
        if (!t.isLoopback() && t.toIPv4Address())
        {
            ui->txtIPs->addItem(t.toString());
        }
    }
}

Dialog::~Dialog()
{
    disCon();
    delete ui;
}

void Dialog::onBtnAsk()
{
    QString username = ui->txtAksName->text().trimmed();
    Packet p(Packet::ASK_User_Addr);
    p.setVaue("name", username);
    send_packet(getServerHost(), p);
}

void Dialog::onBtnCon()
{
    doCon();
}

void Dialog::onBtnHole()
{
    QString name = ui->txtAksName->text();
    Packet p(Packet::ASK_Hole_User);
    p.setVaue("name", name);
    send_packet(getServerHost(), p);
}

void Dialog::onBtnSendDataGram()
{
    QString name = ui->txtAksName->text().trimmed();
    QString msg = ui->txtDataGram->text().trimmed();
    if (msg.isEmpty())
        return;

    if (!user.contains(name))
        return;

    Host host = user.value(name);
    Packet p(Packet::Custom_Data);
    p.setVaue("msg", msg);
    send_packet(host, p);
}

void Dialog::onUdpData()
{
    while(socket->hasPacket())
    {
        DataPacket p = socket->nextPacket();
        qDebug() << "recv: "<<p.host << ", packet id: " << p.packet.type();
        logMsg("recv: "+p.host.toString()+", packet id: " + QString::number(p.packet.type()));
        hand_packet(p);
    }
}

void Dialog::doCon()
{
    Packet p(Packet::ASK_Connect);
    send_packet(getServerHost(), p);
}

void Dialog::disCon()
{
    Packet p (Packet::ASK_Disconnect);
    send_packet(getServerHost(), p);
    // gcc cannot alowed!
//    send_packet(getServerHost(), Packet(Packet::ASK_Disconnect));
}

Host Dialog::getServerHost() const
{
    Host host;
    host.addr = ui->txtIP->text().trimmed();
    host.port = ui->numPort->value();
    return host;
}

void Dialog::send_packet(const Host &h, Packet &p)
{
    Q_ASSERT(p.isValid());
    socket->sendPacket(h, p);
}

void Dialog::logMsg(const QString &msg)
{
    QString txt = QTime::currentTime().toString()+":"+msg;
    ui->txtLog->appendPlainText(txt);
}

void Dialog::hand_packet(const DataPacket &p)
{
    switch(p.packet.type())
    {
    case Packet::REP_NoSession:
        doCon();
        break;
    case Packet::ASK_Connect:
        hand_ask_connect(p);
        break;
    case Packet::REP_Connect:
        hand_rep_connect(p);
        break;
    case Packet::ASK_HeartBeat:
        hand_ask_heart(p);
        break;
    case Packet::ASK_Name:
        hand_ask_name(p);
        break;
    case Packet::ASK_Local_Addr:
        hand_ask_local_addr(p);
        break;
    case Packet::REP_User_Addr:
        hand_rep_user(p);
        break;
    case Packet::ASK_Hole_User:
        hand_ask_hole(p);
        break;
    case Packet::Custom_Data:
        hand_custom(p);
        break;
    case Packet::None:
    default:
        return;
        break;
    }
}

void Dialog::hand_ask_connect(const DataPacket &p)
{
    Packet sp(Packet::REP_Connect);
    sp.setVaue("msg", "hi, I am " + ui->txtName->text().trimmed());
    send_packet(p.host, sp);
    QString name = p.packet.getValue("name").toString();
    logMsg("coming: " + name);
    if (!user.contains(name))
    {
        user.insert(name, p.host);
    }
}

void Dialog::hand_rep_connect(const DataPacket &p)
{
    logMsg(p.packet.getValue("msg").toString());
    ui->txtName->setEnabled(false);
    ui->txtIPs->setEnabled(false);
    ui->btnConnect->setEnabled(false);
}

void Dialog::hand_ask_heart(const DataPacket &p)
{
    Packet sp(Packet::REP_HeartBeat);
    send_packet(p.host, sp);
}

void Dialog::hand_ask_name(const DataPacket &p)
{
    Packet sp(Packet::REP_Name);
    sp.setVaue("name", ui->txtName->text().trimmed());
    send_packet(p.host, sp);
}

void Dialog::hand_ask_local_addr(const DataPacket &p)
{
    QString local_ip = ui->txtIPs->currentText();
    Packet sp(Packet::REP_Local_Addr);
    sp.setVaue("ip", local_ip);
    sp.setVaue("port", socket->getQUdpSocket()->localPort());
    send_packet(p.host, sp);
}

void Dialog::hand_rep_user(const DataPacket &p)
{
    ui->txtPeerIP_Remote->setText(p.packet.getValue("remote_ip").toString());
    ui->numPeerPort_Remote->setValue(p.packet.getValue("remote_port").toInt());

    ui->txtPeerIP_Local->setText(p.packet.getValue("local_ip").toString());
    ui->numPeerPort_Local->setValue(p.packet.getValue("local_port").toInt());
}

void Dialog::hand_ask_hole(const DataPacket &p)
{
    Host rhost, lhost;
    rhost.addr = QHostAddress(p.packet.getValue("remote_ip").toString());
    rhost.port = p.packet.getValue("remote_port").toInt();

    lhost.addr = QHostAddress(p.packet.getValue("local_ip").toString());
    lhost.port = p.packet.getValue("local_port").toInt();

    Packet cp(Packet::ASK_Connect);
    cp.setVaue("name", ui->txtName->text().trimmed());
    send_packet(lhost, cp);
    send_packet(rhost, cp);
}

void Dialog::hand_custom(const DataPacket &dp)
{
    logMsg(dp.packet.getValue("msg").toString());
}
