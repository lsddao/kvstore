#include "server.h"
#include "workerthread.h"
#include "../common/LocalKeyValueProvider.h"

#include <QTimer>

Server::Server(QObject *parent)
    : QTcpServer(parent)
	, _store(new LocalKeyValueProvider)
{
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &Server::printStats);
	timer->start(5000);
}

Server::~Server()
{
}

void Server::incomingConnection(qintptr socketDescriptor)
{
	qInfo() << "incomingConnection at socket" << socketDescriptor;

    WorkerThread *th = new WorkerThread(socketDescriptor, _store.data());
	connect(th, &WorkerThread::finished, th, &WorkerThread::deleteLater);
	connect(th, &WorkerThread::finished, [th]() { qDebug() << "thread" << th << "terminated"; });
	connect(th, &WorkerThread::error, [](QTcpSocket::SocketError err) { qCritical() << "socket error" << err; });

    th->start();
}

void Server::printStats()
{
	qInfo() << "Server stats:" << _store->count() << "items in cache";
}
