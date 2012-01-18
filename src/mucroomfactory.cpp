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

#include "mucroomfactory_p.h"
#include "util.h"
#include "jstrings.h"
#include <QXmlStreamWriter>

#define NS_MUC QLatin1String("http://jabber.org/protocol/muc")
#define NS_MUCUSER QLatin1String("http://jabber.org/protocol/muc#user")
#define NS_MUCADMIN QLatin1String("http://jabber.org/protocol/muc#admin")
#define NS_MUCOWNER QLatin1String("http://jabber.org/protocol/muc#owner")

namespace Jreen
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

void MUCRoomQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
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

Payload::Ptr MUCRoomQueryFactory::createPayload()
{
	return Payload::Ptr(new MUCRoomQuery(QString()));
}

static const char *mucroom_affiliations[] = {
	"none", "outcast", "member", "owner", "admin"
};

static const char *mucroom_roles[] = {
	"none", "visitor", "participant", "moderator"
};

MUCRoomItemFactory::MUCRoomItemFactory()
{
	m_depth = 0;
	m_state = AtNowhere;
}

MUCRoomItemFactory::~MUCRoomItemFactory()
{
}

bool MUCRoomItemFactory::canParse(const QStringRef &name, const QStringRef &uri,
								  const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("item");
}

void MUCRoomItemFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
											const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		m_item.reset(new MUCRoomItem);
		m_item->jid = attributes.value(QLatin1String("jid")).toString();
		m_item->nick = attributes.value(QLatin1String("nick")).toString();
		QStringRef tmp = attributes.value(QLatin1String("affiliation"));
		m_item->affiliation = strToEnum<MUCRoom::Affiliation>(tmp, mucroom_affiliations);
		tmp = attributes.value(QLatin1String("role"));
		m_item->role = strToEnum<MUCRoom::Role>(tmp, mucroom_roles);
	}  else if (m_depth == 2) {
		if (name == QLatin1String("actor"))
			m_item->actor = attributes.value(QLatin1String("jid")).toString();
		else if (name == QLatin1String("reason"))
			m_state = AtReason;
	}
}

void MUCRoomItemFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	if (m_depth == 2 && m_state == AtReason)
		m_state = AtNowhere;
	m_depth--;
}

void MUCRoomItemFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtReason)
		m_item->reason = text.toString();
}

void MUCRoomItemFactory::serialize(MUCRoomItem *item, QXmlStreamWriter *writer)
{
	writer->writeStartElement(QLatin1String("item"));
	if (item->jid.isValid())
		writer->writeAttribute(QLatin1String("jid"), item->jid);
	if (!item->nick.isEmpty())
		writer->writeAttribute(QLatin1String("nick"), item->nick);
	if (item->affiliation != MUCRoom::AffiliationInvalid)
		writer->writeAttribute(QLatin1String("affiliation"), enumToStr(item->affiliation, mucroom_affiliations));
	if (item->role != MUCRoom::RoleInvalid)
		writer->writeAttribute(QLatin1String("role"), enumToStr(item->role, mucroom_roles));
	if (item->actor.isValid()) {
		writer->writeEmptyElement(QLatin1String("actor"));
		writer->writeAttribute(QLatin1String("jid"), item->actor);
	}
	if (!item->reason.isEmpty())
		writer->writeTextElement(QLatin1String("reason"), item->reason);
	writer->writeEndElement();
}

void MUCRoomItemFactory::result(MUCRoomItem *item)
{
	*item = *m_item;
	m_item.reset(0);
}

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
	m_depth++;
	if (m_depth == 1) {
		m_query.reset(new MUCRoomUserQuery);
	} else if (m_depth == 2) {
		if (m_item.canParse(name, uri, attributes)) {
			m_state = AtItem;
		} else if (name == QLatin1String("status")) {
			QStringRef code = attributes.value(QLatin1String("code"));
			int codeInt = QString::fromRawData(code.data(), code.size()).toInt();
			m_query->flags |= userQueryCodeToFlag(codeInt);
		} else if (name == QLatin1String("invite")) {
			m_state = AtOperation;
			m_query->operation = MUCRoomUserQuery::Invite;
			m_query->item.jid = attributes.value(QLatin1String("from")).toString();
		} else if (name == QLatin1String("decline")) {
			m_state = AtOperation;
			m_query->operation = MUCRoomUserQuery::Decline;
			m_query->item.jid = attributes.value(QLatin1String("from")).toString();
		} else if (name == QLatin1String("password")) {
			m_state = AtPassword;
		}
	} else if (m_depth == 3 && m_state == AtOperation) {
		if (name == QLatin1String("reason"))
			m_state = AtReason;
		else if (name == QLatin1String("continue"))
			m_query->thread = attributes.value(QLatin1String("thread")).toString();
	}
	if (m_state == AtItem)
		m_item.handleStartElement(name, uri, attributes);
}

void MUCRoomUserQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtItem) {
		m_item.handleEndElement(name, uri);
		if (m_depth == 2) {
			m_item.result(&m_query->item);
			m_state = AtNowhere;
		}
	} else if (m_depth == 2) {
		m_state = AtNowhere;
	} else if (m_depth == 3 && m_state == AtReason) {
		m_state = AtOperation;
	}
	m_depth--;
}

void MUCRoomUserQueryFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtItem)
		m_item.handleCharacterData(text);
	else if (m_state == AtReason)
		m_query->item.reason = text.toString();
	else if (m_state == AtPassword)
		m_query->password = text.toString();
}

void MUCRoomUserQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	MUCRoomUserQuery *query = se_cast<MUCRoomUserQuery*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_MUCUSER);
	if (query->operation > 0) {
		if (query->operation == MUCRoomUserQuery::Invite)
			writer->writeStartElement(QLatin1String("invite"));
		else if (query->operation == MUCRoomUserQuery::Decline)
			writer->writeStartElement(QLatin1String("decline"));
		else
			Q_ASSERT(!"Unknown operation");
		writer->writeAttribute(QLatin1String("to"), query->item.jid);
		writer->writeTextElement(QLatin1String("reason"), query->item.reason);
		if (!query->thread.isEmpty()) {
			writer->writeEmptyElement(QLatin1String("continue"));
			writer->writeAttribute(QLatin1String("thread"), query->thread);
		}
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

Payload::Ptr MUCRoomUserQueryFactory::createPayload()
{
	return Payload::Ptr(m_query.take());
}

MUCRoomAdminQueryFactory::MUCRoomAdminQueryFactory()
{
	m_depth = 0;
	m_state = AtNowhere;
}

MUCRoomAdminQueryFactory::~MUCRoomAdminQueryFactory()
{
}

QStringList MUCRoomAdminQueryFactory::features() const
{
	return QStringList(NS_MUCADMIN);
}

bool MUCRoomAdminQueryFactory::canParse(const QStringRef &name, const QStringRef &uri,
										const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_MUCADMIN;
}

void MUCRoomAdminQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
												  const QXmlStreamAttributes &attributes)
{
	m_depth++;
	if (m_depth == 1)
		m_query.reset(new MUCRoomAdminQuery);
	else if (m_depth == 2 && m_item.canParse(name, uri, attributes))
		m_state = AtItem;
	if (m_state == AtItem)
		m_item.handleStartElement(name, uri, attributes);
}

void MUCRoomAdminQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtItem) {
		m_item.handleEndElement(name, uri);
		if (m_depth == 2) {
			MUCRoomItem item;
			m_item.result(&item);
			m_query->items.append(item);
			m_state = AtNowhere;
		}
	}
	m_depth--;
}

void MUCRoomAdminQueryFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtItem)
		m_item.handleCharacterData(text);
}

void MUCRoomAdminQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	MUCRoomAdminQuery *query = se_cast<MUCRoomAdminQuery*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_MUCADMIN);
	for (int i = 0; i < query->items.size(); i++)
		m_item.serialize(&query->items[i], writer);
	writer->writeEndElement();
}

Payload::Ptr MUCRoomAdminQueryFactory::createPayload()
{
	return Payload::Ptr(m_query.take());
}

MUCRoomOwnerQueryFactory::MUCRoomOwnerQueryFactory()
{
	m_depth = 0;
	m_state = AtNowhere;
}

MUCRoomOwnerQueryFactory::~MUCRoomOwnerQueryFactory()
{
}

QStringList MUCRoomOwnerQueryFactory::features() const
{
	return QStringList(NS_MUCOWNER);
}

bool MUCRoomOwnerQueryFactory::canParse(const QStringRef &name, const QStringRef &uri,
										const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_MUCOWNER;
}

void MUCRoomOwnerQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
												  const QXmlStreamAttributes &attributes)
{
	m_depth++;
	if (m_depth == 1)
		m_query.reset(new MUCRoomOwnerQuery);
	else if (m_depth == 2 && m_form.canParse(name, uri, attributes))
		m_state = AtForm;
	if (m_state == AtForm)
		m_form.handleStartElement(name, uri, attributes);
}

void MUCRoomOwnerQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtForm) {
		m_form.handleEndElement(name, uri);
		if (m_depth == 2) {
			m_query->form = m_form.createPayload().staticCast<DataForm>();
			m_state = AtNowhere;
		}
	}
	m_depth--;
}

void MUCRoomOwnerQueryFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtForm)
		m_form.handleCharacterData(text);
}

void MUCRoomOwnerQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	MUCRoomOwnerQuery *query = se_cast<MUCRoomOwnerQuery*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_MUCOWNER);
	if (query->form)
		m_form.serialize(query->form.data(), writer);
	writer->writeEndElement();
}

Payload::Ptr MUCRoomOwnerQueryFactory::createPayload()
{
	return Payload::Ptr(m_query.take());
}
}
