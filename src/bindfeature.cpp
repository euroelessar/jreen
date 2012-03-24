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

#include "bindfeature_p.h"
#include "client.h"
#include "iq_p.h"
#include <QXmlStreamWriter>
#include <QStringList>
#include "logger.h"

#define NS_BIND QLatin1String("urn:ietf:params:xml:ns:xmpp-bind")

namespace Jreen
{
class BindQuery : public Payload
{
	J_PAYLOAD(Jreen::BindQuery)
	public:
		BindQuery(const JID &jid, const QString &resource, bool bind = true) 
	  : m_jid(jid), m_resource(resource), m_bind(bind) {}
	JID jid() { return m_jid; }
	QString resource() { return m_resource; }
	bool isBind() { return m_bind; }
private:
	JID m_jid;
	QString m_resource;
	bool m_bind;
};

class JREEN_AUTOTEST_EXPORT BindQueryFactory : public PayloadFactory<BindQuery>
{
public:
	BindQueryFactory() : m_bind(true), m_depth(0), m_state(AtStart) {}
	QStringList features() const { return QStringList(); }
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	enum State { AtStart, AtResource, AtJid };
	bool m_bind;
	QString m_resource;
	JID m_jid;
	int m_depth;
	State m_state;
};

bool BindQueryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return (name == QLatin1String("bind") || name == QLatin1String("unbind")) && uri == NS_BIND;
}

void BindQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
	if (m_depth == 1) {
		m_state = AtStart;
		m_bind = name == QLatin1String("bind");
		m_jid.clear();
		m_resource.clear();
	} else if (m_depth == 2) {
		if (name == QLatin1String("jid"))
			m_state = AtJid;
		else if (name == QLatin1String("resource"))
			m_state = AtResource;
		else
			m_state = AtStart;
	}
}

void BindQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
	if (m_depth == 1)
		m_state = AtStart;
}

void BindQueryFactory::handleCharacterData(const QStringRef &text)
{
	if (m_depth == 2 && m_state == AtResource)
		m_resource = text.toString();
	else if (m_depth == 2 && m_state == AtJid)
		m_jid = text.toString();
}

void BindQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	BindQuery *query = se_cast<BindQuery*>(extension);
	writer->writeStartElement(QLatin1String(query->isBind() ? "bind" : "unbind"));
	writer->writeDefaultNamespace(NS_BIND);
	if (query->jid().isValid())
		writer->writeTextElement(QLatin1String("jid"), query->jid());
	else if (!query->resource().isEmpty())
		writer->writeTextElement(QLatin1String("resource"), query->resource());
	writer->writeEndElement();
}

Payload::Ptr BindQueryFactory::createPayload()
{
	return Payload::Ptr(new BindQuery(m_jid, m_resource, m_bind));
}

BindFeature::BindFeature() : StreamFeature(Custom)
{
	m_hasFeature = false;
}

void BindFeature::setStreamInfo(StreamInfo *info)
{
	StreamFeature::setStreamInfo(info);
	info->client()->registerPayload(new BindQueryFactory);
}

void BindFeature::reset()
{
	m_hasFeature = false;
}

bool BindFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	Logger::debug() << Q_FUNC_INFO;
	return name == QLatin1String("bind") && uri == NS_BIND;
}

void BindFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	Logger::debug() << Q_FUNC_INFO;
	m_hasFeature = true;
}

void BindFeature::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void BindFeature::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

bool BindFeature::isActivatable()
{
	return m_hasFeature;
}

bool BindFeature::activate()
{
	IQ::Ptr iq(new ConnectionIQ(IQ::Set, JID()));
	iq->addExtension(new BindQuery(JID(), m_info->jid().resource()));
	m_info->client()->send(*iq.data(), this, SLOT(onIQResult(Jreen::IQ,int)), 0);
	return true;
}

void BindFeature::onIQResult(const IQ &iq, int context)
{
	Q_ASSERT(context == 0);
	BindQuery::Ptr query = iq.payload<BindQuery>();
	if (query && iq.subtype() == IQ::Result) {
		m_info->setJID(query->jid());
		m_info->completed(StreamInfo::ActivateNext);
	}
}
}
