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

	if (request["type"] != "request")
	{
		respondError("invalid request type");
		return;
	}

	if (request["method"] == "put")
	{
		//qInfo() << "+ " << request["key"] << request["value"];
		_store->insert(request["key"], request["value"]);
		respondOk("inserted");
	}
	else if (request["method"] == "get")
	{
		//qInfo() << "? " << request["key"];
		QString val = _store->value(request["key"]);
		
	}
	else if (request["method"] == "delete")
	{
		//qInfo() << "- " << request["key"];
	}
	else
	{
		respondError("invalid request method");
		return;
	}

	respondError("ok");
}

void WorkerThread::readRequest()
{
	request.clear();
	_strm.startTransaction();
	_strm >> request;
	_strm.commitTransaction();
}

void WorkerThread::respondError(const QString& str)
{
	QMap<QString, QString> response;
	response["type"] = "response";
	response["error"] = str;
	_strm.startTransaction();
	_strm << response;
	_strm.commitTransaction();
}

void WorkerThread::respondOk(const QString& strDetails)
{
	QMap<QString, QString> response;
	response["type"] = "response";
	response["error"] = "ok";
	if (!strDetails.isEmpty())
		response["details"] = strDetails;
	_strm.startTransaction();
	_strm << response;
	_strm.commitTransaction();
}

void WorkerThread::respondValue(const QString& key, const QString& val, const QString& strDetails)
{
	QMap<QString, QString> response;
	response["type"] = "response";
	response["value"] = val;
	response["value"] = val;
	response["error"] = "ok";
	if (!strDetails.isEmpty())
		response["details"] = strDetails;
	_strm.startTransaction();
	_strm << response;
	_strm.commitTransaction();
}

