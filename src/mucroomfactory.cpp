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

#include "mucroomfactory_p.h"
#include "util.h"
#include "jstrings.h"
#include <QXmlStreamWriter>

#define NS_MUC QLatin1String("http://jabber.org/protocol/muc")
#define NS_MUCUSER QLatin1String("http://jabber.org/protocol/muc#user")

namespace jreen
{
	MUCRoomQueryFactory::MUCRoomQueryFactory()
	{
	}
	
	MUCRoomQueryFactory::~MUCRoomQueryFactory()
	{
	}
	
	QStringList MUCRoomQueryFactory::features() const
	{
		return QStringList(NS_MUC);
	}
	
	bool MUCRoomQueryFactory::canParse(const QStringRef &name, const QStringRef &uri,
									   const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
		return name == QLatin1String("x") && uri == NS_MUC;
	}
	
	void MUCRoomQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
												 const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
	}
	
	void MUCRoomQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
	}
	
	void MUCRoomQueryFactory::handleCharacterData(const QStringRef &text)
	{
		Q_UNUSED(text);
	}
	
	void MUCRoomQueryFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
	{
		MUCRoomQuery *query = se_cast<MUCRoomQuery*>(extension);
		writer->writeStartElement(QLatin1String("x"));
		writer->writeDefaultNamespace(NS_MUC);
		if (!query->m_password.isNull())
			writer->writeTextElement(QLatin1String("password"), query->m_password);
		if (query->m_maxChars != -1 || query->m_maxStanzas != -1
			|| query->m_seconds != -1 || query->m_since.isValid()) {
			writer->writeStartElement(QLatin1String("history"));
			if (query->m_maxChars != -1)
				writer->writeAttribute(QLatin1String("maxchars"), QString::number(query->m_maxChars));
			if (query->m_maxStanzas != -1)
				writer->writeAttribute(QLatin1String("maxstanzas"), QString::number(query->m_maxStanzas));
			if (query->m_seconds != -1)
				writer->writeAttribute(QLatin1String("seconds"), QString::number(query->m_seconds));
			if (query->m_since.isValid())
				writer->writeAttribute(QLatin1String("since"), Util::toStamp(query->m_since));
			writer->writeEndElement();
		}
		writer->writeEndElement();
	}
	
	StanzaExtension::Ptr MUCRoomQueryFactory::createExtension()
	{
		return StanzaExtension::Ptr(new MUCRoomQuery(QString()));
	}
	
	static const char *mucroom_affiliations[] = {
		"none", "outcast", "member", "owner", "admin"
	};
	
	static const char *mucroom_roles[] = {
		"none", "visitor", "participant", "moderator"
	};
	
	MUCRoomUserQueryFactory::MUCRoomUserQueryFactory()
	{
		m_depth = 0;
		m_state = AtNowhere;
	}
	
	MUCRoomUserQueryFactory::~MUCRoomUserQueryFactory()
	{
	}
	
	QStringList MUCRoomUserQueryFactory::features() const
	{
		return QStringList(NS_MUCUSER);
	}
	
	bool MUCRoomUserQueryFactory::canParse(const QStringRef &name, const QStringRef &uri,
										   const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
		return name == QLatin1String("x") && uri == NS_MUCUSER;
	}
	
	int userQueryCodeToFlag(int code)
	{
		switch (code) {
		case 100:
		case 172:
			return MUCRoomUserQuery::NonAnonymous;
		case 101:
			return MUCRoomUserQuery::AffiliationChangeWNR;
		case 110:
			return MUCRoomUserQuery::Self;
		case 170:
			return MUCRoomUserQuery::LoggingEnabled;
		case 171:
			return MUCRoomUserQuery::LoggingDisabled;
		case 173:
			return MUCRoomUserQuery::SemiAnonymous;
		case 174:
			return MUCRoomUserQuery::FullyAnonymous;
		case 201:
			return MUCRoomUserQuery::NewRoom;
		case 210:
			return MUCRoomUserQuery::NickAssigned;
		case 301:
			return MUCRoomUserQuery::Banned;
		case 303:
			return MUCRoomUserQuery::NickChanged;
		case 307:
			return MUCRoomUserQuery::Kicked;
		case 321:
			return MUCRoomUserQuery::AffiliationChanged;
		case 322:
			return MUCRoomUserQuery::MembershipRequired;
		case 332:
			return MUCRoomUserQuery::RoomSegfaulted;
		default:
			return 0;
		}
	}
	
	void MUCRoomUserQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
													 const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		m_depth++;
		if (m_depth == 1) {
			m_query.reset(new MUCRoomUserQuery);
		} else if (m_depth == 2) {
			if (name == QLatin1String("item")) {
				m_query->jid = attributes.value(QLatin1String("jid")).toString();
				m_query->nick = attributes.value(QLatin1String("nick")).toString();
				m_query->affiliation = strToEnum<MUCRoom::Affiliation>(attributes.value(QLatin1String("affiliation")), mucroom_affiliations);
				m_query->role = strToEnum<MUCRoom::Role>(attributes.value(QLatin1String("role")), mucroom_roles);
				m_state = AtItem;
			} else if (name == QLatin1String("status")) {
				QStringRef code = attributes.value(QLatin1String("code"));
				int codeInt = QString::fromRawData(code.data(), code.size()).toInt(); 
				m_query->flags |= userQueryCodeToFlag(codeInt);
			}
		} else if (m_depth == 3) {
			if (m_state == AtItem && name == QLatin1String("actor"))
				m_query->actor = attributes.value(QLatin1String("jid")).toString();
			else if (m_state == AtItem && name == QLatin1String("reason"))
				m_state = AtReason;
		}
	}
	
	void MUCRoomUserQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
		if (m_depth == 3 && m_state == AtReason)
			m_state = AtItem;
		else if (m_depth == 2 && m_state == AtItem)
			m_state = AtNowhere;
		m_depth--;
	}
	
	void MUCRoomUserQueryFactory::handleCharacterData(const QStringRef &text)
	{
		if (m_state == AtReason)
			m_query->reason = text.toString();
	}
	
	void MUCRoomUserQueryFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
	{
		Q_UNUSED(extension);
		writer->writeStartElement(QLatin1String("x"));
		writer->writeDefaultNamespace(NS_MUCUSER);
		writer->writeEndElement();
	}
	
	StanzaExtension::Ptr MUCRoomUserQueryFactory::createExtension()
	{
		return StanzaExtension::Ptr(m_query.take());
	}
}
