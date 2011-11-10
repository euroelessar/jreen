/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "presencefactory_p.h"


#include "presencefactory_p.h"
#include "presence_p.h"

namespace Jreen
{

PresenceFactory::PresenceFactory(Client *client) : StanzaFactory(client)
{
	m_depth = 0;
	m_state = AtNowhere;
	clear();
}

void PresenceFactory::clear()
{
	m_status.clear();
	m_priority = 0;
	m_subtype = Presence::Available;
}

int PresenceFactory::stanzaType()
{
	return StanzaPrivate::StanzaPresence;
}

Stanza::Ptr PresenceFactory::createStanza()
{
	PresencePrivate *p = new PresencePrivate;
	p->from = m_from;
	p->to = m_to;
	p->id = m_id;
	p->subtype = m_subtype;
	p->status = m_status;
	p->priority = m_priority;
	return Stanza::Ptr(new Presence(*p));
}

void PresenceFactory::serialize(Stanza *stanza, QXmlStreamWriter *writer)
{
	Presence *presence = static_cast<Presence*>(stanza);
	if(presence->subtype() == Presence::Invalid)
		return;
	writer->writeStartElement(QLatin1String("presence"));
	writeAttributes(stanza, writer);

	QString type;
	QString show;
	switch (presence->subtype()) {
	case Presence::Away:
		show = QLatin1String("away");
		break;
	case Presence::Chat:
		show = QLatin1String("chat");
		break;
	case Presence::DND:
		show = QLatin1String("dnd");
		break;
	case Presence::XA:
		show = QLatin1String("xa");
		break;
	case Presence::Available:
		break;
	case Presence::Unavailable:
		type = QLatin1String("unavailable");
		break;
	case Presence::Subscribe:
		type = QLatin1String("subscribe");
		break;
	case Presence::Subscribed:
		type = QLatin1String("subscribed");
		break;
	case Presence::Unsubscribe:
		type = QLatin1String("unsubscribe");
		break;
	case Presence::Unsubscribed:
		type = QLatin1String("unsubscribed");
		break;
	case Presence::Error:
		type = QLatin1String("error");
		break;
	case Presence::Probe:
		type = QLatin1String("probe");
		break;
	default:
		break;
	}

	if(!type.isEmpty())
		writer->writeAttribute(QLatin1String("type"), type);
	writer->writeTextElement(QLatin1String("priority"), QString::number(presence->priority()));
	writePayloads(stanza, writer);
	writeLangMap(QLatin1String("status"),presence->status(), writer);
	if(!show.isEmpty())
		writer->writeTextElement(QLatin1String("show"), show);
	writer->writeEndElement();
}

bool PresenceFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("presence");
}

void PresenceFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		clear();
		parseAttributes(attributes);
		QStringRef type = attributes.value(QLatin1String("type"));
		if (type == QLatin1String("unavailable"))
			m_subtype = Presence::Unavailable;
		else if (type == QLatin1String("probe"))
			m_subtype = Presence::Probe;
		else if (type == QLatin1String("subscribe"))
			m_subtype = Presence::Subscribe;
		else if (type == QLatin1String("unsubscribe"))
			m_subtype = Presence::Unsubscribe;
		else if (type == QLatin1String("subscribed"))
			m_subtype = Presence::Subscribe;
		else if (type == QLatin1String("unsubscribed"))
			m_subtype = Presence::Unsubscribe;
		else if (type == QLatin1String("error"))
			m_subtype = Presence::Error;
		else
			m_subtype = Presence::Available;
	} else if(m_depth == 2) {
		if(name == QLatin1String("show"))
			m_state = AtShow;
		else if(name == QLatin1String("priority")) {
			m_state = AtPriority;
		}
		else if(name == QLatin1String("status")) {
			m_state = AtStatus;
			m_xmllang = attributes.value(QLatin1String("xml:lang"));
		}
	}
}

void PresenceFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_depth == 2)
		m_state = AtNowhere;
	m_depth--;
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void PresenceFactory::handleCharacterData(const QStringRef &text)
{
	if(m_depth == 2) {
		if(m_state == AtShow) {
/*			if(text == QLatin1String("available"))
				m_type = Presence::Available;
			else if(text == QLatin1String("unavailable"))
				m_type = Presence::Unavailable;
			else */if(text == QLatin1String("away"))
				m_subtype = Presence::Away;
			else if(text == QLatin1String("chat"))
				m_subtype = Presence::Chat;
			else if(text == QLatin1String("dnd"))
				m_subtype = Presence::DND;
			else if(text == QLatin1String("xa"))
				m_subtype = Presence::XA;
		}
		else if(m_state == AtPriority) {
			m_priority = text.toString().toInt();
		}
		else if(m_state == AtStatus) {
			m_status[m_xmllang.toString()] = text.toString();
		}
	}
}

}

