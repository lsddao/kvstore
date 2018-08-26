#pragma once

#include <QObject>

#include "../common/KeyValueProvider.h"

using KeysAndValues = QList<QPair<QString, QString>>;

class Client : public QObject
{
    Q_OBJECT
	
private slots:
	void initTestCase();

	void test_Insert();

	void cleanupTestCase();

private:
	void benchmark_InsertDelete();
	void test_Persistense();

	void helper_InsertAll(IKeyValueProvider* provider, const KeysAndValues& container);
	void helper_VerifyAll(IKeyValueProvider* provider, const KeysAndValues& container);
	void helper_DeleteAll(IKeyValueProvider* provider, const KeysAndValues& container);
	void helper_VerifyAllEmpty(IKeyValueProvider* provider, const KeysAndValues& container);

	void helper_Insert(IKeyValueProvider* provider, const int nInserts);
	void helper_InsertDelete(IKeyValueProvider* provider, const int nInserts);

	void helper_InsertReadDelete(IKeyValueProvider* provider, const int nInserts, const int nReads);

	void initKeyProvider();
	void initLocalProvider();
	void initNetworkProvider();

	QString randomString(const int length);
	QString randomKey();
	QString randomValue();
	KeysAndValues randomKeysAndValues(const int count);

private:
	QScopedPointer<IKeyValueProvider> _store;
	QString _keyPrefix;
	int _maxIterations{1000};
	bool _local{ true };
};

