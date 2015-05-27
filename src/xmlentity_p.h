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

#ifndef JREEN_XMLENTITY_P_H
#define JREEN_XMLENTITY_P_H

#include "jreen.h"
#include <QSharedPointer>

namespace Jreen
{
	class JREEN_EXPORT XmlEntity
	{
		Q_DISABLE_COPY(XmlEntity)
	public:
		typedef QSharedPointer<XmlEntity> Ptr;
		
		XmlEntity();
		virtual ~XmlEntity();
		
		static int registerXmlEntityType(const char *type);
		
		virtual int entityType() const = 0;
	};
	
	template <typename T>
	Q_INLINE_TEMPLATE T entity_cast(XmlEntity *se)
	{
		if (se && reinterpret_cast<T>(0)->staticXmlEntityType() == se->entityType())
			return static_cast<T>(se);
		return 0;
	}
}

#define J_ENTITY(Class, XPath) \
	public:  \
		typedef QSharedPointer<Class> Ptr; \
		static int staticXmlEntityType() \
		{ \
			static int entityType = 0; \
			if (!entityType) { \
				entityType = XmlEntity::registerXmlEntityType( #Class ); \
				Class *useFullNameWithNamespaces = reinterpret_cast< ::Class* >(0); \
				Q_UNUSED(useFullNameWithNamespaces); \
			} \
			return entityType; \
		} \
		virtual int entityType() const \
		{ \
			Q_UNUSED(static_cast<const ::Class*>(this)); \
			return staticEntityType(); \
		} \
	private:

#endif // JREEN_XMLENTITY_P_H
