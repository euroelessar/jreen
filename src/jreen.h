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

# ifndef J_NAMESPACE
#  define J_NAMESPACE jreen
# endif

# define J_USE_NAMESPACE using namespace ::J_NAMESPACE;
# define J_BEGIN_NAMESPACE namespace J_NAMESPACE {

# ifdef J_BUILD_LIBRARY
#  define J_END_NAMESPACE } \
	J_USE_NAMESPACE
# else
# define J_END_NAMESPACE }
# endif

# define J_DECLARE_PRIVATE(Class) \
	inline Class##Private* j_func() { return reinterpret_cast<Class##Private *>(j_ptr); } \
	inline const Class##Private* j_func() const { return reinterpret_cast<Class##Private *>(j_ptr); } \
	friend class Class##Private;

# define J_D(Class) Class##Private * const j = j_func()

# define J_PURE_SINGLETON(Class) \
public: \
	static Class &instance() \
	{ \
		static Class *pointer = 0; \
		if( !pointer ) \
			pointer = new Class; \
		return *pointer; \
	} \
private:

# define J_SINGLETON(Class) \
	J_PURE_SINGLETON(Class) \
	Class() {} \
	virtual ~Class() {}

# define J_STRING(text) static const QString text##_str( QLatin1String( #text ) );

#define J_PURE_EXTENSION(Extension,XPath) \
	public: \
		inline static const StanzaExtensionMeta &meta() \
		{ \
			static StanzaExtensionMeta staticExtensionMeta( #Extension , XPath ); \
			return staticExtensionMeta; \
		} \
		virtual QString xPath() const { return meta().xpath; } \
		virtual int extensionType() const { return meta().type; } \
		virtual QByteArray extensionName() const { return meta().name; } \
	private:

#define J_EXTENSION(Extension,XPath) \
	public: \
		virtual StanzaExtension *fromNode( const QDomElement &node ) const { return new Extension( node ); } \
		J_PURE_EXTENSION(Extension,XPath)

#define J_FEATURE(XPath) \
	public: \
		const QString &xPath() const \
		{ \
			static const QString xpath( XPath ); \
			return xpath; \
		}

# ifndef J_FORELEMENTS

class ForElementContainer
{
public:
	inline ForElementContainer( const QDomNode &parent ) : list(parent.childNodes()), current(list.item(0).toElement()), i(0), test(true) {}
	QDomNodeList list;
	QDomElement current;
	uint i;
	bool test;
	inline void check() { while((current=list.item(++i).toElement()).isNull() && i<list.length()) {} test=true; }
};

# define J_FORELEMENTS( _node_, _parent_ ) \
	for( ForElementContainer _container_(_parent_); _container_.i<_container_.list.length(); _container_.check() ) \
		for( _node_=_container_.current; _container_.test; _container_.test=false )

# endif // J_FORELEMENTS

# ifndef forelements
#  define forelements J_FORELEMENTS
# endif

namespace J_NAMESPACE
{
	JREEN_EXPORT QDomElement createElement( QDomDocument *doc, const QString &name, const QString &value = QString() );
	JREEN_EXPORT QDomElement createElement( QDomElement parent, const QString &name, const QString &value = QString() );
}

#endif // JREEN_H
