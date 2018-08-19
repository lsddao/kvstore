#include "workerthread.h"

#include <QtNetwork>

WorkerThread::WorkerThread(int socketDescriptor, IKeyValueProvider* store)
	: _socketDescriptor(socketDescriptor)
	, _store(store)
{
	if (!_socket.setSocketDescriptor(_socketDescriptor))
	{
		emit error(_socket.error());
		return;
	}

	_strm.setDevice(&_socket);
	_strm.setVersion(QDataStream::Qt_5_10);

	connect(&_socket, &QTcpSocket::disconnected, this, &WorkerThread::quit);
	connect(&_socket, &QTcpSocket::readyRead, this, &WorkerThread::onReadyRead);
}

void WorkerThread::run()
{
	exec();
}

void WorkerThread::onReadyRead()
{
	readRequest();

	try
	{
		requireFieldIs("type", "request");
		requireField("key");

		if (request["method"] == "put")
		{
			//qInfo() << "+" << request["key"];

			_store->insert(request["key"], request["value"]);
			respondOk("inserted");
		}
		else if (request["method"] == "get")
		{
			//qInfo() << "?" << request["key"];

			QString val = _store->value(request["key"]);
			respondValue(request["key"], val, val.isEmpty() ? QString("empty") : QString());
		}
		else if (request["method"] == "delete")
		{
			//qInfo() << "-" << request["key"];

			_store->remove(request["key"]);
			respondOk("deleted");
		}
		else
		{
			throw std::exception("invalid request method");
		}

		printStats();
	}
	catch (const std::exception& ex)
	{
		respondError(ex.what());
	}
}

void WorkerThread::readRequest()
{
	request.clear();
	_strm.startTransaction();
	_strm >> request;
	_strm.commitTransaction();
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
	_strm.startTransaction();
	_strm << response;
	_strm.commitTransaction();
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
	response["value"] = val;
	response["error"] = "ok";
	if (!strDetails.isEmpty())
		response["details"] = strDetails;
	sendResponse();
}

void WorkerThread::printStats()
{
	//qInfo() << "total count:" << _store->count();
}

