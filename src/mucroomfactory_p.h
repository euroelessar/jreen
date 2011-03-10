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

#ifndef MUCROOMFACTORY_P_H
#define MUCROOMFACTORY_P_H

#include "mucroom_p.h"
#include "dataformfactory_p.h"

namespace Jreen
{
class MUCRoomQueryFactory : public StanzaExtensionFactory<MUCRoomQuery>
{
public:
	MUCRoomQueryFactory();
	virtual ~MUCRoomQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
};

class MUCRoomItemFactory : public XmlStreamFactory<MUCRoomItem>
{
public:
	MUCRoomItemFactory();
	virtual ~MUCRoomItemFactory();
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(MUCRoomItem *item, QXmlStreamWriter *writer);
	void result(MUCRoomItem *item);
private:
	enum State { AtNowhere, AtActor, AtReason } m_state;
	int m_depth;
	QScopedPointer<MUCRoomItem> m_item;
};

class MUCRoomUserQueryFactory : public StanzaExtensionFactory<MUCRoomUserQuery>
{
public:
	MUCRoomUserQueryFactory();
	virtual ~MUCRoomUserQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	MUCRoomItemFactory m_item;
	QScopedPointer<MUCRoomUserQuery> m_query;
	int m_depth;
	enum State { AtNowhere, AtItem, AtActor, AtReason };
	State m_state;
};

class MUCRoomAdminQueryFactory : public StanzaExtensionFactory<MUCRoomAdminQuery>
{
public:
	MUCRoomAdminQueryFactory();
	virtual ~MUCRoomAdminQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	enum State { AtNowhere, AtItem } m_state;
	int m_depth;
	MUCRoomItemFactory m_item;
	QScopedPointer<MUCRoomAdminQuery> m_query;
};

class MUCRoomOwnerQueryFactory : public StanzaExtensionFactory<MUCRoomOwnerQuery>
{
public:
	MUCRoomOwnerQueryFactory();
	virtual ~MUCRoomOwnerQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	enum State { AtNowhere, AtForm } m_state;
	int m_depth;
	DataFormFactory m_form;
	QScopedPointer<MUCRoomOwnerQuery> m_query;
};
}
#endif // MUCROOMFACTORY_P_H
