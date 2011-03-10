/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "sessionfeature_p.h"
#include "client.h"
#include "iq.h"
#include <QXmlStreamWriter>
#include <QStringList>
#include <QDebug>

#define NS_SESSION QLatin1String("urn:ietf:params:xml:ns:xmpp-session")

namespace Jreen
{
class SessionQuery : public StanzaExtension
{
	J_EXTENSION(Jreen::SessionQuery, "")
	public:
		SessionQuery() {}
};

class SessionQueryFactory : public StanzaExtensionFactory<SessionQuery>
{
public:
	SessionQueryFactory() {}
	QStringList features() const { return QStringList(); }
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
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

void SessionQueryFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Q_UNUSED(extension);
	writer->writeEmptyElement(QLatin1String("session"));
	writer->writeDefaultNamespace(NS_SESSION);
}

StanzaExtension::Ptr SessionQueryFactory::createExtension()
{
	return StanzaExtension::Ptr(new SessionQuery());
}

SessionFeature::SessionFeature() : StreamFeature(Custom)
{
	m_hasFeature = false;
}

void SessionFeature::setStreamInfo(StreamInfo *info)
{
	StreamFeature::setStreamInfo(info);
	info->client()->registerStanzaExtension(new SessionQueryFactory);
}

void SessionFeature::reset()
{
	m_hasFeature = false;
}

bool SessionFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	qDebug() << Q_FUNC_INFO;
	return name == QLatin1String("session") && uri == NS_SESSION;
}

void SessionFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	qDebug() << Q_FUNC_INFO;
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
	IQ::Ptr iq(new IQ(IQ::Set, JID()));
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
