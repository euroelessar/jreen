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

#ifndef UTIL_H
#define UTIL_H

#include "jreen.h"
#include <QDateTime>
#include <QXmlStreamWriter>

namespace Jreen
{

class JID;

namespace Util
{
JREEN_EXPORT QDateTime fromStamp(const QString &stamp);
JREEN_EXPORT QString toStamp(const QDate &date);
JREEN_EXPORT QString toStamp(const QDateTime &date_time);
JREEN_EXPORT QByteArray randomHash();
JREEN_EXPORT QString randomStringHash(int len);

inline int log2(register uint n)
{
	register int pos = 0;
	while(n > 1) { ++pos; n >>= 1; }
	return (n == 0) ? (-1) : pos;
}

inline void writeAttribute(QXmlStreamWriter *writer,
						   const QLatin1String &name,const QString &value)
{
	if(!value.isEmpty())
		writer->writeAttribute(name,value);
}
inline void writeTextElement(QXmlStreamWriter *writer,
							 const QLatin1String &name,const QString &value)
{
	if(!value.isEmpty())
		writer->writeTextElement(name,value);
}

}

}

#endif // UTIL_H
