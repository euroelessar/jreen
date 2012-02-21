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

#include "privacyqueryfactory_p.h"
#include "jstrings.h"
#include <QStringList>
#include <QXmlStreamWriter>

#define NS_PRIVACY QLatin1String("jabber:iq:privacy")

namespace Jreen
{

PrivacyQueryFactory::PrivacyQueryFactory()
{
	m_depth = 0;
	m_state = AtNowhere;
}

PrivacyQueryFactory::~PrivacyQueryFactory()
{
}

QStringList PrivacyQueryFactory::features() const
{
	return QStringList(NS_PRIVACY);
}

bool PrivacyQueryFactory::canParse(const QStringRef &name, const QStringRef &uri,
								   const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_PRIVACY;
}

static const char *privacy_types[] = { "jid", "group", "subscription" };
static const char *privacy_actions[] = { "deny", "allow" };
static const char *subscription_types[] = { "from", "to", "both", "remove", "none" };
static const char *privacy_stanzas[] = { "message", "presence-in", "presence-out", "iq" };

void PrivacyQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
											 const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		m_query.reset(new PrivacyQuery);
	} if (m_depth == 2) {
		QStringRef listName = attributes.value(QLatin1String("name"));
		if (name == QLatin1String("list")) {
			m_state = AtList;
			m_name = listName.toString();
			
		} else if (name == QLatin1String("active")) {
			m_query->activeList = listName.toString();
		} else if (name == QLatin1String("default")) {
			m_query->defaultList = listName.toString();
		}
	} else if (m_depth == 3 && m_state == AtList && name == QLatin1String("item")) {
		// We should provide fallback as said in rfc-3920
		m_items.append(m_items.isEmpty() ? PrivacyItem() : m_items.last());
		PrivacyItem &item = m_items.last();
		m_state = AtItem;
		QStringRef data = attributes.value(QLatin1String("value"));
		int type = strToEnum(attributes.value(QLatin1String("type")), privacy_types);
		if (type == -1 && !data.isEmpty())
			type = item.type();
		switch (type) {
		case PrivacyItem::ByJID:
			item.setJID(data.toString());
			break;
		case PrivacyItem::ByGroup:
			item.setGroup(data.toString());
			break;
		case PrivacyItem::BySubscription:
			item.setSubscription(strToEnum<PrivacyItem::SubscriptionType>(data, subscription_types));
			break;
		default:
			item = PrivacyItem();
			break;
		}
		data = attributes.value(QLatin1String("action"));
		item.setAction(strToEnum<PrivacyItem::Action>(data, privacy_actions));
		item.setOrder(attributes.value(QLatin1String("order")).toString().toInt());
		item.setStanzaTypes(0);
	} else if (m_depth == 4 && m_state == AtItem) {
		int data = strToEnum(name, privacy_stanzas);
		if (data == -1)
			return;
		PrivacyItem &item = m_items.last();
		item.setStanzaTypes(item.stanzaTypes() | static_cast<PrivacyItem::StanzaType>(1 << data));
	}
}

void PrivacyQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	if (m_depth == 2 && m_state == AtList) {
		m_state = AtNowhere;
		m_query->lists << PrivacyQuery::List(m_name, m_items);
		m_name.clear();
		m_items.clear();
	} else if (m_depth == 3 && m_state == AtItem) {
		m_state = AtList;
	}
	m_depth--;
}

void PrivacyQueryFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void PrivacyQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	PrivacyQuery *query = se_cast<PrivacyQuery*>(extension);
	Q_ASSERT(query);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_PRIVACY);
	if (!query->activeList.isNull()) {
		writer->writeEmptyElement(QLatin1String("active"));
		if (!query->activeList.isEmpty())
			writer->writeAttribute(QLatin1String("name"), query->activeList);
	}
	if (!query->defaultList.isNull()) {
		writer->writeEmptyElement(QLatin1String("default"));
		if (!query->defaultList.isEmpty())
			writer->writeAttribute(QLatin1String("name"), query->defaultList);
	}
	for (int i = 0; i < query->lists.size(); i++) {
		const PrivacyQuery::List &list = query->lists.at(i);
		writer->writeStartElement(QLatin1String("list"));
		writer->writeAttribute(QLatin1String("name"), list.name);
		for (int j = 0; j < list.items.size(); j++) {
			const PrivacyItem &item = list.items.at(j);
			QString value;
			switch (item.type()) {
			case PrivacyItem::ByJID:
				value = item.jid().full();
				break;
			case PrivacyItem::ByGroup:
				value = item.group();
				break;
			case PrivacyItem::BySubscription:
				value = enumToStr(item.subscription(), subscription_types);
				break;
			default:
				break;
			}
			writer->writeStartElement(QLatin1String("item"));
			if (item.type() != PrivacyItem::All) {
				writer->writeAttribute(QLatin1String("type"), enumToStr(item.type(), privacy_types));
				writer->writeAttribute(QLatin1String("value"), value);
			}
			writer->writeAttribute(QLatin1String("action"), enumToStr(item.action(), privacy_actions));
			writer->writeAttribute(QLatin1String("order"), QString::number(item.order()));
			int types = item.stanzaTypes();
			for (int k = 0; k < 4; k++) {
				if (types & (1 << k))
					writer->writeEmptyElement(QLatin1String(privacy_stanzas[k]));
			}
			writer->writeEndElement();
		}
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

Payload::Ptr PrivacyQueryFactory::createPayload()
{
	return Payload::Ptr(m_query.take());
}

} // namespace Jreen
