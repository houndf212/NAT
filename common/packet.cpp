#include "packet.h"

const QString Packet::key_id = "packet_type";

Packet Packet::fromData(const QByteArray &data)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error!=QJsonParseError::NoError)
    {
        return Packet(Packet::None);
    }
    QJsonObject obj = doc.object();
    Packet::Type pid = Packet::Type(obj.value(key_id).toInt(0));
    Packet p(pid);
    p.obj = obj;
    p.obj.insert("recv_time",QDateTime::currentDateTime().toString(Qt::ISODate));
    return p;
}

Packet::Packet(Packet::Type id)
    :pid(id)
{
    setVaue("send_time", QDateTime::currentDateTime().toString(Qt::ISODate));
}

QByteArray Packet::toData() const
{
    QJsonObject o = obj;
    o.insert(key_id, pid);
    QJsonDocument doc;
    doc.setObject(o);
    return doc.toJson(QJsonDocument::Compact);
}
