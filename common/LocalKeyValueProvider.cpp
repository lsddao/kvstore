#include "LocalKeyValueProvider.h"
#include "PersistentKeyValueStorage.h"

#include <QReadLocker>
#include <QWriteLocker>

#include <QDebug>

LocalKeyValueProvider::LocalKeyValueProvider(unsigned int maxMemoryMB, bool persistValues)
	: _maxMemory(maxMemoryMB * 1024 * 1024) 
{
	if (persistValues)
		_persistentStorage.reset(new PersistentKeyValueStorage);
}

LocalKeyValueProvider::~LocalKeyValueProvider()
{

}

QString LocalKeyValueProvider::value(const QString& key) const
{
	QReadLocker l(&_lock);

	auto it = _map.constFind(key);
	if (it != _map.constEnd())
		return *it;

	if (!_persistentStorage)
		return{};

	// we shall wait in an case
	QString res = _persistentStorage->value(key);
	_lock.tryLockForWrite();
	insertToCache(key, res);

	return res;
}

void LocalKeyValueProvider::insert(const QString& key, const QString& val)
{	
	QWriteLocker l(&_lock);
	insertToCache(key, val);

	// TODO -> separate worker thread with queue
	// i.e. make insert non-blocking
	if (_persistentStorage)
		_persistentStorage->insert(key, val);

	// TODO -> separate worker thread
	reduceCacheIfNeeded();
}

void LocalKeyValueProvider::remove(const QString& key)
{
	QWriteLocker l(&_lock);
	removeFromCache(key);
	_lastUsedKeys.removeOne(key);

	// won't block, because removal is only logical
	if (_persistentStorage)
		_persistentStorage->remove(key);
}

int LocalKeyValueProvider::count()
{
	QReadLocker l(&_lock);
	return _persistentStorage ? _persistentStorage->count() : _map.size();
}

qint64 LocalKeyValueProvider::cacheMemoryOccupied() const
{
	return _usedMemory;
}

unsigned int LocalKeyValueProvider::usedMemory(const QString& key, const QString& val) const
{
	// TODO how to count QHash's internal memory usage?
	return 2 * (key.capacity() + val.capacity());
}

void LocalKeyValueProvider::reduceCacheIfNeeded()
{
	const bool reducingNeeded = _usedMemory > _maxMemory;
	if (reducingNeeded)
		qInfo() << "High memory usage; reducing cache...";

	while (_usedMemory > _maxMemory)
	{
		removeFromCache(_lastUsedKeys.dequeue());
	}

	if (reducingNeeded)
		qInfo() << "Cache reducing done";
}

void LocalKeyValueProvider::insertToCache(const QString& key, const QString& val) const
{
	if (!val.isEmpty())
	{
		removeFromCache(key);
		_map[key] = val;
		_lastUsedKeys.removeOne(key);
		_lastUsedKeys.enqueue(key);
		_usedMemory += usedMemory(key, val);
	}
}

void LocalKeyValueProvider::removeFromCache(const QString& key) const
{
	auto it = _map.constFind(key);
	if (it == _map.constEnd())
		return;
	_usedMemory -= usedMemory(key, *it);
	_map.erase(it);
}
