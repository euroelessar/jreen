/****************************************************************************
 *  stanzaextension.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

class JREEN_EXPORT StanzaExtension
{
	Q_DISABLE_COPY(StanzaExtension)
public:
	typedef QSharedPointer<StanzaExtension> Ptr;
			
	StanzaExtension();
	virtual ~StanzaExtension();
	
	static int registerExtensionType(const char *type);
	
	virtual int extensionType() const = 0;
};

class JREEN_EXPORT AbstractStanzaExtensionFactory : public XmlStreamFactory<StanzaExtension>
{
	Q_DISABLE_COPY(AbstractStanzaExtensionFactory)
public:
	AbstractStanzaExtensionFactory();
	virtual ~AbstractStanzaExtensionFactory();
	
	virtual QStringList features() const = 0;
	virtual int extensionType() const = 0;
	virtual StanzaExtension::Ptr createExtension() = 0;
};

typedef QMap<int, AbstractStanzaExtensionFactory*> StanzaExtensionFactoryMap;

template <typename Extension>
class JREEN_EXPORT StanzaExtensionFactory : public AbstractStanzaExtensionFactory
{
	Q_DISABLE_COPY(StanzaExtensionFactory)
public:
	StanzaExtensionFactory() {}
	virtual ~StanzaExtensionFactory() {}
	
	virtual int extensionType() const { return Extension::staticExtensionType(); }
};

template <typename Extension>
class JREEN_EXPORT SimpleStanzaExtensionFactory : public StanzaExtensionFactory<Extension>
{
	Q_DISABLE_COPY(SimpleStanzaExtensionFactory)
public:
	SimpleStanzaExtensionFactory(const QString &name, const QString &uri, Client *client)
		: SimpleStanzaExtensionFactory(client), m_elementName(name), m_elementUri(uri) {}

	virtual ~SimpleStanzaExtensionFactory() {}
	
	virtual bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(attributes);
		return name == m_elementName && uri == m_elementUri;
	}
private:
	QString m_elementName;
	QString m_elementUri;
};

//template <typename T>
//Q_INLINE_TEMPLATE T se_cast(StanzaExtension *se)
//{
//	if (se && qMetaTypeId<T>() == se->extensionType())
//		return static_cast<T>(se);
//	return 0;
//}

template <typename T>
Q_INLINE_TEMPLATE T se_cast(StanzaExtension *se)
{
	if (se && reinterpret_cast<T>(0)->staticExtensionType() == se->extensionType())
		return static_cast<T>(se);
	return 0;
}

typedef QSharedPointer<StanzaExtension>   StanzaExtensionPointer;
typedef QMultiMap<int, StanzaExtension::Ptr> StanzaExtensionList;

}

#define J_EXTENSION(Class, XPath) \
	public:  \
		typedef QSharedPointer<Class> Ptr; \
		static int staticExtensionType() \
		{ \
			static QBasicAtomicInt extension_type = Q_BASIC_ATOMIC_INITIALIZER(0); \
			if (!extension_type) { \
				extension_type = StanzaExtension::registerExtensionType( #Class ); \
				Class *useFullNameWithNamespaces = reinterpret_cast< ::Class* >(0); \
				Q_UNUSED(useFullNameWithNamespaces); \
			} \
			return extension_type; \
		} \
		virtual int extensionType() const \
		{ \
			Q_UNUSED(static_cast<const ::Class*>(this)); \
			return staticExtensionType(); \
		} \
	private:

#define J_DECLARE_EXTENSION(Class)

#endif // STANZAEXTENSION_H
