#pragma once

#include "KeyValueProvider.h"

#include <QFile>

#include <map>

class PersistentKeyValueStorage : public IKeyValueProvider
{
public:
	explicit PersistentKeyValueStorage(const QString& storageFileName = "storage.db");
	~PersistentKeyValueStorage() override;

	QString value(const QString& key) const override;
	void insert(const QString& key, const QString& val) override;
	void remove(const QString& key) override;
	int count() override;

private:
	QString indexFilePath() const;

	bool loadIndex();
	bool storeIndex();

private:
	QFile _persistentStorageFile;
	std::map<uint, qint64> _index;
};
