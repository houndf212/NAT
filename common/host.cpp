#include "host.h"

bool operator<(const Host &h1, const Host &h2)
{
    quint32 ip1 = h1.addr.toIPv4Address();
    quint32 ip2 = h2.addr.toIPv4Address();

    if (ip1<ip2)
    { return true; }
    else if (ip1==ip2)
    { return h1.port<h2.port; }
    else
    { return false; }
}


QString Host::toString() const
{
    return addr.toString() + ":" +QString::number(port);
}
