#ifndef HOST_INFO_H
#define HOST_INFO_H

#include <QDateTime>
#include "host.h"

struct Host_Info
{
    Host local_host;
    QString user_name;
    QDateTime update_time;
};

#endif // HOST_INFO_H
