#ifndef SERVER_H
#define SERVER_H
#include <QtCore>
#include <QtNetwork>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject* parent = 0);
private:
    Q_SLOT void onTimeout();
    Q_SLOT void onClientComing();
    Q_SLOT void onDataReady();
    Q_SLOT void onClose();

    void askName(QTcpSocket* s);
private:
    QTimer *timer;
    QTcpServer *server_socket;
    QMap<QTcpSocket*, QString> socket_map;
};

#endif // SERVER_H
