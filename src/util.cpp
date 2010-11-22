/****************************************************************************
 *  util.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "util.h"
#include "jid.h"
#include <QCryptographicHash>

namespace jreen
{

#define FULL_STAMP_STR QLatin1String("yyyy-MM-ddThh:mm:ss.zzz")
#define FULLZ_STAMP_STR QLatin1String("yyyy-MM-ddThh:mm:ss.zzzZ")
#define DATE_TIME_STAMP_STR QLatin1String("yyyy-MM-ddThh:mm:ss")
#define DEPRECATED_STAMP_STR QLatin1String("yyyyMMddThh:mm:ss")
#define TIME_STAMP_STR QLatin1String("hh:mm")

QDateTime Util::fromStamp(const QString &stamp)
{
	QDateTime date_time;
	if(stamp.contains(QLatin1Char('-')))
	{
		int num = stamp.indexOf(QLatin1Char('Z'));
		if(num < 0)
			num = stamp.lastIndexOf(QLatin1Char('-'));
		QString time = stamp;
		time.truncate(num);
		if(num == 19)
			date_time = QDateTime::fromString(time, DATE_TIME_STAMP_STR);
		else
			date_time = QDateTime::fromString(time, FULL_STAMP_STR);
		if(num > 19)
		{
			QTime delta = QTime::fromString(stamp.right(5), TIME_STAMP_STR);
			int multi = 1;
			if(stamp.at(stamp.length() - 6) == QLatin1Char('+'))
				multi = -1;
			date_time = date_time.addSecs(multi * (delta.minute() * 60 + delta.hour() * 3600));
		}
	}
	else // Deprecated format of stamp
		date_time = QDateTime::fromString(stamp, DEPRECATED_STAMP_STR);
	if(!date_time.isValid())
		return QDateTime();
	date_time.setTimeSpec(Qt::UTC);
	return date_time.toLocalTime();
}

QString Util::toStamp(const QDateTime &date_time)
{
	return date_time.toUTC().toString(FULLZ_STAMP_STR);
}

QString Util::randomHash(const JID &jid)
{
	QCryptographicHash hash(QCryptographicHash::Sha1);
	qptrdiff temp = QDateTime::currentDateTime().toTime_t();
	QByteArray data;
	data.append(jid.full().toUtf8());
	data.append('\0');
	for(uint i = 0; i < sizeof(qptrdiff); i++, temp /= 0x100)
		data.append(temp % 0x100);
	data.append('\0');
	temp = reinterpret_cast<qptrdiff>(&jid);
	for(uint i = 0; i < sizeof(qptrdiff); i++, temp /= 0x100)
		data.append(temp % 0x100);
	hash.addData(data);
	data.append('\0');
	temp = qrand();
	for(uint i = 0; i < sizeof(qptrdiff); i++, temp /= 0x100)
		data.append(temp % 0x100);
	hash.addData(data);
	return QLatin1String(hash.result().toHex());
}

}
