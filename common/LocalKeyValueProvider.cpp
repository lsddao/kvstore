#include "LocalKeyValueProvider.h"

LocalKeyValueProvider::LocalKeyValueProvider(unsigned int maxMemoryMB)
	: _maxMemory(maxMemoryMB * 1024 * 1024)
{
}

LocalKeyValueProvider::~LocalKeyValueProvider()
{

}

QString LocalKeyValueProvider::value(const QString& key) const
{
	auto it = _map.constFind(key);
	if (it == _map.constEnd())
		return QString::null;
	else
		return *it;
}

void LocalKeyValueProvider::insert(const QString& key, const QString& val)
{
	_map[key] = val;
}

void LocalKeyValueProvider::remove(const QString& key)
{
	_map.remove(key);
}

int LocalKeyValueProvider::count()
{
	return _map.size();
}
