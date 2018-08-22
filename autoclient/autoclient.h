#pragma once

#include <QObject>

#include "../common/KeyValueProvider.h"

class Client : public QObject
{
    Q_OBJECT
	
private slots:
	void initTestCase();

	void test_Insert();
	void test_InsertDelete();
	void benchmark_InsertDelete();
	void benchmark_InsertDelete_MT();
	
	void cleanupTestCase();

private:
	void test_InsertReadDelete(const int nInserts, const int nReads);

	void initKeyProvider();
	void initLocalProvider();
	void initNetworkProvider();

	QString randomString(const int length);
	QString randomKey();
	QString randomValue();
	QList<QPair<QString, QString>> randomKeysAndValues(const int count);

private:
	QScopedPointer<IKeyValueProvider> _store;
	QString _keyPrefix;
	int _maxIterations{1000};
};
