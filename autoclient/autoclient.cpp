#include <QtTest>
#include <QRandomGenerator>
#include <QTime>
#include <QtConcurrent>

#include "autoclient.h"
#include "../common/LocalKeyValueProvider.h"
#include "../common/NetworkKeyValueProvider.h"
#include "../common/PersistentKeyValueStorage.h"

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

void Client::test_Insert()
{
	try
	{
		helper_Insert(_store.data(), 10000);
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

void Client::test_Persistense()
{
	try
	{
		initKeyProvider();

		KeysAndValues keysAndValues = randomKeysAndValues(1000);
		helper_InsertAll(_store.data(), keysAndValues);
		helper_VerifyAll(_store.data(), keysAndValues);

		initKeyProvider();

		helper_VerifyAll(_store.data(), keysAndValues);
		helper_DeleteAll(_store.data(), keysAndValues);
		helper_VerifyAllEmpty(_store.data(), keysAndValues);

		initKeyProvider();
		helper_VerifyAllEmpty(_store.data(), keysAndValues);
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
	_store.reset();
	if (_local)
		initLocalProvider();
	else
		initNetworkProvider();
}

void Client::initLocalProvider()
{
	qInfo() << "initializing local provider";
	_store.reset(new LocalKeyValueProvider);
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

	qInfo() << "initializing network provider";
	_store.reset(new NetworkKeyValueProvider(hostAddr, port));

	qInfo() << "connected to a server at " << hostAddr << ":" << port;
}

void Client::helper_InsertDelete(IKeyValueProvider* provider, const int nInserts)
{
	try
	{
		qInfo() << "performing put -> check -> delete -> check" << nInserts << "times";

		for (int i = 0; i < nInserts; ++i)
		{
			const QString key = randomKey();
			const QString val = randomValue();
			provider->insert(key, val);
			QVERIFY(provider->value(key) == val);

			provider->remove(key);
			QVERIFY(provider->value(key).isEmpty());
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
		nThreads = 1;
		qInfo() << "testing in" << nThreads << "thread(s)";

		QList<QFuture<void>> futures;

		for (int tidx = 0; tidx < nThreads; ++tidx)
			futures << QtConcurrent::run([&]() { helper_InsertReadDelete(_store.data(), _maxIterations, _maxIterations * 10000); });

		for (auto& it : futures)
			it.waitForFinished();
	}
	else
	{
		nThreads = 1;
		qInfo() << "testing in" << nThreads << "thread(s)";

		helper_InsertReadDelete(_store.data(), _maxIterations, _maxIterations * 10);
	}
}

void Client::helper_InsertReadDelete(IKeyValueProvider* provider, const int nInserts, const int nReads)
{
	try
	{
		qInfo() << "performing insert" << nInserts  << "times, read" << nReads << "times, then delete everything which was added";

		KeysAndValues keysAndValues = randomKeysAndValues(nInserts);

		//const auto initialKeyCount = _store->count();

		QTime timer;
		timer.start();

		helper_InsertAll(provider, keysAndValues);

		if (timer.elapsed() > 0)
			qInfo() << "INSERT performance:" << keysAndValues.count() * 1000 / timer.elapsed() << "insertions/s";

		timer.restart();
		for (int i = 0; i < nReads; ++i)
		{
			int idx = nReads % keysAndValues.count();
			QVERIFY(provider->value(keysAndValues[idx].first) == keysAndValues[idx].second);
		}
		
		if (timer.elapsed() > 0)
			qInfo() << "READ performance:" << nReads * 1000 / timer.elapsed()  << "reads/s";

		timer.restart();
		helper_DeleteAll(provider, keysAndValues);

		if (timer.elapsed() > 0)
			qInfo() << "REMOVE performance:" << keysAndValues.count() * 1000 / timer.elapsed() << "deletions/s";

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

void Client::helper_InsertAll(IKeyValueProvider* provider, const KeysAndValues& container)
{
	for (const QPair<QString, QString>& kv : container)
		provider->insert(kv.first, kv.second);
}

void Client::helper_VerifyAll(IKeyValueProvider* provider, const KeysAndValues& container)
{
	for (const QPair<QString, QString>& kv : container)
		QVERIFY(provider->value(kv.first) == kv.second);
}

void Client::helper_DeleteAll(IKeyValueProvider* provider, const KeysAndValues& container)
{
	for (const QPair<QString, QString>& kv : container)
		provider->remove(kv.first);
}

void Client::helper_VerifyAllEmpty(IKeyValueProvider* provider, const KeysAndValues& container)
{
	for (const QPair<QString, QString>& kv : container)
		QVERIFY(provider->value(kv.first).isEmpty());
}

void Client::helper_Insert(IKeyValueProvider* provider, const int nInserts)
{
	try
	{
		qInfo() << "performing put -> check" << nInserts << "times";

		for (int i = 0; i < nInserts; ++i)
		{
			const QString key = randomKey();
			const QString val = randomValue();
			provider->insert(key, val);
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

KeysAndValues Client::randomKeysAndValues(const int count)
{
	KeysAndValues res;
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
