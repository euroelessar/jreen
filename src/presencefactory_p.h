/****************************************************************************
 *  presencefactory_p.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef PRESENCEFACTORY_P_H
#define PRESENCEFACTORY_P_H

#include "stanzafactory.h"
#include "presence.h"
#include "langmap.h"

namespace Jreen
{
class PresenceFactory : public StanzaFactory
{
public:
	enum State { AtNowhere, AtShow, AtStatus,AtPriority };
    PresenceFactory(Client *client);
	int stanzaType();
	Stanza::Ptr createStanza();
	void serialize(Stanza *stanza, QXmlStreamWriter *writer);
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &name);
private:
	void clear();
	int m_depth;
	Presence::Type m_subtype;
	int m_priority;
	LangMap m_status;
	State m_state;
	QStringRef m_xmllang;
};
}

#endif // PRESENCEFACTORY_P_H
