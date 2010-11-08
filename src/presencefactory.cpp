/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "presencefactory_p.h"


#include "presencefactory_p.h"
#include "presence_p.h"

namespace jreen
{
	PresenceFactory::PresenceFactory(Client *client) : StanzaFactory(client)
	{
		m_depth = 0;
		m_type = Presence::Available;
		m_priority = 0;
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
		p->subtype = m_type;
		p->status = m_status;
		p->priority = m_priority;
		return Stanza::Ptr(new Presence(*p));
	}

	void PresenceFactory::serialize(Stanza *stanza, QXmlStreamWriter *writer)
	{
		Presence *presence = static_cast<Presence*>(stanza);
		if (presence->subtype() == Presence::Invalid)
			return;
		writer->writeStartElement(QLatin1String("presence"));
		writeAttributes(stanza, writer);
//		QString type;
//		switch (presence->subtype()) {
//		case Presence::Get:
//			type = QLatin1String("get");
//			break;
//		case Presence::Set:
//			type = QLatin1String("set");
//			break;
//		case Presence::Result:
//			type = QLatin1String("result");
//			break;
//		case Presence::Error:
//			type = QLatin1String("error");
//			break;
//		default:
//			break;
//		}
//		writer->writeAttribute(QLatin1String("type"), type);
		writeStanzaExtensions(stanza, writer);
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
		m_depth++;
		if (m_depth == 1) {
			parseAttributes(attributes);
//			QStringRef type = attributes.value(QLatin1String("type"));
//			if (type == QLatin1String("get"))
//				m_type = Presence::Get;
//			else if (type == QLatin1String("set"))
//				m_type = Presence::Set;
//			else if (type == QLatin1String("result"))
//				m_type = Presence::Result;
//			else if (type == QLatin1String("error"))
//				m_type = Presence::Error;
//			else
//				m_type = Presence::Invalid;
		}
	}

	void PresenceFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		m_depth--;
		Q_UNUSED(name);
		Q_UNUSED(uri);
	}

	void PresenceFactory::handleCharacterData(const QStringRef &text)
	{
		Q_UNUSED(text);
	}
}

