#ifndef PACKET_H
#define PACKET_H

#include <QtCore>
#include <new>
class Packet
{
public:
    enum Type
    {
        None,

        REP_NoSession,

        ASK_Connect,
        REP_Connect,

        ASK_Disconnect,

        ASK_HeartBeat,
        REP_HeartBeat,

        ASK_Name,
        REP_Name,

        ASK_Local_Addr,
        REP_Local_Addr,

        ASK_User_Addr,
        REP_User_Addr,

        ASK_Hole_User,
        Custom_Data,
    };
    static const QString key_id;
    static Packet fromData(const QByteArray& data);
public:
    Packet(Packet::Type id = Packet::None);
    Packet::Type type() const { return pid; }
    bool isValid() const { return pid != Packet::None; }
    QByteArray toData() const;

    QJsonValue getValue(const QString& key) const { return obj.value(key); }
    void setVaue(const QString& key, const QJsonValue& val) { obj.insert(key, val); }
private:
    Packet::Type pid;
    QJsonObject obj;
public:
    static void* operator new(std::size_t ) = delete;
    static void* operator new(std::size_t, void*) = delete;
};

#endif // PACKET_H
