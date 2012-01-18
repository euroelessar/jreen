/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "pingfactory_p.h"
#include <QStringList>
#include <QXmlStreamWriter>
#define NS_PING QLatin1String("urn:xmpp:ping")

namespace Jreen {

PingFactory::PingFactory()
{

}

PingFactory::~PingFactory()
{

}

QStringList PingFactory::features() const
{
	return QStringList(NS_PING);
}

bool PingFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("ping") && uri == NS_PING;
}

void PingFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
 const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
}

void PingFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void PingFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void PingFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Q_UNUSED(extension);
	writer->writeStartElement(QLatin1String("ping"));
	writer->writeDefaultNamespace(NS_PING);
	writer->writeEndElement();
}

Payload::Ptr PingFactory::createPayload()
{
	return Payload::Ptr(new Ping());
}

} // namespace Jreen
