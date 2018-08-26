#include "PersistentKeyValueStorage.h"

#include <QDebug>
#include <QFileInfo>

#include <QByteArray>

#include <assert.h>


void checkDataStreamStatus(const QDataStream& strm)
{
	if (strm.status() != QDataStream::Ok)
		throw std::exception(QString("Data stream status is %1: %2").arg(strm.status()).arg(strm.device() ? strm.device()->errorString() : "(no device)").toLatin1().constData());
}

PersistentKeyValueStorage::PersistentKeyValueStorage(const QString& storageFileName)
	: _file(storageFileName)
{
	if (!_file.open(QFile::ReadWrite))
	{
		qInfo() << "Failed to open storage file" << storageFileName << "for reading and writing, error:" << _file.errorString();
		assert(_file.isOpen());
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
	auto it = _index.constFind(key);
	if (it == _index.constEnd())
		return {};

	const AllocationData& data = *it;
	return read(data.pos);
}

void PersistentKeyValueStorage::insert(const QString& key, const QString& val)
{
	// For simplicity, we always append
	remove(key);
	_index[key] = appendString(val);
}

AllocationData PersistentKeyValueStorage::appendString(const QString& str)
{
	AllocationData tmp;
	tmp.pos = _file.size();
	tmp.allocated = write(tmp.pos, str);
	return tmp;
}

void PersistentKeyValueStorage::remove(const QString& key)
{
	auto it = _index.find(key);
	if (it != _index.end())
	{
		_wasted += it->allocated;
		_index.erase(it);
	}
}

int PersistentKeyValueStorage::count()
{
	return _index.size();
}

QString PersistentKeyValueStorage::indexFilePath() const
{
	return QFileInfo(_file.fileName()).canonicalPath() + "/index.db";
}

void PersistentKeyValueStorage::loadIndex()
{
	QFile indexFile(indexFilePath());
	if (!indexFile.open(QFile::ReadOnly))
	{
		if (_file.size() != 0)
			qInfo() << "Failed to open storage index file" << indexFile.fileName() << "for reading, error:" << indexFile.errorString();
		return;
	}

	QDataStream strm(&indexFile);
	strm.setVersion(QDataStream::Qt_5_10);
	strm >> _index;
	checkDataStreamStatus(strm);
}

void PersistentKeyValueStorage::storeIndex()
{
	QFile indexFile(indexFilePath());
	if (!indexFile.open(QFile::WriteOnly | QFile::Truncate))
	{
		qInfo() << "Failed to open storage index file" << indexFile.fileName() << "for writing, error:" << indexFile.errorString();
		return;
	}

	QDataStream strm(&indexFile);
	strm.setVersion(QDataStream::Qt_5_10);
	strm << _index;
	checkDataStreamStatus(strm);
}

inline QByteArray serialize(const QString& str)
{
	QByteArray res;
	QDataStream strm(&res, QIODevice::WriteOnly);
	strm.setVersion(QDataStream::Qt_5_10);
	strm << str;
	assert(res.size() > 0);
	checkDataStreamStatus(strm);
	return res;
}

qint64 PersistentKeyValueStorage::write(const qint64 pos, const QString& str)
{
	qint64 bytesWritten = -1;
	QByteArray data = serialize(str);
	if (_file.seek(pos))
		bytesWritten = _file.write(data);
	if (bytesWritten != data.size())
		throw std::exception(QString("Write failed at location %1: %2").arg(pos).arg(_file.errorString()).toLatin1().constData());
	return bytesWritten;
}

QString PersistentKeyValueStorage::read(const qint64 pos) const
{
	QDataStream strm(&_file);
	strm.setVersion(QDataStream::Qt_5_10);
	if (!_file.seek(pos))
		throw std::exception(QString("Seek to location %1 failed: %2").arg(pos).arg(_file.errorString()).toLatin1().constData());
	QString res;
	strm >> res;
	checkDataStreamStatus(strm);
	return res;
}

