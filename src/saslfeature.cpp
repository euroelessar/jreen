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

#include "saslfeature_p.h"
#include "client_p.h"
#include <QUrl>
#include <QCoreApplication>
#include "logger.h"

#ifdef HAVE_SIMPLESASL
# include "../3rdparty/simplesasl/simplesasl.h"
#endif

#define NS_SASL QLatin1String("urn:ietf:params:xml:ns:xmpp-sasl")

namespace Jreen
{

SASLFeature::SASLFeature() : StreamFeature(SASL)
{
	QCA::init();
	QCA::setAppName(QCoreApplication::applicationName());
	m_depth = 0;
	m_isSupported = QCA::isSupported("sasl");
#ifdef HAVE_SIMPLESASL
	if (!m_isSupported) {
		QCA::insertProvider(XMPP::createProviderSimpleSASL());
		m_isSupported = true;
	}
#endif
}

void SASLFeature::init()
{
	if (!m_isSupported)
		return;
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
	if (!m_isSupported)
		return;
	m_depth = 0;
	m_mechs.clear();
	m_sasl.reset(0);
}

bool SASLFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	if (!m_isSupported)
		return false;
	Q_UNUSED(name);
	Q_UNUSED(attributes);
	return uri == NS_SASL;
}

void SASLFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_ASSERT(m_isSupported);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
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
	Q_ASSERT(m_isSupported);
	Q_UNUSED(uri);
	if (m_depth == 2 && m_state == AtMechanism)
		m_state = AtMechanisms;
	else if (m_depth == 1) {
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
	Q_ASSERT(m_isSupported);
	if (m_state == AtMechanism) {
		m_mechs.append(text.toString());
	} else if (m_state == AtChallenge) {
		m_sasl->putStep(QByteArray::fromBase64(text.toString().toLatin1()));
	}
}

bool SASLFeature::isActivatable()
{
	return m_isSupported && !m_mechs.isEmpty();
}

bool SASLFeature::activate()
{
	if (!m_isSupported)
		return false;
	init();
	m_sasl->setConstraints(QCA::SASL::AllowPlain);
	m_sasl->startClient("xmpp", QUrl::toAce(m_info->jid().domain()), m_mechs, QCA::SASL::AllowClientSendFirst);
	return true;
}

void SASLFeature::onClientStarted(bool init, const QByteArray &data)
{
	QXmlStreamWriter *writer = m_info->writer();
	writer->writeStartElement(QLatin1String("auth"));
	writer->writeDefaultNamespace(NS_SASL);
	writer->writeAttribute(QLatin1String("mechanism"), m_sasl->mechanism());
	if (init)
		writer->writeCharacters(QString::fromLatin1(data.toBase64()));
	writer->writeEndElement();
}

void SASLFeature::onNextStep(const QByteArray &data)
{
	QXmlStreamWriter *writer = m_info->writer();
	writer->writeStartElement(QLatin1String("response"));
	writer->writeDefaultNamespace(NS_SASL);
	writer->writeCharacters(QString::fromLatin1(data.toBase64()));
	writer->writeEndElement();
}

void SASLFeature::onNeedParams(const QCA::SASL::Params &params)
{
	if (params.needPassword())
		m_sasl->setPassword(QCA::SecureArray(m_info->password().toUtf8()));
	if (params.needUsername())
		m_sasl->setUsername(m_info->jid().node());
	if (params.canSendRealm())
		m_sasl->setRealm(m_info->jid().domain());
	// ???
	// Why SASL tells me that I can send Authzid?
	/*if (params.canSendAuthzid() && m_info->jid().domain() != QLatin1String("chat.facebook.com"))
		m_sasl->setAuthzid(m_info->jid().bare());*/
	m_sasl->continueAfterParams();
}

void SASLFeature::onAuthCheck(const QString &user, const QString &authzid)
{
	Q_UNUSED(user);
	Q_UNUSED(authzid);
	m_sasl->continueAfterAuthCheck();
}

void SASLFeature::onError()
{
	m_info->completed(StreamInfo::AuthorizationFailed);
	Logger::debug() << Q_FUNC_INFO << (m_sasl ? m_sasl->errorCode() : -1);
}
}
