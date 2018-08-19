#pragma once

#include "KeyValueProvider.h"

#include <QHash>

class LocalKeyValueProvider : public IKeyValueProvider
{
public:
	LocalKeyValueProvider(unsigned int maxMemoryMB = 10);
	~LocalKeyValueProvider();

	QString value(const QString& key) const override;
	void insert(const QString& key, const QString& val) override;
	void remove(const QString& key) override;
	int count() override;

private:
	const unsigned int _maxMemory;
	QHash<QString, QString> _map;
};
