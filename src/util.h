/****************************************************************************
 *  util.h
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
