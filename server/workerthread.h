#pragma once

#include <QThread>
#include <QTcpSocket>
#include <QDataStream>

#include "../common/KeyValueProvider.h"

class WorkerThread : public QThread
{
	Q_OBJECT

public:
	WorkerThread(qintptr socketDescriptor, IKeyValueProvider* store);

	void run() override;

signals:
	void error(QTcpSocket::SocketError socketError);

private slots:
	void onReadyRead();

protected:
	void prepareResponse();
	void sendResponse();
	void readRequest();
	void requireField(const QString& name);
	void requireFieldIs(const QString& name, const QString& val);
	void respondError(const QString& str);
	void respondOk(const QString& strDetails = QString::null);
	void respondValue(const QString& key, const QString& val, const QString& strDetails = QString());
	void respondCount(int count);

private:
	QScopedPointer<QTcpSocket> _socket;
	qintptr _socketDescriptor{ -1 };
	QScopedPointer<QDataStream> _strm;
	QMap<QString, QString> request;
	QMap<QString, QString> response;
	IKeyValueProvider* _store;
};
