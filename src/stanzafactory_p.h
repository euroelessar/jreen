/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
	void writePayloads(Stanza *stanza, QXmlStreamWriter *writer);
	void writeLangMap(const QString &tag, const LangMap &map,QXmlStreamWriter *writer); //may be move to XmlStreamFactory?
	JID m_from;
	JID m_to;
	QString m_id;
	Client *m_client;
};
}

#endif // STANZAFACTORY_H
