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

J_STRING(delay)
J_STRING(stamp)

DelayedDelivery::DelayedDelivery(const JID& from, const QDateTime &date_time, const QString &reason)
{
	m_from = from;
	m_date_time = date_time;
	m_reason = reason;
}

DelayedDelivery::DelayedDelivery(const QDomElement &node)
{
	if(node.isNull())
		return;
	if(!(node.nodeName() == QLatin1String("x") || node.nodeName() == delay_str))
		return;
	if(node.namespaceURI() != ConstString::xmlns_delay)
		return;
	m_reason = node.text();
	m_date_time = Util::fromStamp(node.attribute(stamp_str));
	m_from = node.attribute(ConstString::from);
}

void DelayedDelivery::writeXml(QXmlStreamWriter *writer) const
{
	if (!m_date_time.isValid())
		return;
	writer->writeStartElement(delay_str);
	writer->writeAttribute(stamp_str, Util::toStamp(m_date_time));
	writer->writeAttribute(ConstString::xmlns, ConstString::xmlns_delay);
	writer->writeAttribute(ConstString::from, m_from);
	writer->writeCharacters(m_reason);
	writer->writeEndElement();
}
}
