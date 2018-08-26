#pragma once

#include "KeyValueProvider.h"

#include <QFile>
#include <QHash>
#include <QDataStream>

struct AllocationData
{
	qint64 pos{ -1 };
	qint64 allocated{ -1 };
};

class PersistentKeyValueStorage : public IKeyValueProvider
{
public:
	explicit PersistentKeyValueStorage(const QString& storageFileName = "storage.db");
	~PersistentKeyValueStorage() override;

	QString value(const QString& key) const override;
	void insert(const QString& key, const QString& val) override;
	void remove(const QString& key) override;
	int count() override;

protected:
	QString indexFilePath() const;

	void loadIndex();
	void storeIndex();

	AllocationData appendString(const QString& str);
	qint64 write(const qint64 pos, const QString& str);
	QString read(const qint64 pos) const;

private:
	mutable QFile _file;
	QHash<QString, AllocationData> _index;
	qint64 _wasted{0};
};

#ifndef QT_NO_DATASTREAM
inline QDataStream& operator >> (QDataStream& s, AllocationData& p)
{
	s >> p.pos >> p.allocated;
	return s;
}

inline QDataStream& operator<<(QDataStream& s, const AllocationData& p)
{
	s << p.pos << p.allocated;
	return s;
}
#endif