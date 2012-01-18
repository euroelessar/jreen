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

#include "langmap.h"
#include "jstrings.h"
#include <QXmlStreamWriter>

namespace Jreen
{

LangMapData LangMapData::shared_null = { Q_BASIC_ATOMIC_INITIALIZER(1), QString(), LangMapData::Base() };

void LangMapData::fillNode(QXmlStreamWriter *writer, const QString &name, const QString &uri) const
{
	if (!writer)
		return;
	if (!base.isEmpty())
		writer->writeTextElement(name, base);
	for (LangMapData::Base::const_iterator i = other.begin(); i != other.end(); i++) {
		writer->writeStartElement(name);
		writer->writeAttribute(QLatin1String("xml:lang"), i.key());
		if (!uri.isEmpty())
			writer->writeDefaultNamespace(uri);
		writer->writeEndElement();
	}
}

const QString &LangMapData::value(const QString &lang) const
{
	if(!lang.isEmpty())
	{
		LangMapData::Base::const_iterator iter = other.find(lang);
		if(iter != other.end())
			return iter.value();
	}
	return base;
}

const QString &LangMapData::value(const QString &lang, const QString &default_value) const
{
	if(!lang.isEmpty())
	{
		LangMapData::Base::const_iterator iter = other.find(lang);
		if(iter != other.end())
			return iter.value();
	}
	return default_value;
}

}
