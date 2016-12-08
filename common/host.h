#ifndef HOST_H
#define HOST_H

#include <QHostAddress>

struct Host
{
    QHostAddress addr;
    quint16 port;

    Host() : port(0) {}
    QString toString() const;
};

bool operator <(const Host& h1, const Host& h2);

inline bool operator ==(const Host& h1, const Host& h2)
{
    return h1.addr == h2.addr && h1.port == h2.port;
}

inline bool operator !=(const Host& h1, const Host& h2)
{
    return !(h1==h2);
}

inline QDebug& operator<<(QDebug& os, const Host& h)
{
    return os << h.toString();
}

inline uint qHash(const Host& h)
{
    quint64 n = h.addr.toIPv4Address();
    n <<= 32;
    n += h.port;
    return qHash(n);
}

#endif // HOST_H
