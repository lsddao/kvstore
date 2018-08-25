#include "LocalKeyValueProvider.h"

#include <QReadLocker>
#include <QWriteLocker>

LocalKeyValueProvider::LocalKeyValueProvider(unsigned int maxMemoryMB, IKeyValueProvider* persistentStorage)
	: _maxMemory(maxMemoryMB * 1024 * 1024)
	, _persistentStorage(persistentStorage)
{
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

	const auto value = _persistentStorage->value(key);
	if (!value.isEmpty())
	{
		_lock.tryLockForWrite();
		_map[key] = value;
	}

	return value;
}

void LocalKeyValueProvider::insert(const QString& key, const QString& val)
{
	QWriteLocker l(&_lock);

	_map[key] = val;
	if (_persistentStorage)
		_persistentStorage->insert(key, val);
}

void LocalKeyValueProvider::remove(const QString& key)
{
	QWriteLocker l(&_lock);

	_map.remove(key);
	if (_persistentStorage)
		_persistentStorage->remove(key);
}

int LocalKeyValueProvider::count()
{
	QReadLocker l(&_lock);

	return _persistentStorage ? _persistentStorage->count() : _map.size();
}
