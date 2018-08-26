#pragma once

#include <QString>

class IKeyValueProvider
{
public:
	virtual QString value(const QString& key) const = 0;
	virtual void insert(const QString& key, const QString& val) = 0;
	virtual void remove(const QString& key) = 0;
	virtual int count() = 0;
	virtual qint64 cacheMemoryOccupied() const { return -1; }

	virtual ~IKeyValueProvider() = default;
};
