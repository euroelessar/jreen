/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#include "util.h"
#include "jid.h"
#include <QCryptographicHash>

namespace Jreen
{

//enum TokenType
//{
//	TokenNumber,
//	TokenChar,
//	TokenNull
//};

//TokenType nextToken(const QChar * &s, int &val)
//{
//	if (!*s)
//		return TokenNull;
//	if (!s->isDigit()) {
//		val = (s++)->unicode();
//		return TokenChar;
//	}
//	val = 0;
//	while (*s && s->isDigit())
//		val = val * 10 + (s++)->unicode() - '0';
//	return TokenNumber;
//}
	
//QDateTime Util::fromStamp(const QChar *s)
//{
//	const QChar *s = stamp.constData();
//	int val;
//	if (TokenNumber != nextToken(s, val))
//		return QDateTime();
//	if (val >= 10000) { // deprecated stamp
//		int year = val / 10000;
//		int month = (val / 100) % 100;
//		int day = val % 100;
//		if (TokenChar != nextToken(s, val) || val != 'T')
//			return QDateTime();
//		QDate date(year, month, day);
//		return QDateTime(date, fromStamp(s).time());
//	}
//	while (s) {
		
//	}
//}
	
#define FULL_STAMP_STR QLatin1String("yyyy-MM-ddThh:mm:ss.zzz")
#define FULLZ_STAMP_STR QLatin1String("yyyy-MM-ddThh:mm:ss.zzzZ")
#define DATE_TIME_STAMP_STR QLatin1String("yyyy-MM-ddThh:mm:ss")
#define DEPRECATED_STAMP_STR QLatin1String("yyyyMMddThh:mm:ss")
#define TIME_STAMP_STR QLatin1String("hh:mm")
#define DATE_STAMP_STR QLatin1String("yyyy-MM-dd")

QDateTime Util::fromStamp(const QString &stamp)
{
	QDateTime date_time;
	if (stamp.contains(QLatin1Char('-'))) {
		if (stamp.size() == 10)
			return QDateTime::fromString(stamp, DATE_STAMP_STR);
		int num = stamp.indexOf(QLatin1Char('Z'));
		if(num < 0)
			num = stamp.lastIndexOf(QLatin1Char('-'));
		QString time = stamp;
		time.truncate(num);
		if(num == 19)
			date_time = QDateTime::fromString(time, DATE_TIME_STAMP_STR);
		else
			date_time = QDateTime::fromString(time, FULL_STAMP_STR);
		if(num > 19) {
			QTime delta = QTime::fromString(stamp.right(5), TIME_STAMP_STR);
			int multi = 1;
			if(stamp.at(stamp.length() - 6) == QLatin1Char('+'))
				multi = -1;
			date_time = date_time.addSecs(multi * (delta.minute() * 60 + delta.hour() * 3600));
		}
	} else { // Deprecated format of stamp
		date_time = QDateTime::fromString(stamp, DEPRECATED_STAMP_STR);
	}
	if(!date_time.isValid())
		return QDateTime();
	date_time.setTimeSpec(Qt::UTC);
	return date_time.toLocalTime();
}

QString Util::toStamp(const QDate &date)
{
	return date.toString(DATE_STAMP_STR);
}

QString Util::toStamp(const QDateTime &date_time)
{
	return date_time.toUTC().toString(FULLZ_STAMP_STR);
}

QByteArray Util::randomHash()
{
	qint32 buf[5];
	for (int i = 0; i < 5; i++)
		buf[i] = qrand();
	return QByteArray::fromRawData(reinterpret_cast<char*>(buf), sizeof(buf)).toHex();
}

QString Util::randomStringHash(int length)
{
	QString str(length, Qt::Uninitialized);
	for (int i = 0; i < length; ++i) {
		int c = qrand() % (10 + 26);
		if (c < 10)
			str[i] = QLatin1Char('0' + c);
		else
			str[i] = QLatin1Char('a' + c - 10);
	}
	return str;
}

}
