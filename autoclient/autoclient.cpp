#include <QtTest>
#include <QRandomGenerator>
#include <QTime>
#include <QtConcurrent>

#include "autoclient.h"
#include "../common/LocalKeyValueProvider.h"
#include "../common/NetworkKeyValueProvider.h"

void Client::initTestCase()
{
	_keyPrefix = randomString(3);

	if (QCoreApplication::arguments().size() > 1)
		_local = false;

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
	if (_local)
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
	{
		QStringList hostPort = args[1].split(":");
		if (hostPort.size() > 0)
			hostAddr = hostPort[0];
		if (hostPort.size() > 1)
		{
			bool ok;
			port = hostPort[1].toUShort(&ok);
			if (!ok)
				port = 50000;
		}
	}
		
	if (args.size() > 2)
	{
		bool ok;
		_maxIterations = args[2].toInt(&ok);
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
	int nThreads = 1;
	if (_local)
	{
		nThreads = 4;
		qInfo() << "testing in" << nThreads << "thread(s)";

		QList<QFuture<void>> futures;

		for (int tidx = 0; tidx < nThreads; ++tidx)
			futures << QtConcurrent::run([&]() { test_InsertReadDelete(_maxIterations, _maxIterations * 10000); });

		for (auto& it : futures)
			it.waitForFinished();
	}
	else
	{
		nThreads = 1;
		qInfo() << "testing in" << nThreads << "thread(s)";

		test_InsertReadDelete(_maxIterations, _maxIterations * 10);
	}
}

void Client::test_InsertReadDelete(const int nInserts, const int nReads)
{
	try
	{
		qInfo() << "performing insert" << nInserts  << "times, read" << nReads << "times, then delete everything which was added";

		const auto keysAndValues = randomKeysAndValues(nInserts);

		//const auto initialKeyCount = _store->count();

		QTime timer;
		timer.start();

		for (int i = 0; i < keysAndValues.count(); ++i)
			_store->insert(keysAndValues[i].first, keysAndValues[i].second);

		if (timer.elapsed() > 0)
			qInfo() << "INSERT performance:" << keysAndValues.count() / timer.elapsed() << "insertions per ms";

		timer.restart();
		for (int i = 0; i < nReads; ++i)
		{
			int idx = nReads % keysAndValues.count();
			QVERIFY(_store->value(keysAndValues[idx].first) == keysAndValues[idx].second);
		}
		
		if (timer.elapsed() > 0)
			qInfo() << "READ performance:" << nReads / timer.elapsed() << "reads per ms";

		timer.restart();
		for (int i = 0; i < keysAndValues.count(); ++i)
			_store->remove(keysAndValues[i].first);

		if (timer.elapsed() > 0)
			qInfo() << "REMOVE performance:" << keysAndValues.count() / timer.elapsed() << "deletions per ms";

		//QVERIFY(_store->count() == initialKeyCount);
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
	return QString("%1_%2").arg(_keyPrefix, randomString(12));
}

QString Client::randomValue()
{
	return randomString(QRandomGenerator::global()->bounded(1, 1024));
}

QList<QPair<QString, QString>> Client::randomKeysAndValues(const int count)
{
	QList<QPair<QString, QString>> res;
	for (int i = 0; i < count; ++i)
		res.append(qMakePair(randomKey(), randomValue()));
	return res;
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
