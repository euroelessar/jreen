/****************************************************************************
 *  disco.h
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

#ifndef DISCO_H
#define DISCO_H

#include <QObject>
#include <QString>
#include <QSet>
#include "client.h"
#include "dataform.h"
#include "jid.h"
#include "stanzaextension.h"

namespace jreen
{

class IQ;
struct DiscoPrivate;

class JREEN_EXPORT Disco : public QObject
{
	Q_DISABLE_COPY(Disco)
	Q_OBJECT
	Q_DECLARE_PRIVATE(Disco)
public:
	struct Identity
	{
		inline Identity() {}
		inline Identity(const QString &category, const QString &type, const QString &name, const QString &lang = QString())
			: category(category), type(type), name(name), lang(lang) {}
		QString category;
		QString type;
		QString name;
		QString lang;
	};
	typedef QList<Identity> IdentityList;

	class Info : public StanzaExtension
	{
		J_EXTENSION(jreen::Disco::Info,"/iq/query[@xmlns='http://jabber.org/protocol/disco#info']")
		public:
			inline Info(const QString &node = QString(), const IdentityList &identities = IdentityList(), const QSet<QString> &features = QSet<QString>(), QSharedPointer<DataForm> form = QSharedPointer<DataForm>())
		  : m_node(node), m_identities(identities), m_features(features), m_form(form) {}
		Info(const QDomElement &node);
		QDomElement node(QDomDocument *document) const;
		inline const QString &node() const { return m_node; }
		inline const IdentityList &identities() const { return m_identities; }
		inline const QSet<QString> &features() const { return m_features; }
		inline const QSharedPointer<DataForm> form() const { return m_form; }
	private:
		QString m_node;
		IdentityList m_identities;
		QSet<QString> m_features;
		QSharedPointer<DataForm> m_form;
	};

	struct Item
	{
		inline Item() {}
		inline Item(const JID &jid, const QString &node, const QString &name)
			: jid(jid), node(node), name(name) {}
		JID jid;
		QString node;
		QString name;
	};
	typedef QList<Item> ItemList;

	class Items : public StanzaExtension
	{
		J_EXTENSION(jreen::Disco::Items,"/iq/query[@xmlns='http://jabber.org/protocol/disco#items']")
		public:
			inline Items(const QString &node = QString(), const ItemList &items = ItemList())
		  : m_items(items), m_node(node) {}
		Items(const QDomElement &node);
		QDomElement node(QDomDocument *document) const;
		inline const QString &node() const { return m_node; }
		inline const ItemList &items() const { return m_items; }
		inline ItemList &items() { return m_items; }
	private:
		ItemList m_items;
		QString m_node;
	};

	Disco(Client *client);
	virtual ~Disco();
	const IdentityList &identities() const;
	IdentityList &identities();
	const QSet<QString> &features() const;
	QSet<QString> &features();
	void setSoftwareVersion(const QString &name, const QString &version, const QString &os = QString());
	const DataForm *form() const;
	void setForm(DataForm *form);

public slots:
private slots:
	void handleIQ(const jreen::IQ &iq);
protected:
	QScopedPointer<DiscoPrivate> d_ptr;
};

}

#endif // DISCO_H
