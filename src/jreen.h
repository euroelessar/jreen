/****************************************************************************
 *  jreen.h
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

#ifndef JREEN_H
#define JREEN_H

# ifdef J_BUILD_LIBRARY
#  define QT_NO_CAST_FROM_ASCII
# endif

# ifndef __cplusplus
#  error "C++ is needed"
# endif // __cplusplus

# include <QDomElement>
# include <QtGlobal>

# ifndef J_BUILD_STATIC
#  ifdef J_BUILD_LIBRARY
#   define JREEN_EXPORT Q_DECL_EXPORT
#  else
#   define JREEN_EXPORT Q_DECL_IMPORT
#  endif
# endif
# ifndef JREEN_EXPORT
#  define JREEN_EXPORT
# endif


# define J_PURE_SINGLETON(Class) \
public: \
	static Class &instance() \
	{ \
		static Class *pointer = 0; \
		if(!pointer) \
			pointer = new Class; \
		return *pointer; \
	} \
private:

# define J_SINGLETON(Class) \
	J_PURE_SINGLETON(Class) \
	Class() {} \
	virtual ~Class() {}

# define J_STRING(text) static const QString text##_str(QLatin1String(#text));

#define J_PURE_EXTENSION(Extension,XPath) \
	public: \
		inline static const StanzaExtensionMeta &meta() \
		{ \
			static StanzaExtensionMeta staticExtensionMeta(#Extension , XPath); \
			return staticExtensionMeta; \
		} \
		virtual QString xPath() const { return meta().xpath; } \
		virtual int extensionType() const { return meta().type; } \
		virtual QByteArray extensionName() const { return meta().name; } \
	private:

#define J_FEATURE(XPath) \
	public: \
		const QString &xPath() const \
		{ \
			static const QString xpath(XPath); \
			return xpath; \
		}

# ifndef J_FORELEMENTS

class ForElementContainer
{
public:
	inline ForElementContainer(const QDomNode &parent) : list(parent.childNodes()), current(list.item(0).toElement()), i(0), test(true) {}
	QDomNodeList list;
	QDomElement current;
	uint i;
	bool test;
	inline void check() { while((current=list.item(++i).toElement()).isNull() && i<list.length()) {} test=true; }
};

# define J_FORELEMENTS(_node_, _parent_) \
	for(ForElementContainer _container_(_parent_); _container_.i<_container_.list.length(); _container_.check()) \
		for(_node_=_container_.current; _container_.test; _container_.test=false)

# endif // J_FORELEMENTS

# ifndef forelements
#  define forelements J_FORELEMENTS
# endif

class QXmlStreamAttributes;
			
namespace jreen
{
	class JREEN_EXPORT XmlStreamParser
	{
	public:
		virtual ~XmlStreamParser() {}
		virtual bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes) = 0;
		virtual void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes) = 0;
		virtual void handleEndElement(const QStringRef &name, const QStringRef &uri) = 0;
		virtual void handleCharacterData(const QStringRef &text) = 0;
	};
}

#endif // JREEN_H
