/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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

#include "attentionfactory_p.h"
#include <QMap>
#include <QXmlStreamReader>
#include <QStringList>
#include "jstrings.h"

#define NS_ATTENTION QLatin1String("urn:xmpp:attention:0")

namespace Jreen {

AttentionFactory::AttentionFactory()
{

}

AttentionFactory::~AttentionFactory()
{

}

QStringList AttentionFactory::features() const
{
	return QStringList(NS_ATTENTION);
}

bool AttentionFactory::canParse(const QStringRef &name,
								const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("attention") && uri == NS_ATTENTION;
}

void AttentionFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	Q_UNUSED(name);
}

void AttentionFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void AttentionFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void AttentionFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	writer->writeStartElement(QLatin1String("attention"));
	writer->writeDefaultNamespace(NS_ATTENTION);
	writer->writeEndElement();
}

Payload::Ptr AttentionFactory::createPayload()
{
	return Payload::Ptr(new Attention());
}

} // namespace Jreen
