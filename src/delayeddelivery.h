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

class DelayedDeliveryPrivate;

//XEP 0203
//http://xmpp.org/extensions/xep-0203.html

class JREEN_EXPORT DelayedDelivery : public Payload
{
	Q_DECLARE_PRIVATE(DelayedDelivery)
	J_PAYLOAD(Jreen::DelayedDelivery)
public:
	DelayedDelivery(const JID &from, const QDateTime &dateTime, const QString &reason = QString());
	virtual ~DelayedDelivery();

	JID from() const;
	QString reason() const;
	QDateTime dateTime() const;
private:
	QScopedPointer<DelayedDeliveryPrivate> d_ptr;
};

}

#endif // DELAYEDDELIVERY_H
