/****************************************************************************
 *  delayeddelivery.h
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

#ifndef DELAYEDDELIVERY_H
#define DELAYEDDELIVERY_H

#include "stanzaextension.h"
#include "jid.h"
#include <QDateTime>

namespace Jreen
{

//XEP 0203
//http://xmpp.org/extensions/xep-0203.html

class JREEN_EXPORT DelayedDelivery : public StanzaExtension
{
	J_EXTENSION(Jreen::DelayedDelivery,
				"/presence/delay[@xmlns='jabber:x:delay']"
				"|/message/delay[@xmlns='jabber:x:delay']"
				"|/presence/x[@xmlns='jabber:x:delay']"
				"|/message/x[@xmlns='jabber:x:delay']")
public:
	DelayedDelivery(const JID& from, const QDateTime &date_time, const QString &reason = QString())
	  : m_from(from),m_date_time(date_time),m_reason(reason) {};
	DelayedDelivery() {}
	inline const JID &from() const { return m_from; }
	inline const QString &reason() const { return m_reason; }
	inline const QDateTime &dateTime() const { return m_date_time; }
	virtual ~DelayedDelivery() {};
private:
	JID m_from;
	QDateTime m_date_time;
	QString m_reason;
};

}

J_DECLARE_EXTENSION(Jreen::DelayedDelivery)

#endif // DELAYEDDELIVERY_H
