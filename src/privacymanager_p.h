/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef PRIVACYMANAGER_P_H
#define PRIVACYMANAGER_P_H

#include "privacymanager.h"
#include "stanzaextension.h"
#include <QVariant>
#include <QSet>
#include <QStringList>

namespace Jreen
{
class PrivacyItemPrivate : public QSharedData
{
public:
	PrivacyItemPrivate() :
			type(PrivacyItem::All), types(PrivacyItem::AllStanzas),
			action(PrivacyItem::Deny), order(0) {}
	PrivacyItemPrivate(const PrivacyItemPrivate &o) :
			QSharedData(o), type(o.type), types(o.types),
			action(o.action), data(o.data), order(o.order) {}
	PrivacyItem::Type type;
	PrivacyItem::StanzaTypes types;
	PrivacyItem::Action action;
	QVariant data;
	uint order;
};

class PrivacyManagerPrivate
{
public:
	Client *client;
	QString defaultList;
	QString activeList;
	QStringList lists;
	QHash<QString, QString> activeListSetter;
	QHash<QString, QString> defaultListSetter;
	QSet<QString> listRequests;
	QString lastListName;
	QList<PrivacyItem> lastList;
	bool validServer;
};

class PrivacyQuery : public StanzaExtension
{
	J_EXTENSION(Jreen::PrivacyQuery, "")
public:
	class List
	{
	public:
		List(const QString &n, const QList<PrivacyItem> &i = QList<PrivacyItem>()) : name(n), items(i) {}
		QString name;
		QList<PrivacyItem> items;
	};

	QString defaultList;
	QString activeList;
	QList<List> lists;
};
}

#endif // PRIVACYMANAGER_P_H
