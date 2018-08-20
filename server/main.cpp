#include <QCoreApplication>

#include "server.h"
#include "utils.h"

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
		utils::logCritical(QString("Unable to start the server: %1.").arg(server.errorString()));
		return -1;
	}

	utils::logInfo(QString("Server is running on %1:%2").arg(server.serverAddress().toString()).arg(server.serverPort()));

	return app.exec();
}
