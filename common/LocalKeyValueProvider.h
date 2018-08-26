#pragma once

#include "KeyValueProvider.h"

#include <QHash>
#include <QReadWriteLock>
#include <QScopedPointer>

class LocalKeyValueProvider : public IKeyValueProvider
{
public:
	LocalKeyValueProvider(unsigned int maxMemoryMB = 10, bool persistValues = true);
	~LocalKeyValueProvider() override;

	QString value(const QString& key) const override;
	void insert(const QString& key, const QString& val) override;
	void remove(const QString& key) override;
	int count() override;

protected:
	void insertToCache(const QString& key, const QString& val);
	void removeFromCache(const QString& key);

	unsigned int usedMemory(const QString& key, const QString& val) const;

private:
	const unsigned int _maxMemory;
	unsigned int _usedMemory{0};
	mutable QHash<QString, QString> _map;
	QScopedPointer<IKeyValueProvider> _persistentStorage;
	mutable QReadWriteLock _lock;
};
