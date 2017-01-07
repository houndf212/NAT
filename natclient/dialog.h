#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtNetwork>
#include "host.h"
#include "udpsocket.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
private:
    Q_SLOT void onBtnAsk();
    Q_SLOT void onBtnCon();
    Q_SLOT void onBtnHole();
    Q_SLOT void onBtnSendDataGram();

    Q_SLOT void onUdpData();

private:
    void doCon();
    void disCon();
    Host getServerHost() const;
    void send_packet(const Host& h, const Packet &p);
    void logMsg(const QString& msg);

    void hand_packet(const DataPacket& p);
    void hand_ask_connect(const DataPacket& p);
    void hand_rep_connect(const DataPacket& p);
    void hand_ask_heart(const DataPacket& p);
    void hand_ask_name(const DataPacket& p);
    void hand_ask_local_addr(const DataPacket& p);
    void hand_rep_user(const DataPacket& p);
    void hand_ask_hole(const DataPacket& p);
    void hand_custom(const DataPacket& dp);
private:
    Ui::Dialog *ui;
    UdpSocket* socket;
    QMap<QString, Host> user;
};

#endif // DIALOG_H
