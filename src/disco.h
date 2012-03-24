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

#ifndef DISCO_H
#define DISCO_H

#include <QObject>
#include <QString>
#include <QSet>
#include "dataform.h"
#include "jid.h"
#include "stanzaextension.h"
#include "iq.h"

namespace Jreen
{

class IQ;
class IQReply;
class Client;
class DiscoPrivate;
class DiscoReply;
class DiscoReplyPrivate;

class JREEN_EXPORT Disco : public QObject
{
	Q_DISABLE_COPY(Disco)
	Q_OBJECT
	Q_DECLARE_PRIVATE(Disco)
public:
	class IdentityData;
	class JREEN_EXPORT Identity
	{
	public:
		Identity();
		Identity(const QString &category, const QString &type, const QString &name, const QString &lang = QString());
		Identity(const Identity &item);
		Identity &operator =(const Identity &item);
		~Identity();
		
		QString category() const;
		QString type() const;
		QString name() const;
		QString lang() const;
		
	private:
		QSharedDataPointer<IdentityData> d;
	};
	typedef QList<Identity> IdentityList;
	
	class InfoPrivate;
	class JREEN_EXPORT Info : public Payload
	{
		Q_DECLARE_PRIVATE(Info)
		J_PAYLOAD(Jreen::Disco::Info)
	public:
		Info(const QString &node = QString(), const IdentityList &identities = IdentityList(),
		        const QSet<QString> &features = QSet<QString>(),
		        QSharedPointer<DataForm> form = QSharedPointer<DataForm>());
		~Info();

		QString node() const;
		IdentityList identities() const;
		QSet<QString> features() const;
		DataForm::Ptr form() const;
		
	private:
		QScopedPointer<InfoPrivate> d_ptr;
	};

	class ItemData;
	class JREEN_EXPORT Item
	{
	public:
		enum Action
		{
			ActionExecute   = 0x01,
			ActionRegister  = 0x02,
			ActionJoin      = 0x04,
			ActionSearch    = 0x08,
			ActionAdd       = 0x10,
			ActionVCard     = 0x20,
			ActionProxy     = 0x40,
			ActionExpand    = 0x80
		};
		typedef QFlags<Action> Actions;
		
		Item();
		Item(const JID &jid, const QString &node, const QString &name);
		Item(const Item &item);
		Item &operator =(const Item &item);
		~Item();

		JID jid() const;
		void setJid(const JID &jid);
		QString node() const;
		void setNode(const QString &node);
		QString name() const;
		void setName(const QString &name);
		IdentityList identities() const;
		bool hasIdentity(const QString &category, const QString &type = QString()) const;
		void addIdentity(const Identity &identity);
		void setIdentities(const IdentityList &identities);
		QSet<QString> features() const;
		void setFeatures(const QSet<QString> &features);
		DataForm::Ptr form() const;
		void setForm(const DataForm::Ptr &form);
		Actions actions() const;

	private:
		QSharedDataPointer<ItemData> d;
	};
	typedef QList<Item> ItemList;
	
	class ItemsPrivate;
	class JREEN_EXPORT Items : public Payload
	{
		Q_DECLARE_PRIVATE(Items)
		J_PAYLOAD(Jreen::Disco::Items)
	public:
		Items(const QString &node = QString(), const ItemList &items = ItemList());
		~Items();
		
		QString node() const;
		ItemList items() const;

	private:
		QScopedPointer<ItemsPrivate> d_ptr;
	};

	Disco(Client *client);
	virtual ~Disco();
	
	DiscoReply *requestInfo(const Item &item);
	DiscoReply *requestItems(const Item &item);
	
	void addIdentity(const Identity &identity);
	void addIdentity(const QString &category, const QString &type, const QString &name, const QString &lang = QString());
	const IdentityList &identities() const;
	IdentityList &identities();
	const QSet<QString> &features() const;
	QSet<QString> &features();
	void addFeature(const QString &feature);
	void setSoftwareVersion(const QString &name, const QString &version, const QString &os = QString());
	void setSoftwareVersion(const QString &name, const QString &version, const QString &os, const QString &osVersion);
	const DataForm *form() const;
	void setForm(DataForm *form);

protected:
	Q_PRIVATE_SLOT(d_func(), void _q_received(const Jreen::IQ &))
	QScopedPointer<DiscoPrivate> d_ptr;
};

class JREEN_EXPORT DiscoReply : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(DiscoReply)
public:
	virtual ~DiscoReply();
	
	Disco::Item item() const;
	Disco::ItemList subitems() const;
	QString errorText() const;
	Jreen::Error::Ptr error() const;
	
signals:
	void finished();
	void error(const Jreen::Error::Ptr &error);
	void infoReceived(const Jreen::Disco::Item &item);
	void itemsReceived(const Jreen::Disco::ItemList &items);
	
private:
	DiscoReply(const Disco::Item &item, Jreen::IQReply *reply);
	Q_PRIVATE_SLOT(d_func(), void _q_received(const Jreen::IQ &))
	friend class Disco;
	QScopedPointer<DiscoReplyPrivate> d_ptr;
};

}

Q_DECLARE_METATYPE(Jreen::Disco::Identity)
Q_DECLARE_METATYPE(Jreen::Disco::IdentityList)
Q_DECLARE_METATYPE(Jreen::Disco::Item)
Q_DECLARE_METATYPE(Jreen::Disco::ItemList)
Q_DECLARE_OPERATORS_FOR_FLAGS(Jreen::Disco::Item::Actions)

#endif // DISCO_H
