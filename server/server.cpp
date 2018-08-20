#include "server.h"
#include "workerthread.h"
#include "../common/LocalKeyValueProvider.h"
#include "../common/PersistentKeyValueStorage.h"
#include "utils.h"

#include <QTimer>

Server::Server(QObject *parent)
	: QTcpServer(parent)
	, _store(new LocalKeyValueProvider)
{
	static_cast<LocalKeyValueProvider*>(_store.data())->setUnderlyingProvider(std::make_unique<PersistentKeyValueStorage>());

	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &Server::printStats);
	timer->start(5000);
}

Server::~Server()
{
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
	utils::logInfo(QString("Server stats: %1 items in cache").arg(_store->count()));
}
