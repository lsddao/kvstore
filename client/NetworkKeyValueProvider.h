#pragma once

#include <QDataStream>
#include <QTcpSocket>

#include "../common/KeyValueProvider.h"

class NetworkKeyValueProvider : public IKeyValueProvider
{
public:
	NetworkKeyValueProvider(const QString& host, quint16 port);
	~NetworkKeyValueProvider();

	QString value(const QString& key) const override;
	void insert(const QString& key, const QString& val) override;
	void remove(const QString& key) override;
	
	QString lastResponseDetails() const;
	QString lastResponseError() const;

protected:
	void readResponse() const;

private:
	QTcpSocket *tcpSocket = nullptr;
	mutable QDataStream strm;
	mutable QMap<QString, QString> response;
};