/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "multimediadatafactory_p.h"
#define NS_MULTIMEDIA_DATA QLatin1String("urn:xmpp:media-element")
#include <QLatin1String>
#include <QXmlStreamWriter>
#include <QStringList>

namespace Jreen {

MultimediaDataFactory::MultimediaDataFactory()
{
}

MultimediaDataFactory::~MultimediaDataFactory()
{

}

QStringList MultimediaDataFactory::features() const
{
	return QStringList(NS_MULTIMEDIA_DATA);
}
bool MultimediaDataFactory::canParse(const QStringRef &name, const QStringRef &uri,
									 const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("media") && uri == NS_MULTIMEDIA_DATA;
}

void MultimediaDataFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
											   const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(attributes);
	Q_UNUSED(uri);
	m_depth++;
	if(m_depth == 1) {
		foreach(const QXmlStreamAttribute &attribute,attributes) {
			m_attributes.insert(attribute.name().toString(),
								attribute.value().toString());
		}
	} else if(m_depth == 2) {
		m_currentDataItem.clear();
		m_currentDataItem.insert("type",attributes.value("type").toString());
	}
}

void MultimediaDataFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	if(m_depth == 2)
		m_data.append(m_currentDataItem);
	m_depth--;
}

void MultimediaDataFactory::handleCharacterData(const QStringRef &text)
{
	if(m_depth == 2)
		m_currentDataItem.insert("uri",text.toString());
}

void MultimediaDataFactory::serialize(const MultimediaData &media, QXmlStreamWriter *writer)
{
	writer->writeStartElement(QLatin1String("media"));
	writer->writeDefaultNamespace(NS_MULTIMEDIA_DATA);

	QVariantMap attributes = media.attributes();
	QVariantMap::const_iterator it = attributes.constBegin();
	while(it != attributes.constEnd()) {
		writer->writeAttribute(it.key(),it.value().toString());
		it++;
	}
	foreach(QVariant item,media.data()) {
		QVariantMap map = item.toMap();
		writer->writeStartElement(QLatin1String("uri"));
		writer->writeAttribute(QLatin1String("type"),map.value("type").toString());
		writer->writeCharacters(map.value("uri").toString());
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

MultimediaData MultimediaDataFactory::create()
{
	return MultimediaData(m_type,m_data,m_attributes);
}

} // namespace Jreen
