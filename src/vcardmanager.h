/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_VCARDMANAGER_H
#define JREEN_VCARDMANAGER_H

#include "vcard.h"
#include "vcardupdate.h"
#include "error.h"

namespace Jreen
{
class JID;
class IQ;
class Presence;
class IQReply;
class VCardReply;
class VCardReplyPrivate;
class VCardManagerPrivate;

class JREEN_EXPORT VCardManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VCardManager)
public:
	VCardManager(Client *client);
	virtual ~VCardManager();
	
	VCardReply *fetch(const JID &jid);
	VCardReply *store(const Jreen::VCard::Ptr &vcard);
	
signals:
	void vCardFetched(const Jreen::VCard::Ptr &vcard, const Jreen::JID &jid);
	void vCardUpdateDetected(const Jreen::JID &jid, const Jreen::VCardUpdate::Ptr &update);
	
private:
	void notifyReplyDeath(const JID &jid);
	friend class VCardReply;
	Q_PRIVATE_SLOT(d_func(), void _q_received(const Jreen::Presence &))
	QScopedPointer<VCardManagerPrivate> d_ptr;
};

class JREEN_EXPORT VCardReply : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VCardReply)
public:
	virtual ~VCardReply();

	JID jid() const;
	VCard::Ptr vcard() const;
	Error::Ptr error() const;
	
signals:
	void vCardFetched(const Jreen::VCard::Ptr &vcard, const Jreen::JID &jid);
	void error(const Jreen::Error::Ptr &error);
	void finished();

private:
	VCardReply(const JID &jid, VCardManager *manager, IQReply *reply);
	friend class VCardManager;
	Q_PRIVATE_SLOT(d_func(), void _q_received(const Jreen::IQ &))
	QScopedPointer<VCardReplyPrivate> d_ptr;
};
}

#endif // JREEN_VCARDMANAGER_H
