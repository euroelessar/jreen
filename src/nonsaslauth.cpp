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

#include <QCryptographicHash>
#include <QXmlStreamAttributes>
#include "nonsaslauth_p.h"
#include "iq_p.h"
#include "client.h"
#include "jstrings.h"

#define NS_IQ_AUTH QLatin1String("http://jabber.org/features/iq-auth")

namespace Jreen
{

bool NonSaslAuth::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("auth") && uri == NS_IQ_AUTH;
}

void NonSaslAuth::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
}

void NonSaslAuth::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void NonSaslAuth::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

bool NonSaslAuth::isActivatable()
{
	return false;
}

bool NonSaslAuth::activate()
{
	return false;
}
		
NonSaslAuth::Query::Query() : m_is_digest(false)
{
}

NonSaslAuth::Query *NonSaslAuth::Query::instance(const JID &jid, const QString &password, const QString &sid) const
{
	Query *q = new Query();
	if(m_is_digest) {
		QCryptographicHash hash(QCryptographicHash::Sha1);
		hash.addData(sid.toUtf8());
		hash.addData(password.toUtf8());
		q->m_password = QString::fromLatin1(hash.result().toHex());
	}
	else {
		q->m_password = password;
	}
	q->m_is_digest = m_is_digest;
	q->m_username = jid.node();
	q->m_resource = jid.resource();
	return q;
}

NonSaslAuth::NonSaslAuth() : StreamFeature(SimpleAuthorization)
{
	reset();
}

void NonSaslAuth::setStreamInfo(StreamInfo *info)
{
	StreamFeature::setStreamInfo(info);
}

void NonSaslAuth::reset()
{
	m_current_step = RequestFields;
}

void NonSaslAuth::handleIq(const IQ &iq, int context)
{
	switch(context) {
	case RequestFields:{
		iq.accept();
		const Query *query = iq.payload<Query>().data();
		ConnectionIQ iq(IQ::Set, m_client->jid().domain());
		iq.addExtension(query->instance(m_client->jid(), m_info->password(), m_info->streamID()));
		m_client->send(iq, this, SLOT(handleIq(IQ,int)), ProvideInformation);
		break;}
	case ProvideInformation:
		iq.accept();
		m_info->completed(StreamInfo::Authorized);
		break;
	}
}

}
