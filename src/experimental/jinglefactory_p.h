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

#ifndef JINGLEFACTORY_P_H
#define JINGLEFACTORY_P_H

#include "jingle_p.h"
#include "../client_p.h"

namespace Jreen
{

class JingleFactory : public PayloadFactory<Jingle>
{
public:
    JingleFactory(Client *client);

	static bool checkSupport(const QSet<QString> &features);
	virtual QStringList features() const;
	virtual bool canParse(const QStringRef &name, const QStringRef &uri,
	                      const QXmlStreamAttributes &attributes);
	virtual void handleStartElement(const QStringRef &name, const QStringRef &uri,
	                                const QXmlStreamAttributes &attributes);
	virtual void handleEndElement(const QStringRef &name, const QStringRef &uri);
	virtual void handleCharacterData(const QStringRef &text);
	virtual void serialize(Payload *obj, QXmlStreamWriter *writer);
	virtual Payload::Ptr createPayload();
	
private:
	ClientPrivate *m_client;
	enum State { AtRoot, AtContent, AtTransport, AtDescription, AtReason } m_state;
	int m_depth;
	Jingle::Ptr m_jingle;
	AbstractPayloadFactory *m_factory;
	QScopedPointer<Jingle::Content> m_content;
};

}

#endif // JINGLEFACTORY_P_H
