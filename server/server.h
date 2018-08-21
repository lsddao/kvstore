#pragma once

#include <QStringList>
#include <QTcpServer>

#include "../common/KeyValueProvider.h"

class Server : public QTcpServer
{
	Q_OBJECT

public:
	Server(QObject *parent = nullptr);
	~Server() override = default;

protected:
	void incomingConnection(qintptr socketDescriptor) override;

protected:
	void printStats();

private:
	QScopedPointer<IKeyValueProvider> _store;
};
