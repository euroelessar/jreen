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

#ifndef JREEN_H
#define JREEN_H

# ifndef __cplusplus
#  error "C++ is needed"
# endif // __cplusplus

# include <QtGlobal>
# include <QtCore/QString>
# include <QtCore/QXmlStreamWriter> //needed on OS X (10.5) for QXmlStreamWriter typedef

# ifndef J_BUILD_STATIC
#  ifdef J_BUILD_INTERNAL
#   ifdef J_BUILD_LIBRARY
#    define JREEN_AUTOTEST_EXPORT Q_DECL_EXPORT
#   else
#    define JREEN_AUTOTEST_EXPORT Q_DECL_IMPORT
#   endif
#  endif
#  ifdef J_BUILD_LIBRARY
#   define JREEN_EXPORT Q_DECL_EXPORT
#  else
#   define JREEN_EXPORT Q_DECL_IMPORT
#  endif
# endif
# ifndef JREEN_AUTOTEST_EXPORT
#  define JREEN_AUTOTEST_EXPORT
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

#define J_PURE_EXTENSION(Extension,XPath) \
	public: \
		inline static const PayloadMeta &meta() \
		{ \
			static PayloadMeta staticExtensionMeta(#Extension , XPath); \
			return staticExtensionMeta; \
		} \
		virtual QString xPath() const { return meta().xpath; } \
		virtual int payloadType() const { return meta().type; } \
		virtual QByteArray extensionName() const { return meta().name; } \
	private:

#define J_FEATURE(XPath) \
	public: \
		const QString &xPath() const \
		{ \
			static const QString xpath; \
			return xpath; \
		}

			
class QXmlStreamAttributes;

namespace Jreen
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
	
	template <typename T>
	class XmlStreamFactory : public XmlStreamParser
	{
	public:
		virtual void serialize(T *obj, QXmlStreamWriter *writer) = 0;
	};

}

#endif // JREEN_H
