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
	
	bool MUCRoomQueryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
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
	
	
	
	
	MUCRoomUserQueryFactory::MUCRoomUserQueryFactory()
	{
	}
	
	MUCRoomUserQueryFactory::~MUCRoomUserQueryFactory()
	{
	}
	
	QStringList MUCRoomUserQueryFactory::features() const
	{
		return QStringList(NS_MUCUSER);
	}
	
	bool MUCRoomUserQueryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
		return name == QLatin1String("x") && uri == NS_MUCUSER;
	}
	
	void MUCRoomUserQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
	 const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
	}
	
	void MUCRoomUserQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
	}
	
	void MUCRoomUserQueryFactory::handleCharacterData(const QStringRef &text)
	{
		Q_UNUSED(text);
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
		return StanzaExtension::Ptr(new MUCRoomUserQuery());
	}
}
