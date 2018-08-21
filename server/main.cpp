#include <QCoreApplication>

#include "server.h"
#include "utils.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h> 

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		QCoreApplication::exit(0);
		return(TRUE);

	// Pass other signals to the next handler. 
	//case CTRL_BREAK_EVENT:
	//	return FALSE;

	//case CTRL_LOGOFF_EVENT:
	//	return FALSE;

	//case CTRL_SHUTDOWN_EVENT:
	//	return FALSE;

	default:
		return FALSE;
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
		utils::logCritical("Could not set control handler");

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
