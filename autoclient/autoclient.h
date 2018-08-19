#pragma once

#include <QObject>

#include "../common/KeyValueProvider.h"

class Client : public QObject
{
    Q_OBJECT
	
private slots:

	void initTestCase();
	
	void test_Insert();
	
	void cleanupTestCase();

private:
	void initKeyProvider();

	QString randomString(const int length);
	QString randomKey();
	QString randomValue();

private:
	QScopedPointer<IKeyValueProvider> _store;
	QString _keyPrefix;
};
