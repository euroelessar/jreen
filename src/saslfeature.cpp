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
#include <QUrl>
#include <QDebug>

namespace jreen
{
	Q_GLOBAL_STATIC(QCA::Initializer, qcaInit)
	
	SASLFeature::SASLFeature() : StreamFeature(SASL)
	{
		QCA::setAppName("qutim");
		qcaInit();
		m_depth = 0;
		m_firstStep = true;
		qDebug() << QCA::supportedFeatures();
		if(QCA::isSupported("sasl")) {
			m_sasl = new QCA::SASL(this);
			connect(m_sasl, SIGNAL(clientStarted(bool,QByteArray)),
					this, SLOT(onClientStarted(bool,QByteArray)));
			connect(m_sasl, SIGNAL(nextStep(QByteArray)),
					this, SLOT(onNextStep(QByteArray)));
			connect(m_sasl, SIGNAL(needParams(QCA::SASL::Params)),
					this, SLOT(onNeedParams(QCA::SASL::Params)));
			connect(m_sasl, SIGNAL(authCheck(QString,QString)),
					this, SLOT(onAuthCheck(QString,QString)));
			connect(m_sasl, SIGNAL(error()), this, SLOT(onError()));
		} else {
			qWarning("Jreen: SASL is not provided by QCA");
			m_sasl = 0;
		}
	}
	
	void SASLFeature::reset()
	{
		m_firstStep = true;
		m_depth = 0;
		m_mechs.clear();
	}
	
	bool SASLFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		// All other methods shouldn't be called is canParse returnes false
		if (!m_sasl)
			return false;
		Q_UNUSED(name);
		Q_UNUSED(attributes);
		qDebug() << Q_FUNC_INFO << name << uri;
		return uri == QLatin1String("urn:ietf:params:xml:ns:xmpp-sasl");
	}

	void SASLFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
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
		if (m_depth == 2 && m_state == AtMechanism)
			m_state = AtMechanisms;
		else if (m_depth == 1) {
			qDebug() << Q_FUNC_INFO << m_mechs;
			m_state = AtStart;
			if (name == QLatin1String("success"))
				m_info->completed(StreamInfo::Authorized | StreamInfo::ResendHeader);
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
//			if (m_firstStep)
//				m_sasl->putServerFirstStep("DIGEST-MD5");
			m_sasl->putStep(QByteArray::fromBase64(text.toString().toLatin1()));
			m_firstStep = false;
		}
	}

	bool SASLFeature::isActivatable()
	{
		return m_sasl && !m_mechs.isEmpty();
	}

	bool SASLFeature::activate()
	{
		qDebug() << Q_FUNC_INFO << m_info->password();
		m_sasl->setPassword(QCA::SecureArray(m_info->password().toUtf8()));
		m_sasl->setUsername(m_info->jid().node());
		m_sasl->setRealm(m_info->jid().domain());
		m_sasl->setAuthzid(m_info->jid().bare());
		m_sasl->startClient("xmpp", QUrl::toAce(m_info->jid().domain()), m_mechs, QCA::SASL::AllowClientSendFirst);
		return true;
	}
	
	void SASLFeature::onClientStarted(bool init, const QByteArray &data)
	{
		qDebug() << Q_FUNC_INFO << init << data << m_sasl->mechanism();
		QXmlStreamWriter *writer = ClientPrivate::get(m_client)->writer;
		writer->writeEmptyElement(QLatin1String("auth"));
		writer->writeDefaultNamespace(QLatin1String("urn:ietf:params:xml:ns:xmpp-sasl"));
		writer->writeAttribute(QLatin1String("mechanism"), m_sasl->mechanism());
		writer->writeCharacters(QString());
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
		if (params.canSendAuthzid())
			m_sasl->setAuthzid(m_info->jid().bare());
		m_sasl->continueAfterParams();
	}
	
	void SASLFeature::onAuthCheck(const QString &user, const QString &authzid)
	{
		qDebug() << Q_FUNC_INFO << user << authzid;
		m_sasl->continueAfterAuthCheck();
	}
	
	void SASLFeature::onError()
	{
		qDebug() << Q_FUNC_INFO << m_sasl->errorCode();
	}
}
