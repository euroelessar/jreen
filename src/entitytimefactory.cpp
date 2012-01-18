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

#include "entitytimefactory_p.h"
#include "util.h"
#include <QStringList>
#include <QXmlStreamWriter>

#define NS_TIME QLatin1String("urn:xmpp:time")

namespace Jreen
{
	EntityTimeFactory::EntityTimeFactory()
	{
		m_depth = 0;
		m_state = AtNowhere;
		m_tzo = 0;
	}
	
	EntityTimeFactory::~EntityTimeFactory()
	{
	}
	
	QStringList EntityTimeFactory::features() const
	{
		return QStringList(NS_TIME);
	}
	
	bool EntityTimeFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
		return name == QLatin1String("time") && uri == NS_TIME;
	}
	
	void EntityTimeFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
											   const QXmlStreamAttributes &attributes)
	{
		m_depth++;
		if (m_depth == 1) {
			m_state = AtNowhere;
			m_tzo = 0;
			m_utc = QDateTime();
		} else if (m_depth == 2) {
			if (name == QLatin1String("tzo"))
				m_state = AtTzo;
			else if (name == QLatin1String("utc"))
				m_state = AtUtc;
		}
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
	}
	
	void EntityTimeFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		if (m_depth == 2)
			m_state = AtNowhere;
		Q_UNUSED(name);
		Q_UNUSED(uri);
		m_depth--;
	}
	
	void EntityTimeFactory::handleCharacterData(const QStringRef &text)
	{
		if (m_state == AtUtc) {
			m_utc = Util::fromStamp(text.toString());
		} else if (m_state == AtTzo) {
			QString str = text.toString();
			int multiple = str.startsWith('-') ? -1 : 1;
			QTime delta = QTime::fromString(str.mid(1), QLatin1String("hh:mm"));
			m_tzo = multiple * (delta.hour() * 60 + delta.minute());
		}
		Q_UNUSED(text);
	}
	
	void EntityTimeFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
	{
		EntityTime *time = se_cast<EntityTime*>(extension);
		writer->writeStartElement(QLatin1String("time"));
		writer->writeDefaultNamespace(NS_TIME);
		if (time->universalTime().isValid()) {
			writer->writeTextElement(QLatin1String("utc"), Util::toStamp(time->universalTime()));
			QTime t(0, 0);
			t = t.addSecs(qAbs(time->timeZoneOffset()) * 60);
			QString tzo = QLatin1Char(time->timeZoneOffset() < 0 ? '-' : '+')
						  + t.toString(QLatin1String("hh:mm"));
			writer->writeTextElement(QLatin1String("tzo"), tzo);
		}
		writer->writeEndElement();
	}
	
	Payload::Ptr EntityTimeFactory::createPayload()
	{
		return Payload::Ptr(new EntityTime(m_tzo, m_utc));
	}
}
