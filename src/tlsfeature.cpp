/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "tlsfeature_p.h"
#include "tlsdatastream_p.h"
#include "client.h"
#include <QXmlStreamWriter>
#include "logger.h"
#include <QCoreApplication>

#define NS_TLS QLatin1String("urn:ietf:params:xml:ns:xmpp-tls")

namespace Jreen
{

TLSFeature::TLSFeature() : StreamFeature(SecurityLayer)
{
	QCA::init();
	QCA::setAppName(QCoreApplication::applicationName());
	m_required = false;
	m_available = false;
	m_hasTls = QCA::isSupported("tls");
	if (!m_hasTls)
		Logger::warning() << "Jreen: TLS is not provided by QCA";
}

void TLSFeature::init()
{
	Q_ASSERT(m_hasTls);
	Q_ASSERT(!m_tls);
	m_tls.reset(new QCA::TLS(this));
	m_tls->setTrustedCertificates(QCA::systemStore());
	connect(m_tls.data(), SIGNAL(handshaken()), SLOT(onHandshaken()));
	connect(m_tls.data(), SIGNAL(closed()), SLOT(onClosed()));
	connect(m_tls.data(), SIGNAL(error()), SLOT(onError()));
}

void TLSFeature::setStreamInfo(StreamInfo *info)
{
	if(m_client)
		m_client->disconnect(this);
	StreamFeature::setStreamInfo(info);
	connect(m_client, SIGNAL(disconnected(Jreen::Client::DisconnectReason)),
	        SLOT(onDisconnected()));
}

void TLSFeature::reset()
{
	m_required = false;
	m_available = false;
}

bool TLSFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	// All other methods shouldn't be called is canParse returnes false
	if (!m_hasTls)
		return false;
	Q_UNUSED(uri);
	Q_UNUSED(name);
	Q_UNUSED(attributes);
	Logger::debug() << Q_FUNC_INFO << name << uri;
	return uri == NS_TLS;
}

void TLSFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	if (name == QLatin1String("starttls"))
		m_available = true;
	else if (name == QLatin1String("required"))
		m_required = true;
	//		Q_UNUSED(uri);
	//		m_depth++;
	//		Logger::debug() << Q_FUNC_INFO << m_depth << name;
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
	Q_UNUSED(uri);
	if (name == QLatin1String("proceed")) {
		Logger::debug() << Q_FUNC_INFO;
		m_info->addDataStream(new TLSDataStream(m_tls.data()));
		m_tls->startClient(m_info->jid().domain());
	}
	//		if (m_depth == 2 && m_state == AtMechanism)
	//			m_state = AtMechanisms;
	//		else if (m_depth == 1) {
	//			Logger::debug() << Q_FUNC_INFO << m_mechs;
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
	//			Logger::debug() << Q_FUNC_INFO << "mechanism" << text;
	//			m_mechs.append(text.toString());
	//		} else if (m_state == AtChallenge) {
	//			Logger::debug() << Q_FUNC_INFO << "challenge" << text;
	//			//			if (m_firstStep)
	//			//				m_tls->putServerFirstStep("DIGEST-MD5");
	//			m_tls->putStep(QByteArray::fromBase64(text.toString().toLatin1()));
	//			m_firstStep = false;
	//		}
}

bool TLSFeature::isActivatable()
{
	return m_hasTls && m_available;
}

bool TLSFeature::activate()
{
	init();
	QXmlStreamWriter *writer = m_info->writer();
	writer->writeEmptyElement(QLatin1String("starttls"));
	writer->writeDefaultNamespace(NS_TLS);
	writer->writeCharacters(QString());
	//		Logger::debug() << Q_FUNC_INFO << m_info->password();
	//		m_tls->setPassword(QCA::SecureArray(m_info->password().toUtf8()));
	//		m_tls->setUsername(m_info->jid().node());
	//		m_tls->setRealm(m_info->jid().domain());
	//		m_tls->setAuthzid(m_info->jid().bare());
	//		m_tls->startClient("xmpp", QUrl::toAce(m_info->jid().domain()), m_mechs, QCA::TLS::AllowClientSendFirst);
	return true;
}

void TLSFeature::onHandshaken()
{
	Logger::debug() << Q_FUNC_INFO;
	m_tls->continueAfterStep();
	m_info->completed();
}

void TLSFeature::onClosed()
{
	Logger::debug() << Q_FUNC_INFO;
	reset();
}

void TLSFeature::onError()
{
	Logger::debug() << Q_FUNC_INFO;
	reset();
	m_client->disconnectFromServer(true);
}

void TLSFeature::onDisconnected()
{
	m_tls.reset(0);
	Logger::debug() << Q_FUNC_INFO;
}

} //namespace Jreen


