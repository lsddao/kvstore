#include <QtTest>
#include <QRandomGenerator>
#include <QTime>

#include "autoclient.h"
#include "../common/LocalKeyValueProvider.h"
#include "../common/NetworkKeyValueProvider.h"

void Client::initTestCase()
{
	_keyPrefix = randomString(3);

	try
	{
		initKeyProvider();
		QVERIFY(!_store.isNull());
	}
	catch (const std::exception& ex)
	{
		QFAIL(ex.what());
	}
	catch (...)
	{
		QFAIL("UNKNOWN ERROR");
	}
	
}

void Client::initKeyProvider()
{
	if (QCoreApplication::arguments().size() < 2)
		initLocalProvider();
	else
		initNetworkProvider();
}

void Client::initLocalProvider()
{
	qInfo() << "initializing local provider";
	_store.reset(new LocalKeyValueProvider());
}

void Client::initNetworkProvider()
{
	QString hostAddr("127.0.0.1");
	quint16 port = 0;

	QStringList args = QCoreApplication::arguments();
	if (args.size() > 1)
		hostAddr = args[1];

	if (args.size() > 2)
	{
		bool ok;
		port = args[2].toUShort(&ok);
		if (!ok)
			port = 50000;
	}

	if (args.size() > 3)
	{
		bool ok;
		_maxIterations = args[3].toInt(&ok);
		if (!ok)
			_maxIterations = 1000;
	}

	qInfo() << "initializing network provider";
	_store.reset(new NetworkKeyValueProvider(hostAddr, port));

	qInfo() << "connected to a server at " << hostAddr << ":" << port;
}

void Client::test_InsertDelete()
{
	try
	{
		qInfo() << "performing put -> check -> delete -> check" << _maxIterations << "times";

		for (int i = 0; i < _maxIterations; ++i)
		{
			const QString key = randomKey();
			const QString val = randomValue();
			_store->insert(key, val);
			QVERIFY(_store->value(key) == val);

			_store->remove(key);
			QVERIFY(_store->value(key).isEmpty());
		}
	}
	catch (const std::exception& ex) 
	{ 
		QFAIL(ex.what()); 
	}
	catch (...) 
	{ 
		QFAIL("UNKNOWN ERROR");
	}
}

void Client::benchmark_InsertDelete()
{
	try
	{
		qInfo() << "performing insert" << _maxIterations << "times";
		QStringList keys, values;
		while (keys.count() < _maxIterations)
		{
			const auto key = randomKey();
			const auto value = randomKey();

			if (keys.contains(key) || values.contains(value))
				continue;

			keys.push_back(key);
			values.push_back(value);
		}

		QTime timer;
		timer.start();
		for (int i = 0; i < _maxIterations; ++i)
		{
			_store->insert(keys[i], values[i]);
		}
		qInfo() << "INSERT performance:" << _maxIterations * 1000 / timer.elapsed() << "insertions per second";

		timer.restart();
		for (int i = 0; i < _maxIterations; ++i)
		{
			_store->remove(keys[i]);
		}
		qInfo() << "REMOVE performance:" << _maxIterations * 1000 / timer.elapsed() << "deletions per second";
	}
	catch (const std::exception& ex)
	{
		QFAIL(ex.what());
	}
	catch (...)
	{
		QFAIL("UNKNOWN ERROR");
	}
}

void Client::test_Insert()
{
	try
	{
		qInfo() << "performing put -> check" << _maxIterations << "times";

		for (int i = 0; i < _maxIterations; ++i)
		{
			const QString key = randomKey();
			const QString val = randomValue();
			_store->insert(key, val);
			QVERIFY(_store->value(key) == val);
		}
	}
	catch (const std::exception& ex)
	{
		QFAIL(ex.what());
	}
	catch (...)
	{
		QFAIL("UNKNOWN ERROR");
	}
}

void Client::cleanupTestCase()
{
}

QString Client::randomString(const int length)
{
	QString res;
	res.reserve(length);
	for (int i = 0; i < length; ++i)
		res.append(static_cast<char>(QRandomGenerator::global()->bounded(32, 127)));
	return res;
}

QString Client::randomKey()
{
	return _keyPrefix + "_" + randomString(12);
}

QString Client::randomValue()
{
	return randomString(QRandomGenerator::global()->bounded(1, 1024));
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setAttribute(Qt::AA_Use96Dpi, true);
	Client tc;
	QTEST_SET_MAIN_SOURCE_PATH

	QStringList argvTest{ argv[0], "-nocrashhandler" };
	return QTest::qExec(&tc, argvTest);
}
