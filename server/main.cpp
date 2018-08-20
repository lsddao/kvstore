#include <QCoreApplication>
#include <QtCore>

#include <stdlib.h>

#include "server.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QString hostAddr("127.0.0.1");
	quint16 port = 0;

	const QStringList args = QCoreApplication::arguments();
	if (args.size() > 1)
		hostAddr = args[1];

	if (args.size() > 2)
	{
		bool ok;
		port = args[2].toUShort(&ok);
		if (!ok)
			port = 0;
	}

	Server server;
	if (!server.listen(QHostAddress(hostAddr), port))
	{
		qCritical() << QString("Unable to start the server: %1.").arg(server.errorString());
		return -1;
	}

	qInfo() << "The server is running on " << server.serverAddress().toString() << ":" << server.serverPort();

	return app.exec();
}
