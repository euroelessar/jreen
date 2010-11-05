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

namespace jreen
{

DelayedDelivery::DelayedDelivery(const JID& from, const QDateTime &date_time, const QString &reason)
{
	m_from = from;
	m_date_time = date_time;
	m_reason = reason;
}

}
