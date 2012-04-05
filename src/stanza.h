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

#ifndef STANZA_H
#define STANZA_H

#include "stanzaextension.h"
#include "error.h"
#include "jid.h"
#include <QMap>

class QXmlStreamWriter;

namespace Jreen
{

class StanzaPrivate;

class JREEN_EXPORT Stanza
{
	Q_DECLARE_PRIVATE(Stanza)
public:
	typedef QSharedPointer<Stanza> Ptr;
	Stanza(const Stanza &stanza);
	virtual ~Stanza();
	void setFrom(const JID &jid);
	const JID &from() const;
	const JID &to() const;
	const QString &id() const;
	void addExtension(Payload::Ptr se);
	inline void addPayload(Payload::Ptr se)
	{ addExtension(se); }
	inline void addExtension(Payload* se)
	{ addExtension(Payload::Ptr(se)); }
	inline void addPayload(Payload* se)
	{ addExtension(Payload::Ptr(se)); }
	PayloadList payloads() const;
	template< class T >
	QList<typename T::Ptr> payloads() const
	{
		QList<typename T::Ptr> list;
		foreach (const Payload::Ptr &payload, payloads().values(T::staticPayloadType()))
			list << payload.staticCast<T>();
		return list;
	}
	template< class T >
	inline const QSharedPointer<T> payload() const
	{ return qSharedPointerCast<T>(payloads().value(T::staticPayloadType())); }
	template< class T >
	inline bool containsPayload() const
	{ return payloads().contains(T::staticPayloadType()); }
	void removePayloads();
	template< class T >
	inline void removePayload() { removePayload(T::staticPayloadType()); }
	void removePayload(int id);
	Error::Ptr error() const;
protected:
	Stanza(StanzaPrivate &);
	Stanza &operator =(const Stanza &stanza);
	StanzaPrivate *d_ptr;
};

}

#endif // STANZA_H
