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

#include "saslfeature.h"
#include "client_p.h"
#include "../3rdparty/simplesasl/simplesasl.h"
#include <QUrl>
#include <QDebug>

namespace Jreen
{

SASLFeature::SASLFeature() : StreamFeature(SASL)
{
	QCA::init();
	QCA::setAppName("qutim");	
	m_depth = 0;
	qDebug() << QCA::supportedFeatures();
	if (!QCA::isSupported("sasl"))
		QCA::insertProvider(XMPP::createProviderSimpleSASL());
}

void SASLFeature::init()
{
	Q_ASSERT(!m_sasl);
	m_sasl.reset(new QCA::SASL(this));
	m_sasl->setConstraints(QCA::SASL::AllowPlain);
	connect(m_sasl.data(), SIGNAL(clientStarted(bool,QByteArray)),
			this, SLOT(onClientStarted(bool,QByteArray)));
	connect(m_sasl.data(), SIGNAL(nextStep(QByteArray)),
			this, SLOT(onNextStep(QByteArray)));
	connect(m_sasl.data(), SIGNAL(needParams(QCA::SASL::Params)),
			this, SLOT(onNeedParams(QCA::SASL::Params)));
	connect(m_sasl.data(), SIGNAL(authCheck(QString,QString)),
			this, SLOT(onAuthCheck(QString,QString)));
	// Don't listen it for sure
//	connect(m_sasl.data(), SIGNAL(error()), this, SLOT(onError()));
}

void SASLFeature::reset()
{
	m_depth = 0;
	m_mechs.clear();
	m_sasl.reset(0);
}

bool SASLFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(attributes);
	qDebug() << Q_FUNC_INFO << name << uri;
	return uri == QLatin1String("urn:ietf:params:xml:ns:xmpp-sasl");
}

void SASLFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
	qDebug() << Q_FUNC_INFO << m_depth << name;
	if (m_depth == 1) {
		if (name == QLatin1String("mechanisms")) {
			m_state = AtMechanisms;
			m_mechs.clear();
		} else if (name == QLatin1String("challenge")) {
			m_state = AtChallenge;
		}
	} else if (m_depth == 2 && name == QLatin1String("mechanism")) {
		m_state = AtMechanism;
	}
}

void SASLFeature::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(uri);
	if (m_depth == 2 && m_state == AtMechanism)
		m_state = AtMechanisms;
	else if (m_depth == 1) {
		qDebug() << Q_FUNC_INFO << name << m_mechs;
		m_state = AtStart;
		if (name == QLatin1String("success"))
			m_info->completed(StreamInfo::Authorized | StreamInfo::ResendHeader);
		if (name == QLatin1String("failure"))
			onError();
	}
	m_depth--;
}

void SASLFeature::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtMechanism) {
		qDebug() << Q_FUNC_INFO << "mechanism" << text;
		m_mechs.append(text.toString());
	} else if (m_state == AtChallenge) {
		qDebug() << Q_FUNC_INFO << "challenge" << text;
		m_sasl->putStep(QByteArray::fromBase64(text.toString().toLatin1()));
	}
}

bool SASLFeature::isActivatable()
{
	return !m_mechs.isEmpty();
}

bool SASLFeature::activate()
{
	init();
	m_sasl->setPassword(QCA::SecureArray(m_info->password().toUtf8()));
	m_sasl->setUsername(m_info->jid().node());
	m_sasl->setRealm(m_info->jid().domain());
	m_sasl->setAuthzid(m_info->jid().bare());
	m_sasl->setConstraints(QCA::SASL::AllowPlain);
	m_sasl->startClient("xmpp", QUrl::toAce(m_info->jid().domain()), m_mechs, QCA::SASL::AllowClientSendFirst);
	qDebug() << Q_FUNC_INFO << m_sasl->mechanismList();
	return true;
}

void SASLFeature::onClientStarted(bool init, const QByteArray &data)
{
	qDebug() << Q_FUNC_INFO << init << data << m_sasl->mechanism();
	QXmlStreamWriter *writer = ClientPrivate::get(m_client)->writer;
	writer->writeStartElement(QLatin1String("auth"));
	writer->writeDefaultNamespace(QLatin1String("urn:ietf:params:xml:ns:xmpp-sasl"));
	writer->writeAttribute(QLatin1String("mechanism"), m_sasl->mechanism());
	if (init)
		writer->writeCharacters(QString::fromLatin1(data.toBase64()));
	writer->writeEndElement();
//	writer->writeCharacters(QString());
}

void SASLFeature::onNextStep(const QByteArray &data)
{
	qDebug() << Q_FUNC_INFO << data;
	QXmlStreamWriter *writer = ClientPrivate::get(m_client)->writer;
	writer->writeStartElement(QLatin1String("response"));
	writer->writeDefaultNamespace(QLatin1String("urn:ietf:params:xml:ns:xmpp-sasl"));
	writer->writeCharacters(QString::fromLatin1(data.toBase64()));
	writer->writeEndElement();
}

void SASLFeature::onNeedParams(const QCA::SASL::Params &params)
{
	qDebug() << Q_FUNC_INFO << params.needPassword() << params.needUsername()
			 << params.canSendAuthzid() << params.canSendRealm();
	if (params.needPassword())
		m_sasl->setPassword(QCA::SecureArray(m_info->password().toUtf8()));
	if (params.needUsername())
		m_sasl->setUsername(m_info->jid().node());
	if (params.canSendRealm())
		m_sasl->setRealm(m_info->jid().domain());
//	if (params.canSendAuthzid())
//		m_sasl->setAuthzid(m_info->jid().bare());
	m_sasl->continueAfterParams();
}

void SASLFeature::onAuthCheck(const QString &user, const QString &authzid)
{
	qDebug() << Q_FUNC_INFO << user << authzid;
	m_sasl->continueAfterAuthCheck();
}

void SASLFeature::onError()
{
	m_info->completed(StreamInfo::AuthorizationFailed);
	qDebug() << Q_FUNC_INFO << (m_sasl ? m_sasl->errorCode() : -1);
}
}
