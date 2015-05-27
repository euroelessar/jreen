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

#ifndef JREEN_SUBSCRIPTION_H
#define JREEN_SUBSCRIPTION_H

#include "stanza.h"

namespace Jreen
{

//struct SubscriptionPrivate;

//class JREEN_EXPORT Subscription : public Stanza
//{
//	Q_DECLARE_PRIVATE(Subscription)
//public:
//	enum Type
//	{
//		Subscribe,                  /**> A subscription request. */
//		Subscribed,                 /**< A subscription notification. */
//		Unsubscribe,                /**< An unsubscription request. */
//		Unsubscribed,               /**< An unsubscription notification. */
//		Invalid                     /**< The stanza is invalid. */
//	};
//	Subscription(const QDomElement &node);
//	Subscription(Type type, const JID& to, const QString &status = QString(), const QString &xmllang = QString());
//	inline Subscription &operator =(const Subscription &stanza)
//		{ return *static_cast<Subscription *>(&Jreen::Stanza::operator =(stanza)); }
//	Type subtype() const;
//	const QString &status(const QString &lang = QString()) const;
//	void writeXml(QXmlStreamWriter *writer) const;
//};

}

#endif // JREEN_SUBSCRIPTION_H
