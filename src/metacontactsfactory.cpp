/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "metacontactsfactory_p.h"
#include <QStringList>
#define NS_METACONTACTS QLatin1String("storage:metacontacts")

namespace Jreen {

MetaContactsFactory::MetaContactsFactory()
{
	m_depth = 0;
}

MetaContactsFactory::~MetaContactsFactory()
{
}

QStringList MetaContactsFactory::features() const
{
	return QStringList(NS_METACONTACTS);
}

bool MetaContactsFactory::canParse(const QStringRef &name, const QStringRef &uri,
							   const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("storage") && uri == NS_METACONTACTS;
}

void MetaContactsFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
										 const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if(m_depth == 1) {
		m_metacontacts.reset(new MetaContacts);
	} else if (m_depth == 2 && name == QLatin1String("meta")) {
		Jreen::MetaContactStorage::Item item;
		item.setJID(attributes.value(QLatin1String("jid")).toString());
		item.setTag(attributes.value(QLatin1String("tag")).toString());
		QString orderStr = attributes.value(QLatin1String("order")).toString();
		bool ok = true;
		uint order = orderStr.toUInt(&ok);
		if (ok)
			item.setOrder(order);
		m_metacontacts->items << item;
	}
}

void MetaContactsFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void MetaContactsFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void MetaContactsFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	MetaContacts *metacontacts = se_cast<MetaContacts*>(extension);
	writer->writeStartElement(QLatin1String("storage"));
	writer->writeDefaultNamespace(NS_METACONTACTS);
	foreach (const MetaContactStorage::Item &item, metacontacts->items) {
		writer->writeStartElement(QLatin1String("meta"));
		writer->writeAttribute(QLatin1String("jid"), item.jid().full());
		writer->writeAttribute(QLatin1String("tag"), item.tag());
		if (item.hasOrder())
			writer->writeAttribute(QLatin1String("order"), QString::number(item.order()));
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

Payload::Ptr MetaContactsFactory::createPayload()
{
	return Payload::Ptr(m_metacontacts.take());
}

} // namespace Jreen
