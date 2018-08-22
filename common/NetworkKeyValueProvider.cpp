#include "NetworkKeyValueProvider.h"

NetworkKeyValueProvider::NetworkKeyValueProvider(const QString& host, quint16 port)
	: tcpSocket(new QTcpSocket)
{
	tcpSocket->connectToHost(host, port);
	tcpSocket->waitForConnected();
	if (tcpSocket->state() != QAbstractSocket::ConnectedState)
		throw std::exception("cannot connect to the server");

	strm.setDevice(tcpSocket);
	strm.setVersion(QDataStream::Qt_5_10);
}

NetworkKeyValueProvider::~NetworkKeyValueProvider()
{
	delete tcpSocket;
}

QString NetworkKeyValueProvider::value(const QString& key) const
{
	strm.startTransaction();
	QMap<QString, QString> data;
	data["type"] = "request";
	data["method"] = "get";
	data["key"] = key;
	strm << data;
	strm.commitTransaction();

	readResponse();

	return response["value"];
}

void NetworkKeyValueProvider::insert(const QString& key, const QString& val)
{
	strm.startTransaction();
	QMap<QString, QString> data;
	data["type"] = "request";
	data["method"] = "put";
	data["key"] = key;
	data["value"] = val;
	strm << data;
	strm.commitTransaction();

	readResponse();
}

void NetworkKeyValueProvider::readResponse() const
{
	response.clear();

	do
	{
		if (!tcpSocket->waitForReadyRead())
			throw std::exception("waitForReadyRead failed");
		strm.startTransaction();
		strm >> response;
	}
	while (!strm.commitTransaction());

	if (response["type"] != "response")
		throw std::exception("invalid response type from the server");
	if (lastResponseError() != "ok")
		throw std::exception(lastResponseError().toStdString().c_str());
}

void NetworkKeyValueProvider::remove(const QString& key)
{
	strm.startTransaction();
	QMap<QString, QString> data;
	data["type"] = "request";
	data["method"] = "delete";
	data["key"] = key;
	strm << data;
	strm.commitTransaction();

	readResponse();
}

int NetworkKeyValueProvider::count()
{
	strm.startTransaction();
	QMap<QString, QString> data;
	data["type"] = "request";
	data["method"] = "count";
	strm << data;
	strm.commitTransaction();

	readResponse();

	return response["value"].toInt();
}

QString NetworkKeyValueProvider::lastResponseDetails() const
{
	return response["details"];
}

QString NetworkKeyValueProvider::lastResponseError() const
{
	return response["error"];
}
