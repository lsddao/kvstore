#include <QCoreApplication> 
#include <QtCore>

#include <stdlib.h>

#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

	Server server;
	if (!server.listen())
	{
		qCritical() << QString("Unable to start the server: %1.").arg(server.errorString());
		return -1;
	}

	qInfo() << "The server is running on " << server.serverAddress().toString() << ":" << server.serverPort();

    return app.exec();
}
