#include "LocalKeyValueProvider.h"

LocalKeyValueProvider::LocalKeyValueProvider(unsigned int maxMemoryMB)
	: _maxMemory(maxMemoryMB * 1024 * 1024)
{
}

LocalKeyValueProvider::~LocalKeyValueProvider()
{

}

void LocalKeyValueProvider::setUnderlyingProvider(std::unique_ptr<IKeyValueProvider> &&underlyingProvider)
{
	_underlyingProvider = std::move(underlyingProvider);
}

QString LocalKeyValueProvider::value(const QString& key) const
{
	auto it = _map.constFind(key);
	if (it != _map.constEnd())
		return *it;

	if (!_underlyingProvider)
		return {};

	const auto value = _underlyingProvider->value(key);
	if (!value.isEmpty())
		_map[key] = value;

	return value;
}

void LocalKeyValueProvider::insert(const QString& key, const QString& val)
{
	_map[key] = val;
	if (_underlyingProvider)
		_underlyingProvider->insert(key, val);
}

void LocalKeyValueProvider::remove(const QString& key)
{
	_map.remove(key);
	if (_underlyingProvider)
		_underlyingProvider->remove(key);
}

int LocalKeyValueProvider::count()
{
	return _underlyingProvider ? _underlyingProvider->count() : _map.size();
}
