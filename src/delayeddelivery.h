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

namespace jreen
{

class DelayedDelivery : public StanzaExtension
{
	J_EXTENSION(DelayedDelivery,
		   "/presence/delay[@xmlns='jabber:x:delay']"
		   "|/message/delay[@xmlns='jabber:x:delay']"
		   "|/presence/x[@xmlns='jabber:x:delay']"
		   "|/message/x[@xmlns='jabber:x:delay']")
public:
	DelayedDelivery(const JID& from, const QDateTime &date_time, const QString &reason = QString());
	DelayedDelivery(const QDomElement &node = QDomElement());
	QDomElement node(QDomDocument *document) const;
	inline const JID &from() const { return m_from; }
	inline const QString &reason() const { return m_reason; }
	inline const QDateTime &dateTime() const { return m_date_time; }
private:
	QDateTime m_date_time;
	QString m_reason;
	JID m_from;
};

}

#endif // DELAYEDDELIVERY_H
