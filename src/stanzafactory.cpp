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

#include "stanzafactory.h"

namespace jreen
{
	StanzaFactory::StanzaFactory(Client *client)
	{
		Q_UNUSED(client);
	}

	StanzaFactory::~StanzaFactory()
	{
	}
	
	void StanzaFactory::parseAttributes(const QXmlStreamAttributes &attributes)
	{
		m_from = attributes.value(QLatin1String("from")).toString();
		m_to = attributes.value(QLatin1String("to")).toString();
		m_id = attributes.value(QLatin1String("id")).toString();
	}
	
	void StanzaFactory::writeAttributes(Stanza *stanza, QXmlStreamWriter *writer)
	{
		if(stanza->from().isValid())
			writer->writeAttribute(QLatin1String("from"), stanza->from());
		if(stanza->to().isValid())
			writer->writeAttribute(QLatin1String("to"), stanza->to());
		if(!stanza->id().isEmpty())
			writer->writeAttribute(QLatin1String("id"), stanza->id());
		writer->writeAttribute(QLatin1String("xmlns"), QLatin1String("jabber:client"));
	}
	
	void StanzaFactory::writeStanzaExtensions(Stanza *stanza, QXmlStreamWriter *writer)
	{
		Q_UNUSED(stanza);
		Q_UNUSED(writer);
	}
}
