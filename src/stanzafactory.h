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

#ifndef STANZAFACTORY_H
#define STANZAFACTORY_H

#include "stanza.h"
#include "langmap.h"
#include <QXmlStreamAttributes>

namespace Jreen
{

class StanzaFactory : public XmlStreamFactory<Stanza>
{
public:
	StanzaFactory(Client *client);
	virtual ~StanzaFactory();
	virtual int stanzaType() = 0;
	virtual Stanza::Ptr createStanza() = 0;
protected:
	void parseAttributes(const QXmlStreamAttributes &attributes);
	void writeAttributes(Stanza *stanza, QXmlStreamWriter *writer);
	void writeStanzaExtensions(Stanza *stanza, QXmlStreamWriter *writer);
	void writeLangMap(const QString &tag, const LangMap &map,QXmlStreamWriter *writer); //may be move to XmlStreamFactory?
	JID m_from;
	JID m_to;
	QString m_id;
	Client *m_client;
};
}

#endif // STANZAFACTORY_H
