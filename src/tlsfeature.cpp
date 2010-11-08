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

#include "tlsfeature.h"
#include "tlsdatastream.h"
#include <QXmlStreamWriter>
#include <QDebug>

#define NS_TLS QLatin1String("urn:ietf:params:xml:ns:xmpp-tls")

namespace jreen
{
	Q_GLOBAL_STATIC(QCA::Initializer, qcaInit)
	
	TLSFeature::TLSFeature() : StreamFeature(SecurityLayer)
	{
		QCA::setAppName("qutim");
		qcaInit();
		m_required = false;
		m_available = false;
		if(QCA::isSupported("tls")) {
			m_tls = new QCA::TLS(this);
			m_tls->setTrustedCertificates(QCA::systemStore());
			connect(m_tls, SIGNAL(handshaken()), SLOT(onHandshaken()));
			connect(m_tls, SIGNAL(closed()), SLOT(onClosed()));
			connect(m_tls, SIGNAL(error()), SLOT(onError()));
		} else {
			qWarning("Jreen: TLS is not provided by QCA");
			m_tls = 0;
		}
	}
	
	void TLSFeature::reset()
	{
		m_required = false;
		m_available = false;
	}
	
	bool TLSFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		// All other methods shouldn't be called is canParse returnes false
		if (!m_tls)
			return false;
		Q_UNUSED(name);
		Q_UNUSED(attributes);
		qDebug() << Q_FUNC_INFO << name << uri;
		return uri == NS_TLS;
	}
	
	void TLSFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		if (name == QLatin1String("starttls"))
			m_available = true;
		else if (name == QLatin1String("required"))
			m_required = true;
		else if (name == QLatin1String("proceed")) {
			m_info->addDataStream(new TLSDataStream(m_tls));
			m_tls->startClient(m_info->jid().domain());
		}
//		Q_UNUSED(uri);
//		m_depth++;
//		qDebug() << Q_FUNC_INFO << m_depth << name;
//		if (m_depth == 1) {
//			if (name == QLatin1String("mechanisms")) {
//				m_state = AtMechanisms;
//				m_mechs.clear();
//			} else if (name == QLatin1String("challenge")) {
//				m_state = AtChallenge;
//			}
//		} else if (m_depth == 2 && name == QLatin1String("mechanism")) {
//			m_state = AtMechanism;
//		}
	}
	
	void TLSFeature::handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
//		if (m_depth == 2 && m_state == AtMechanism)
//			m_state = AtMechanisms;
//		else if (m_depth == 1) {
//			qDebug() << Q_FUNC_INFO << m_mechs;
//			m_state = AtStart;
//			if (name == QLatin1String("success"))
//				m_info->completed(StreamInfo::Authorized);
//		}
//		m_depth--;
	}
	
	void TLSFeature::handleCharacterData(const QStringRef &text)
	{
		Q_UNUSED(text);
//		if (m_state == AtMechanism) {
//			qDebug() << Q_FUNC_INFO << "mechanism" << text;
//			m_mechs.append(text.toString());
//		} else if (m_state == AtChallenge) {
//			qDebug() << Q_FUNC_INFO << "challenge" << text;
//			//			if (m_firstStep)
//			//				m_tls->putServerFirstStep("DIGEST-MD5");
//			m_tls->putStep(QByteArray::fromBase64(text.toString().toLatin1()));
//			m_firstStep = false;
//		}
	}
	
	bool TLSFeature::isActivatable()
	{
		return m_tls && m_available;
	}
	
	bool TLSFeature::activate()
	{
		QXmlStreamWriter *writer = m_info->writer();
		writer->writeEmptyElement(QLatin1String("starttls"));
		writer->writeDefaultNamespace(NS_TLS);
		writer->writeCharacters(QString());
//		qDebug() << Q_FUNC_INFO << m_info->password();
//		m_tls->setPassword(QCA::SecureArray(m_info->password().toUtf8()));
//		m_tls->setUsername(m_info->jid().node());
//		m_tls->setRealm(m_info->jid().domain());
//		m_tls->setAuthzid(m_info->jid().bare());
//		m_tls->startClient("xmpp", QUrl::toAce(m_info->jid().domain()), m_mechs, QCA::TLS::AllowClientSendFirst);
		return true;
	}
	
	void TLSFeature::onHandshaken()
	{
		qDebug() << Q_FUNC_INFO;
		m_tls->continueAfterStep();
		m_info->completed();
	}

	void TLSFeature::onClosed()
	{
		qDebug() << Q_FUNC_INFO;
	}

	void TLSFeature::onError()
	{
		qDebug() << Q_FUNC_INFO;
	}
}
