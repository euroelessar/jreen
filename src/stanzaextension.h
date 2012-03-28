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

#ifndef STANZAEXTENSION_H
#define STANZAEXTENSION_H

#include <QObject>
#include <QSharedPointer>
#include <QMetaTypeId>
#include "jreen.h"

class QXmlStreamWriter;
class QXmlStreamAttributes;
template <typename T, typename X> class QMultiMap;

namespace Jreen
{

class Client;

// TODO: Rename to Payload or XmlEntity, it'd be more appropriate for current ideas

class JREEN_EXPORT Payload
{
	Q_DISABLE_COPY(Payload)
public:
	typedef QSharedPointer<Payload> Ptr;
	
	Payload();
	virtual ~Payload();
	
	static int registerPayloadType(const char *type);
	static const char *payloadName(int type);
	
	virtual int payloadType() const = 0;
	const char *payloadName() const;
};

typedef QMultiMap<int, Payload::Ptr> PayloadList;

class JREEN_EXPORT AbstractPayloadFactory : public XmlStreamFactory<Payload>
{
	Q_DISABLE_COPY(AbstractPayloadFactory)
public:
	AbstractPayloadFactory();
	virtual ~AbstractPayloadFactory();
	
	virtual QStringList features() const = 0;
	virtual int payloadType() const = 0;
	virtual Payload::Ptr createPayload() = 0;
};

typedef QMap<int, AbstractPayloadFactory*> PayloadFactoryMap;

template <typename Extension>
class JREEN_AUTOTEST_EXPORT PayloadFactory : public AbstractPayloadFactory
{
	Q_DISABLE_COPY(PayloadFactory)
public:
	PayloadFactory();
	virtual ~PayloadFactory();
	
	virtual int payloadType() const;
};

//template <typename T>
//Q_INLINE_TEMPLATE T se_cast(Payload *se)
//{
//	if (se && qMetaTypeId<T>() == se->payloadType())
//		return static_cast<T>(se);
//	return 0;
//}

template <typename Extension>
Q_INLINE_TEMPLATE PayloadFactory<Extension>::PayloadFactory()
{
}

template <typename Extension>
Q_INLINE_TEMPLATE PayloadFactory<Extension>::~PayloadFactory()
{
}

template <typename Extension>
Q_INLINE_TEMPLATE int PayloadFactory<Extension>::payloadType() const
{
	return Extension::staticPayloadType();
}

template <typename T>
Q_INLINE_TEMPLATE T payload_cast(Payload *se)
{
	if (se && reinterpret_cast<T>(0)->staticPayloadType() == se->payloadType())
		return static_cast<T>(se);
	return 0;
}

template <typename T>
Q_INLINE_TEMPLATE T se_cast(Payload *se)
{
	return payload_cast<T>(se);
}
}

#define J_PAYLOAD(Class) \
	public:  \
		typedef QSharedPointer<Class> Ptr; \
		static int staticPayloadType() \
		{ \
			static QBasicAtomicInt payloadType = Q_BASIC_ATOMIC_INITIALIZER(0); \
			if (!payloadType) { \
				payloadType = Payload::registerPayloadType( #Class ); \
				Class *useFullNameWithNamespaces = reinterpret_cast< ::Class* >(0); \
				Q_UNUSED(useFullNameWithNamespaces); \
			} \
			return payloadType; \
		} \
		virtual int payloadType() const \
		{ \
			Q_UNUSED(static_cast<const ::Class*>(this)); \
			return staticPayloadType(); \
		} \
	private:

#endif // STANZAEXTENSION_H
