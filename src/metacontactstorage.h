/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JREEN_METACONTACTSTORAGE_H
#define JREEN_METACONTACTSTORAGE_H

#include "jid.h"
#include "privatexml.h"
#include "error.h"

namespace Jreen
{

class MetaContactStoragePrivate;

class JREEN_EXPORT MetaContactStorage : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(MetaContactStorage)
public:
	class ItemData;
	class JREEN_EXPORT Item
	{
	public:
		Item();
		Item(const JID &jid, const QString &tag);
		Item(const JID &jid, const QString &tag, uint order);
		Item(const Item &item);
		Item &operator =(const Item &item);
		~Item();

		JID jid() const;
		void setJID(const JID &jid);
		QString tag() const;
		void setTag(const QString &tag);
		uint order() const;
		bool hasOrder() const;
		void clearOrder();
		void setOrder(uint order);

	private:
		QSharedDataPointer<ItemData> d;
	};
	typedef QList<Item> ItemList;
	
	MetaContactStorage(Client *client);
	~MetaContactStorage();

	void setPrivateXml(PrivateXml *privateXml);
	
	void requestMetaContacts();
	void storeMetaContacts(const ItemList &items);

signals:
	void metaContactsReceived(const Jreen::MetaContactStorage::ItemList &items);

private slots:
	void onResultReady(const Jreen::Payload::Ptr &,Jreen::PrivateXml::Result,const Jreen::Error::Ptr &);

private:
	QScopedPointer<MetaContactStoragePrivate> d_ptr;
};

} // namespace Jreen

#endif // JREEN_METACONTACTSTORAGE_H
