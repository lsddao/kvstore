#pragma once

#include <QDateTime>

namespace utils
{
	inline QString prependCurrentTS(const QString& str)
	{
		return QString("%1 %2").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).arg(str);
	}

	inline void logInfo(const QString& str)
	{
		qInfo(prependCurrentTS(str).toLatin1().constData());
	}

	inline void logCritical(const QString& str)
	{
		qCritical(prependCurrentTS(str).toLatin1().constData());
	}
}