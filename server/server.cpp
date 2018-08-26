#include "server.h"
#include "workerthread.h"
#include "../common/LocalKeyValueProvider.h"
#include "utils.h"

#include <QTimer>

Server::Server(QObject *parent)
	: QTcpServer(parent)
	, _store(new LocalKeyValueProvider)
{
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &Server::printStats);
	timer->start(5000);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
	utils::logInfo(QString("IncomingConnection at socket %1").arg(socketDescriptor));

	WorkerThread *th = new WorkerThread(socketDescriptor, _store.data());
	connect(th, &WorkerThread::finished, th, &WorkerThread::deleteLater);
	connect(th, &WorkerThread::finished, [th]()  { qDebug() << "Thread" << th << "terminated";	});
	connect(th, &WorkerThread::error, [](QTcpSocket::SocketError err) { qCritical() << "socket error" << err; });

	th->start();
	qDebug() << "Thread" << th << "started";
}

void Server::printStats()
{
	utils::logInfo(QString("Server stats: %1 items in cache, occupied approx. %2 bytes").arg(_store->count()).arg(_store->cacheMemoryOccupied()));
}
