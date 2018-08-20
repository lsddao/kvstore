#pragma once

#include "KeyValueProvider.h"

#include <QHash>

#include <memory>

class LocalKeyValueProvider : public IKeyValueProvider
{
public:
	LocalKeyValueProvider(unsigned int maxMemoryMB = 10);
	~LocalKeyValueProvider() override;

	void setUnderlyingProvider(std::unique_ptr<IKeyValueProvider>&& underlyingProvider);

	QString value(const QString& key) const override;
	void insert(const QString& key, const QString& val) override;
	void remove(const QString& key) override;
	int count() override;

private:
	const unsigned int _maxMemory;
	mutable QHash<QString, QString> _map;
	std::unique_ptr<IKeyValueProvider> _underlyingProvider;
};
