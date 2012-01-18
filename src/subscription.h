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

#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

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

#endif // SUBSCRIPTION_H
