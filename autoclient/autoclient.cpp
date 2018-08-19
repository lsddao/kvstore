#include <QtTest>
#include <QRandomGenerator>

#include "autoclient.h"
#include "../common/LocalKeyValueProvider.h"
#include "../common/NetworkKeyValueProvider.h"

void Client::initTestCase()
{
	_keyPrefix = randomString(3);
	initKeyProvider();
}

void Client::initKeyProvider()
{
	//_store.reset(new LocalKeyValueProvider);
	_store.reset(new NetworkKeyValueProvider("127.0.0.1", 50000));
}

void Client::test_Insert()
{
	try
	{
		for (int i = 0; i < 1000; ++i)
		{
			const QString key = randomKey();
			QVERIFY(_store->value(key).isEmpty());

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

void Client::cleanupTestCase()
{
}

QString Client::randomString(const int length)
{
	QString res;
	for (int i = 0; i < length; ++i)
		res.append(static_cast<char>(QRandomGenerator::global()->bounded(32, 127)));
	return res;
}

QString Client::randomKey()
{
	return _keyPrefix + "_" + randomString(4);
}

QString Client::randomValue()
{
	return randomString(QRandomGenerator::global()->bounded(1, 1024));
}

QTEST_MAIN(Client)
