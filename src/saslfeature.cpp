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

#define NS_SASL QLatin1String("urn:ietf:params:xml:ns:xmpp-sasl")

namespace Jreen
{

#ifdef USE_GSASL
static Gsasl *sasl_context = NULL;

static int callback_function(Gsasl *context, Gsasl_session *session, Gsasl_property prop)
{
	StreamInfo *info = reinterpret_cast<StreamInfo*>(gsasl_session_hook_get(session));
	if (!info) {
		jreenDebug() << Q_FUNC_INFO << context << session << prop;
		return;
	}
	Q_ASSERT(info);
	switch (prop) {
	case GSASL_SERVICE:
		gsasl_property_set(session, GSASL_SERVICE,  "xmpp");
		return GSASL_OK;
	case GSASL_HOSTNAME:
		gsasl_property_set(session, GSASL_HOSTNAME,    QUrl::toAce(info->jid().domain()));
		return GSASL_OK;
	case GSASL_REALM:
		gsasl_property_set(session, GSASL_REALM,    info->jid().domain().toUtf8());
		return GSASL_OK;
	case GSASL_AUTHID:
		gsasl_property_set(session, GSASL_AUTHID,   info->jid().node().toUtf8());
		return GSASL_OK;
//	case GSASL_AUTHZID:
//		gsasl_property_set(session, GSASL_AUTHZID,  info->jid().bare().toUtf8());
//		return GSASL_OK;
	case GSASL_PASSWORD:
		gsasl_property_set(session, GSASL_PASSWORD, info->password().toUtf8());
		return GSASL_OK;
	default:
		jreenWarning() << "SASL property request unhandled:" << prop;
		return GSASL_NO_CALLBACK;
	}
}

static void destroy_sasl_context()
{
	gsasl_done(sasl_context);
}

#else

static sasl_callback_t sasl_callbacks[] = {
	{ SASL_CB_GETREALM, NULL, NULL },
	{ SASL_CB_USER, NULL, NULL },
	{ SASL_CB_AUTHNAME, NULL, NULL },
	{ SASL_CB_PASS, NULL, NULL },
	{ SASL_CB_LIST_END, NULL, NULL }
};
#endif

static bool sasl_inited_successfully = false;
static bool sasl_tried_to_init = false;

SASLFeature::SASLFeature() : StreamFeature(SASL), m_session(NULL)
{
	m_depth = 0;
	if (!sasl_tried_to_init) {
		sasl_tried_to_init = true;
#ifdef USE_GSASL
		int error = gsasl_init(&sasl_context);
		if (error != GSASL_OK) {
			sasl_context = NULL;
			jreenWarning() << "Cannot initialize libgsasl:" << error << ": " << gsasl_strerror (error);
			return;
		} else {
			sasl_inited_successfully = true;
			gsasl_callback_set(sasl_context, callback_function);
			qAddPostRoutine(destroy_sasl_context);
		}
#else
		int error = sasl_client_init(sasl_callbacks);
		if (error == SASL_OK) {
			sasl_inited_successfully = true;
			qAddPostRoutine(sasl_done);
		} else {
			jreenWarning() << "Cannot initialize sasl2:" << error << ": " << sasl_errstring(error, NULL, NULL);
		}
#endif
	}
}

#ifndef USE_GSASL
void SASLFeature::getParameters()
{
	const int *maybe_sff;
	int result_ssf = 0;
	if (SASL_OK == sasl_getprop(m_session, SASL_SSF, reinterpret_cast<const void **>(&maybe_sff)))
		result_ssf = *maybe_sff;

	const int *maybe_maxoutbuf;
	int maxoutbuf = 0;
	if (SASL_OK == sasl_getprop(m_session, SASL_MAXOUTBUF, reinterpret_cast<const void **>(&maybe_maxoutbuf)))
		maxoutbuf = *maybe_maxoutbuf;

	jreenWarning() << "getParameters" << result_ssf << maybe_sff << maxoutbuf << maybe_maxoutbuf;
}

void SASLFeature::interactWithInfo(sasl_interact_t *interacts)
{
	for (size_t i = 0; interacts[i].id != SASL_CB_LIST_END; ++i) {
		sasl_interact *interact = &interacts[i];
		switch (interact->id) {
		case SASL_CB_AUTHNAME:
			setInteractionResult(interact, m_info->jid().node().toUtf8());
			break;
		case SASL_CB_GETREALM:
			setInteractionResult(interact, m_info->jid().domain().toUtf8());
			break;
		case SASL_CB_USER:
//			setInteractionResult(interact, m_info->jid().bare().toUtf8());
			break;
		case SASL_CB_PASS:
			setInteractionResult(interact, m_info->password().toUtf8());
			break;
		}
	}
}

void SASLFeature::setInteractionResult(sasl_interact_t *interact, const QByteArray &value)
{
	m_interact << value;
	interact->result = value.constData();
}

#endif

void SASLFeature::reset()
{
	if (!sasl_inited_successfully)
		return;
	m_depth = 0;
	m_mechs.clear();
#ifdef USE_GSASL
	m_session.reset();
#else
	sasl_dispose(&m_session);
	m_interact.clear();
#endif
}

bool SASLFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	if (!sasl_inited_successfully)
		return false;
	Q_UNUSED(name);
	Q_UNUSED(attributes);
	return uri == NS_SASL;
}

void SASLFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_ASSERT(sasl_inited_successfully);
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
	Q_ASSERT(sasl_inited_successfully);
	Q_UNUSED(uri);
	if (m_depth == 2 && m_state == AtMechanism)
		m_state = AtMechanisms;
	else if (m_depth == 1) {
		m_state = AtStart;
		if (name == QLatin1String("success"))
			m_info->completed(StreamInfo::Authorized | StreamInfo::ResendHeader);
		if (name == QLatin1String("failure"))
			m_info->completed(StreamInfo::AuthorizationFailed);
	}
	m_depth--;
}

void SASLFeature::handleCharacterData(const QStringRef &text)
{
	Q_ASSERT(sasl_inited_successfully);
	if (m_state == AtMechanism) {
		m_mechs.append(text.toString());
	} else if (m_state == AtChallenge) {
#ifdef USE_GSASL
		char *result;
		int error = gsasl_step64(m_session.data(), text.toString().toLatin1(), &result);
		if (error == GSASL_NEEDS_MORE || error == GSASL_OK) {
			QXmlStreamWriter *writer = m_info->writer();
			writer->writeStartElement(QLatin1String("response"));
			writer->writeDefaultNamespace(NS_SASL);
			writer->writeCharacters(QString::fromLatin1(result));
			writer->writeEndElement();
			gsasl_free(result);
		} else {
			jreenWarning() << "SASL next step:" << error << ":" << gsasl_strerror(error);
			m_info->completed(StreamInfo::AuthorizationFailed);
			m_session.reset();
		}
#else
		QByteArray input = QByteArray::fromBase64(text.toLatin1());
		sasl_interact_t *interacts = NULL;
		const char *result;
		unsigned result_length;
		int error;

		do {

			error = sasl_client_step(m_session,
									 input,
									 input.size(),
									 &interacts,
									 &result,
									 &result_length);

			if (error == SASL_INTERACT) {
				interactWithInfo(interacts);
			}


		} while (error == SASL_INTERACT);

		if (error != SASL_CONTINUE && error != SASL_OK) {
			jreenWarning() << "Cannot do step:" << error << ":" << sasl_errstring(error, NULL, NULL);
			sasl_dispose(&m_session);
			m_info->completed(StreamInfo::AuthorizationFailed);
			return;
		}

		getParameters();

		QXmlStreamWriter *writer = m_info->writer();
		writer->writeStartElement(QLatin1String("response"));
		writer->writeDefaultNamespace(NS_SASL);
		writer->writeCharacters(QString::fromLatin1(
									QByteArray::fromRawData(result, result_length)
									.toBase64()));
		writer->writeEndElement();
#endif
	}
}

bool SASLFeature::isActivatable()
{
	return sasl_inited_successfully && !m_mechs.isEmpty();
}

bool SASLFeature::activate()
{
	if (!sasl_inited_successfully)
		return false;

#ifdef USE_GSASL
	Gsasl_session *session;
	QByteArray mechs = m_mechs.join(QLatin1String(":")).toLatin1();
	const char *mech = gsasl_client_suggest_mechanism(sasl_context, mechs.data());
	int error = gsasl_client_start(sasl_context, mech, &session);
	if (error != GSASL_OK) {
		jreenWarning() << "SASL Cannot initialize client:" << error << ":" << gsasl_strerror(error);
		return false;
	}
	m_session.reset(session);
	gsasl_session_hook_set(session, m_info);

	char *result;
	error = gsasl_step64(session, "", &result);
	if (error == GSASL_NEEDS_MORE || error == GSASL_OK) {
		QXmlStreamWriter *writer = m_info->writer();
		writer->writeStartElement(QLatin1String("auth"));
		writer->writeDefaultNamespace(NS_SASL);
		writer->writeAttribute(QLatin1String("mechanism"), QString::fromUtf8(mech));
		if (result && *result)
			writer->writeCharacters(QString::fromLatin1(result));
		writer->writeEndElement();
		gsasl_free(result);
	} else {
		jreenWarning() << "SASL activate:" << error << ":" << gsasl_strerror(error);
		m_info->completed(StreamInfo::AuthorizationFailed);
		m_session.reset();
		return false;
	}
#else
	int error = sasl_client_new("xmpp",
								QUrl::toAce(m_info->jid().domain()),
								NULL, NULL, // local and remote ip
								NULL, // session-specific callbacks
								0, // security flags
								&m_session);

	if (error != SASL_OK) {
		jreenWarning() << "Cannot initialize client:" << error
				 << ":" << sasl_errstring(error, NULL, NULL);
		return false;
	}

	QByteArray mechlist = m_mechs.join(QLatin1String(" ")).toLatin1();
	sasl_interact_t *interacts = NULL;
	const char *result, *mechusing;
	unsigned result_length;

	do {

		error = sasl_client_start(m_session,
								 mechlist,
								 &interacts,
								 &result,
								 &result_length,
								 &mechusing);

		if (error == SASL_INTERACT) {
			interactWithInfo(interacts);
		}


	} while (error == SASL_INTERACT);

	if (error != SASL_CONTINUE) {
		jreenWarning() << "Cannot start client:" << error << ":" << sasl_errstring(error, NULL, NULL);
		sasl_dispose(&m_session);
		m_info->completed(StreamInfo::AuthorizationFailed);
		return false;
	}
	getParameters();

	QXmlStreamWriter *writer = m_info->writer();
	writer->writeStartElement(QLatin1String("auth"));
	writer->writeDefaultNamespace(NS_SASL);
	writer->writeAttribute(QLatin1String("mechanism"), QString::fromUtf8(mechusing));
	writer->writeCharacters(QString::fromLatin1(
								QByteArray::fromRawData(result, result_length)
								.toBase64()));
	writer->writeEndElement();
#endif
	return true;
}
}
