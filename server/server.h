#pragma once

#include <QStringList>
#include <QTcpServer>

#include "../common/KeyValueProvider.h"

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject *parent = 0);
	~Server();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
	QScopedPointer<IKeyValueProvider> _store;
};
