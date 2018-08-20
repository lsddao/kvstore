#include "KeyValuePair.h"

#include <QTextStream>

using KeyValuePairQString = KeyValuePair<QString, QString>;

namespace detail {
	inline bool readString(QIODevice& device, QString& str)
	{
		str.clear();

		QTextStream stream(&device);
		QChar qch('\0');

		do {
			stream >> qch;
			if (stream.status() != QTextStream::Ok)
				return false;

			str.append(qch);
		} while (qch != QChar('\0'));

		return true;
	}

	inline bool writeString(QIODevice& device, const QString& str)
	{
		QTextStream stream(&device);

		for (int i = 0; i < str.length(); ++i)
		{
			stream << str.data()[i];
			if (stream.status() != QTextStream::Ok)
				return false;
		}

		stream << QChar('\0');
		return stream.status() == QTextStream::Ok;
	}
}

template<>
inline bool KeyValuePairQString::write(QIODevice& device)
{
	return detail::writeString(device, key) && detail::writeString(device, value);
}

template<>
inline bool KeyValuePairQString::read(QIODevice& device)
{
	if (!detail::readString(device, key) || !detail::readString(device, key))
	{
		key.clear();
		value.clear();

		return false;
	}

	return true;
}

template<>
size_t KeyValuePairQString::spaceRequired() const
{
	return key.length() * 2 /* QChar represents a 2-byte Unicode code point */ + 1 /* null-terminator */ + value.length() * 2 + 1;
}
