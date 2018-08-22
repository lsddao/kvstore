#include "workerthread.h"

#include <QtNetwork>

WorkerThread::WorkerThread(qintptr socketDescriptor, IKeyValueProvider* store)
	: _socketDescriptor(socketDescriptor)
	, _store(store)
{

}

void WorkerThread::run()
{
	_socket.reset(new QTcpSocket);

	if (!_socket->setSocketDescriptor(_socketDescriptor))
	{
		emit error(_socket->error());
		return;
	}

	_strm.reset(new QDataStream);
	_strm->setDevice(_socket.data());
	_strm->setVersion(QDataStream::Qt_5_10);

	connect(_socket.data(), &QTcpSocket::disconnected, this, &WorkerThread::quit, Qt::DirectConnection);
	connect(_socket.data(), &QTcpSocket::readyRead, this, &WorkerThread::onReadyRead, Qt::DirectConnection);

	exec();
}

void WorkerThread::onReadyRead()
{
	readRequest();

	try
	{
		requireFieldIs("type", "request");

		if (request["method"] == "put")
		{
			requireField("key");

			_store->insert(request["key"], request["value"]);
			respondOk("inserted");
		}
		else if (request["method"] == "get")
		{
			requireField("key");

			const QString val = _store->value(request["key"]);
			respondValue(request["key"], val, val.isEmpty() ? QString("empty") : QString());
		}
		else if (request["method"] == "delete")
		{
			requireField("key");

			_store->remove(request["key"]);
			respondOk("deleted");
		}
		else if (request["method"] == "count")
		{
			respondCount(_store->count());
		}
		else
		{
			throw std::exception("invalid request method");
		}
	}
	catch (const std::exception& ex)
	{
		respondError(ex.what());
	}
}

void WorkerThread::readRequest()
{
	request.clear();
	_strm->startTransaction();
	*_strm.data() >> request;
	_strm->commitTransaction();
}

void WorkerThread::requireField(const QString& name)
{
	if (!request.contains(name))
		throw std::exception(QString("missing %1").arg(name).toStdString().c_str());
}

void WorkerThread::requireFieldIs(const QString& name, const QString& val)
{
	requireField(name);
	if (request[name] != val)
		throw std::exception(QString("invalid %1").arg(name).toStdString().c_str());
}

void WorkerThread::prepareResponse()
{
	response.clear();
	response["type"] = "response";
}

void WorkerThread::sendResponse()
{
	do
	{
		_strm->startTransaction();
		*_strm.data() << response;
	}
	while (!_strm->commitTransaction());
}

void WorkerThread::respondError(const QString& str)
{
	prepareResponse();
	response["error"] = str;
	sendResponse();
}

void WorkerThread::respondOk(const QString& strDetails)
{
	prepareResponse();
	response["error"] = "ok";
	if (!strDetails.isEmpty())
		response["details"] = strDetails;
	sendResponse();
}

void WorkerThread::respondValue(const QString& key, const QString& val, const QString& strDetails)
{
	prepareResponse();
	response["key"] = key;
	response["value"] = val;
	response["error"] = "ok";
	if (!strDetails.isEmpty())
		response["details"] = strDetails;
	sendResponse();
}

void WorkerThread::respondCount(int count)
{
	prepareResponse();
	response["value"] = QString::number(count);
	response["error"] = "ok";
	sendResponse();
}
