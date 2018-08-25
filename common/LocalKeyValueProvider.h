#pragma once

#include "KeyValueProvider.h"

#include <QHash>
#include <QReadWriteLock>
#include <QScopedPointer>

class LocalKeyValueProvider : public IKeyValueProvider
{
public:
	LocalKeyValueProvider(unsigned int maxMemoryMB = 10, IKeyValueProvider* persistentStorage = nullptr);
	~LocalKeyValueProvider() override;

	QString value(const QString& key) const override;
	void insert(const QString& key, const QString& val) override;
	void remove(const QString& key) override;
	int count() override;

private:
	const unsigned int _maxMemory;
	mutable QHash<QString, QString> _map;
	QScopedPointer<IKeyValueProvider> _persistentStorage;
	mutable QReadWriteLock _lock;
};
