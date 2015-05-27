/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_STANZAFACTORY_P_H
#define JREEN_STANZAFACTORY_P_H

#include "stanza_p.h"
#include "langmap.h"
#include <QXmlStreamAttributes>
#include <QStack>

namespace Jreen
{

class JREEN_AUTOTEST_EXPORT StanzaFactory : public XmlStreamFactory<Stanza>
{
public:
	StanzaFactory(Client *client);
	virtual ~StanzaFactory();
	
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &name);
	
	virtual int stanzaType() = 0;
	virtual Stanza::Ptr createStanza() = 0;
	void serialize(Stanza *stanza, QXmlStreamWriter *writer);
protected:
	void writeAttributes(Stanza *stanza, QXmlStreamWriter *writer);
	void writePayloads(Stanza *stanza, QXmlStreamWriter *writer);
	void writeEscapedString(const QString &str, QXmlStreamWriter *writer);
	void writeLangMap(const QString &tag, const LangMap &map,QXmlStreamWriter *writer); //may be move to XmlStreamFactory?
	int m_depth;
	QScopedPointer<StanzaPrivate> m_stanza;
	Client *m_client;
	QStack<XmlStreamParser*> m_parsers;
};
}

#endif // JREEN_STANZAFACTORY_P_H
