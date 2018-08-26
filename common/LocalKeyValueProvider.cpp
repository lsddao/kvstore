#include "LocalKeyValueProvider.h"
#include "PersistentKeyValueStorage.h"

#include <QReadLocker>
#include <QWriteLocker>

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

	return{};

	//if (!_persistentStorage)
	//	return{};

	//const auto value = _persistentStorage->value(key);
	//if (!value.isEmpty())
	//{
	//	_lock.tryLockForWrite();
	//	_map[key] = value;
	//}

	//return value;
}

void LocalKeyValueProvider::insert(const QString& key, const QString& val)
{
	QWriteLocker l(&_lock);
	insertToCache(key, val);
	//if (_persistentStorage)
	//	_persistentStorage->insert(key, val);
}

void LocalKeyValueProvider::remove(const QString& key)
{
	QWriteLocker l(&_lock);
	removeFromCache(key);
	//if (_persistentStorage)
	//	_persistentStorage->remove(key);
}

int LocalKeyValueProvider::count()
{
	QReadLocker l(&_lock);

	return /*_persistentStorage ? _persistentStorage->count() :*/ _map.size();
}

unsigned int LocalKeyValueProvider::usedMemory(const QString& key, const QString& val) const
{
	// TODO how to count QHash's internal memory usage?
	return 2 * (key.capacity() + val.capacity());
}

void LocalKeyValueProvider::insertToCache(const QString& key, const QString& val)
{
	_map[key] = val;
	_usedMemory += usedMemory(key, val);
}

void LocalKeyValueProvider::removeFromCache(const QString& key)
{
	auto it = _map.constFind(key);
	if (it == _map.constEnd())
		return;
	_usedMemory -= usedMemory(key, *it);
	_map.erase(it);
}
