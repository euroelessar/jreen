/****************************************************************************
 *  delayeddelivery.cpp
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

#include "delayeddelivery.h"
#include "util.h"
#include "jstrings.h"
#include <QXmlStreamWriter>

namespace Jreen
{

class DelayedDeliveryPrivate
{
public:
	JID from;
	QDateTime dateTime;
	QString reason;
};

DelayedDelivery::DelayedDelivery(const JID &from, const QDateTime &dateTime, const QString &reason)
    : d_ptr(new DelayedDeliveryPrivate)
{
	Q_D(DelayedDelivery);
	d->from = from;
	d->dateTime = dateTime;
	d->reason = reason;
}

DelayedDelivery::~DelayedDelivery()
{
}

JID DelayedDelivery::from() const
{
	return d_func()->from;
}

QString DelayedDelivery::reason() const
{
	return d_func()->reason;
}

QDateTime DelayedDelivery::dateTime() const
{
	return d_func()->dateTime;
}

}
