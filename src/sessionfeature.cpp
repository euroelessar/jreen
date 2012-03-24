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

#include "sessionfeature_p.h"
#include "client.h"
#include "iq_p.h"
#include <QXmlStreamWriter>
#include <QStringList>
#include "logger.h"

#define NS_SESSION QLatin1String("urn:ietf:params:xml:ns:xmpp-session")

namespace Jreen
{
class SessionQuery : public Payload
{
	J_PAYLOAD(Jreen::SessionQuery)
	public:
		SessionQuery() {}
};

class JREEN_AUTOTEST_EXPORT SessionQueryFactory : public PayloadFactory<SessionQuery>
{
public:
	SessionQueryFactory() {}
	QStringList features() const { return QStringList(); }
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
};

bool SessionQueryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("session") && uri == NS_SESSION;
}

void SessionQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
}

void SessionQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void SessionQueryFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void SessionQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Q_UNUSED(extension);
	writer->writeEmptyElement(QLatin1String("session"));
	writer->writeDefaultNamespace(NS_SESSION);
}

Payload::Ptr SessionQueryFactory::createPayload()
{
	return Payload::Ptr(new SessionQuery());
}

SessionFeature::SessionFeature() : StreamFeature(Custom)
{
	m_hasFeature = false;
}

void SessionFeature::setStreamInfo(StreamInfo *info)
{
	StreamFeature::setStreamInfo(info);
	info->client()->registerPayload(new SessionQueryFactory);
}

void SessionFeature::reset()
{
	m_hasFeature = false;
}

bool SessionFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	Logger::debug() << Q_FUNC_INFO;
	return name == QLatin1String("session") && uri == NS_SESSION;
}

void SessionFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	Logger::debug() << Q_FUNC_INFO;
	m_hasFeature = true;
}

void SessionFeature::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void SessionFeature::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

bool SessionFeature::isActivatable()
{
	return m_hasFeature;
}

bool SessionFeature::activate()
{
	IQ::Ptr iq(new ConnectionIQ(IQ::Set, JID()));
	iq->addExtension(new SessionQuery());
	m_info->client()->send(*iq.data(), this, SLOT(onIQResult(Jreen::IQ,int)), 0);
	return true;
}

void SessionFeature::onIQResult(const IQ &iq, int context)
{
	Q_ASSERT(context == 0);
	if (iq.subtype() == IQ::Result) {
		m_info->completed(StreamInfo::Connected);
	}
}
}
