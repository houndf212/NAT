#include "server.h"
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>

Server::Server(QObject *parent)
    :QObject(parent)
{
    server_socket = new QTcpServer(this);
    connect(server_socket, &QTcpServer::newConnection,
            this, &Server::onClientComing);
    bool b =server_socket->listen(QHostAddress::Any, 7778);
    if (b)
    {
        qDebug() << "listen at: "<<server_socket->serverPort();
    }
    else
    {
        qDebug() << "listen failed!";
        qApp->exit(-1);
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Server::onTimeout);
    timer->start(5*1000);
}

void Server::onTimeout()
{
    QList<QTcpSocket*> lst;
    for (auto it=socket_map.begin(); it!=socket_map.end(); ++it)
    {
        if (it.value().isEmpty())
        { lst.append(it.key()); }
    }

    qDebug() << "===============status==============";
    qDebug() << "all client size: "<< socket_map.size();
    qDebug() << "no name client: " << lst.size();


    for (auto it=lst.cbegin(); it!=lst.cend(); ++it)
    {
        QTcpSocket* s = *it;
        socket_map.remove(s);
        qDebug() << "remove: "<<s->peerAddress()<<":"<<s->peerPort();
        s->disconnect(this);
        s->deleteLater();
    }

    for(auto it=socket_map.begin(); it!=socket_map.end(); ++it)
    {
        it.value() = "";
        askName(it.key());
    }
}

void Server::onClientComing()
{
    while(server_socket->hasPendingConnections())
    {
        QTcpSocket* s = server_socket->nextPendingConnection();
        qDebug() << "incoming: " << s;
        Q_ASSERT(s!=nullptr);
        connect(s, &QTcpSocket::disconnected, this, &Server::onClose);
        connect(s, &QTcpSocket::readyRead, this, &Server::onDataReady);
        socket_map.insert(s, "");
        askName(s);
    }
}

void Server::onDataReady()
{
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    QByteArray buffer = s->readAll();
    qDebug() <<s->peerAddress()<<":"<<s->peerPort() << " "<<buffer;
    if (buffer.startsWith("name:"))
    {
        buffer.remove(0, 5);
        socket_map[s] = buffer;
    }
}

void Server::onClose()
{
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "remove: "<<s->peerAddress()<<":"<<s->peerPort();
    s->deleteLater();
    socket_map.remove(s);
}

void Server::askName(QTcpSocket *s)
{
    s->write("name");
}
