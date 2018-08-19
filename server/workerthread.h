#pragma once

#include <QThread>
#include <QTcpSocket>
#include <QDataStream>

#include "../common/KeyValueProvider.h"

class WorkerThread : public QThread
{
	Q_OBJECT

public:
	WorkerThread(int socketDescriptor, IKeyValueProvider* store);

	void run() override;

signals:
	void error(QTcpSocket::SocketError socketError);

private slots:
	void onReadyRead();

protected:
	void readRequest();
	void respondError(const QString& str);
	void respondOk(const QString& strDetails = QString::null);
	void respondValue(const QString& key, const QString& val, const QString& strDetails = QString::null);

private:
	QTcpSocket _socket;
	int _socketDescriptor{ -1 };
	QDataStream _strm;
	QMap<QString, QString> request;
	IKeyValueProvider* _store;
};
