#pragma once

#include "KeyValueProvider.h"

#include <QHash>
#include <QReadWriteLock>
#include <QScopedPointer>
#include <QQueue>

class LocalKeyValueProvider : public IKeyValueProvider
{
public:
	LocalKeyValueProvider(unsigned int maxMemoryMB = 10, bool persistValues = true);
	~LocalKeyValueProvider() override;

	QString value(const QString& key) const override;
	void insert(const QString& key, const QString& val) override;
	void remove(const QString& key) override;
	int count() override;
	qint64 cacheMemoryOccupied() const override;

protected:
	void insertToCache(const QString& key, const QString& val) const;
	void removeFromCache(const QString& key) const;

	unsigned int usedMemory(const QString& key, const QString& val) const;
	void reduceCacheIfNeeded();

private:
	const unsigned int _maxMemory;
	mutable qint64 _usedMemory{0};
	mutable QHash<QString, QString> _map;
	// TODO must be indexed for good performance
	mutable QQueue<QString> _lastUsedKeys;
	QScopedPointer<IKeyValueProvider> _persistentStorage;
	mutable QReadWriteLock _lock;
};
