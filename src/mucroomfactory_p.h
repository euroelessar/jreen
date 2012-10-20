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

#ifndef MUCROOMFACTORY_P_H
#define MUCROOMFACTORY_P_H

#include "mucroom_p.h"
#include "dataformfactory_p.h"

namespace Jreen
{
class JREEN_AUTOTEST_EXPORT MUCRoomQueryFactory : public PayloadFactory<MUCRoomQuery>
{
public:
	MUCRoomQueryFactory();
	virtual ~MUCRoomQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
};

class JREEN_AUTOTEST_EXPORT MUCRoomItemFactory : public XmlStreamFactory<MUCRoomItem>
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

class JREEN_AUTOTEST_EXPORT MUCRoomUserQueryFactory : public PayloadFactory<MUCRoomUserQuery>
{
public:
	MUCRoomUserQueryFactory();
	virtual ~MUCRoomUserQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	MUCRoomItemFactory m_item;
	QScopedPointer<MUCRoomUserQuery> m_query;
	int m_depth;
	enum State { AtNowhere, AtItem, AtActor, AtReason, AtPassword, AtOperation };
	State m_state;
};

class JREEN_AUTOTEST_EXPORT MUCRoomAdminQueryFactory : public PayloadFactory<MUCRoomAdminQuery>
{
public:
	MUCRoomAdminQueryFactory();
	virtual ~MUCRoomAdminQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	enum State { AtNowhere, AtItem } m_state;
	int m_depth;
	MUCRoomItemFactory m_item;
	QScopedPointer<MUCRoomAdminQuery> m_query;
};

class JREEN_AUTOTEST_EXPORT MUCRoomOwnerQueryFactory : public PayloadFactory<MUCRoomOwnerQuery>
{
public:
	MUCRoomOwnerQueryFactory();
	virtual ~MUCRoomOwnerQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	enum State { AtNowhere, AtForm } m_state;
	int m_depth;
	DataFormFactory m_form;
	QScopedPointer<MUCRoomOwnerQuery> m_query;
};
}
#endif // MUCROOMFACTORY_P_H
