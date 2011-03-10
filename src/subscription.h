/****************************************************************************
 *  subscription.h
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
