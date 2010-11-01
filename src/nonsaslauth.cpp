/****************************************************************************
 *  nonsaslauth.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <QCryptographicHash>
#include "nonsaslauth.h"
#include "iq.h"
#include "client.h"
#include "jstrings.h"

namespace jreen
{

J_STRING(auth)
		
bool NonSaslAuth::canHandle(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("auth") && uri == QLatin1String("http://jabber.org/features/iq-auth");
}

void NonSaslAuth::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
}

void NonSaslAuth::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
}

void NonSaslAuth::handleCharacterData(const QStringRef &name)
{
}

bool NonSaslAuth::isActivatable()
{
}

bool NonSaslAuth::activate()
{
}
		
NonSaslAuth::Query::Query(const QDomElement &node) : m_is_digest(false)
{
	forelements(const QDomElement &elem, node)
		if(elem.nodeName() == QLatin1String("digest"))
		{
			m_is_digest = true;
			return;
		}
}

QDomElement NonSaslAuth::Query::node(QDomDocument *document) const
{
	QDomElement node = document->createElement(ConstString::query);
	node.setAttribute(ConstString::xmlns, "jabber:iq:auth");
	if(!m_username.isEmpty())
		node.appendChild(createElement(document, "username", m_username));
	if(!m_password.isEmpty())
	{
		if(m_is_digest)
			node.appendChild(createElement(document, "digest",   m_password));
		else
			node.appendChild(createElement(document, "password", m_password));
	}
	if(!m_resource.isEmpty())
		node.appendChild(createElement(document, "resource", m_resource));
	return node;
}

NonSaslAuth::Query *NonSaslAuth::Query::instance(const JID &jid, const QString &password, const QString &sid) const
{
	Query *q = new Query();
	if(m_is_digest)
	{
		QCryptographicHash hash(QCryptographicHash::Sha1);
		hash.addData(sid.toUtf8());
		hash.addData(password.toUtf8());
		q->m_password = QString::fromLatin1(hash.result().toHex());
	}
	else
	{
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
	if(m_client)
		m_client->registerStanzaExtension(new Query);
}

void NonSaslAuth::reset()
{
	m_current_step = RequestFields;
}

//void NonSaslAuth::processElement(const QDomElement &node)
//{
//	if(m_current_step == Completed)
//		return;
//	Q_UNUSED(node);
//	switch(m_current_step)
//	{
//	case RequestFields:{
//		IQ iq(IQ::Get, m_client->jid().domain());
//		iq.addExtension(new Query);
//		m_client->send(iq, this, SLOT(handleIq(IQ,int)), RequestFields);
//		m_current_step = ProvideInformation;
//		break;}
//	case ProvideInformation:
//		m_current_step = WaitingForResults;
//		break;
//	case WaitingForResults:
//		break;
//	default:
//		break;
//	}
//}

void NonSaslAuth::handleIq(const IQ &iq, int context)
{
	switch(context)
	{
	case RequestFields:{
		iq.accept();
		const Query *query = iq.findExtension<Query>().data();
		IQ iq(IQ::Set, m_client->jid().domain());
		iq.addExtension(query->instance(m_client->jid(), m_info->password(), m_info->streamID()));
		m_client->send(iq, this, SLOT(handleIq(IQ,int)), ProvideInformation);
		break;}
	case ProvideInformation:
		iq.accept();
		completed();
		break;
	}
}

}
