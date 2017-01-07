#include "host.h"

QString Host::toString() const
{
    return addr.toString() + ":" +QString::number(port);
}
