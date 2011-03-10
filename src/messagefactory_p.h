/****************************************************************************
 *  messagefactory_p.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef MESSAGEFACTORY_P_H
#define MESSAGEFACTORY_P_H

#include "stanzafactory.h"
#include "message.h"

namespace Jreen
{
class MessageFactory : public StanzaFactory
{
public:
	enum State { AtMessage, AtBody, AtSubject,AtThread };
	MessageFactory(Client *client);
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
	Message::Type m_subtype;
	LangMap m_body;
	LangMap m_subject;
	State m_state;
	QStringRef m_thread;
};

} // namespace  Jreen

#endif // MESSAGEFACTORY_P_H
