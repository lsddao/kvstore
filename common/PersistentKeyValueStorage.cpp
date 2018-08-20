#include "PersistentKeyValueStorage.h"
#include "KeyValuePairQString.h"

#include <QDebug>
#include <QFileInfo>

#include <assert.h>

PersistentKeyValueStorage::PersistentKeyValueStorage(const QString& storageFileName /*= "storage.db"*/) :
	_persistentStorageFile(storageFileName)
{
	if (!_persistentStorageFile.open(QFile::ReadWrite))
	{
		qInfo() << "Failed to open storage file" << storageFileName << "for reading and writing, error:" << _persistentStorageFile.errorString();
		assert(_persistentStorageFile.isOpen());
		return;
	}

	loadIndex();
}

PersistentKeyValueStorage::~PersistentKeyValueStorage()
{
	storeIndex();
}

QString PersistentKeyValueStorage::value(const QString& key) const
{
	return {};
}

void PersistentKeyValueStorage::insert(const QString& key, const QString& val)
{

}

void PersistentKeyValueStorage::remove(const QString& key)
{

}

int PersistentKeyValueStorage::count()
{
	return 0;
}

QString PersistentKeyValueStorage::indexFilePath() const
{
	return QFileInfo(_persistentStorageFile.fileName()).canonicalPath() + "index.db";
}

bool PersistentKeyValueStorage::loadIndex()
{
	QFile indexFile(indexFilePath());
	if (!indexFile.open(QFile::ReadOnly) && _persistentStorageFile.size() != 0)
	{
		qInfo() << "Failed to open storage index file" << indexFile.fileName() << "for reading, error:" << indexFile.errorString();
		assert(indexFile.isOpen());
		return false;
	}
	else if (_persistentStorageFile.size() == 0)
		return false; // No index to read because the storage file is empty.

	// Reading the storage index
	while(!indexFile.atEnd())
	{
		decltype(_index)::value_type indexEntry;
		if (indexFile.read((char*)&indexEntry.first, sizeof(indexEntry.first)) != sizeof(indexEntry.first))
		{
			qInfo() << "Error reading index hash from" << indexFile.fileName() << ":" << indexFile.errorString();
			assert(false);
			return false;
		}

		if (indexFile.read((char*)&indexEntry.second, sizeof(indexEntry.second)) != sizeof(indexEntry.second))
		{
			qInfo() << "Error reading index offset from" << indexFile.fileName() << ":" << indexFile.errorString();
			assert(false);
			return false;
		}

		const auto insertionResult = _index.insert(indexEntry);
		if (!insertionResult.second)
		{
			qInfo() << "Duplicate entry in the index. Key hash:" << indexEntry.first << ", offset in the storage:" << indexEntry.second;
			assert(insertionResult.second);
		}
	}

	return true;
}

bool PersistentKeyValueStorage::storeIndex()
{
	if (_index.empty())
	{
		assert(_persistentStorageFile.size() == 0);
		return true;
	}

	QFile indexFile(indexFilePath());
	if (!indexFile.open(QFile::WriteOnly))
	{
		qInfo() << "Failed to open storage index file" << indexFile.fileName() << "for writing, error:" << indexFile.errorString();
		assert(indexFile.isOpen());
		return false;
	}

	for (const auto& indexEntry: _index)
	{
		if (indexFile.write((char*)&indexEntry.first, sizeof(indexEntry.first)) != sizeof(indexEntry.first))
		{
			qInfo() << "Error writing index hash to" << indexFile.fileName() << ":" << indexFile.errorString();
			assert(false);
			return false;
		}

		if (indexFile.read((char*)&indexEntry.second, sizeof(indexEntry.second)) != sizeof(indexEntry.second))
		{
			qInfo() << "Error writing index offset to" << indexFile.fileName() << ":" << indexFile.errorString();
			assert(false);
			return false;
		}
	}

	return true;
}
