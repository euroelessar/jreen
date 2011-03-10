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

#include "bookmarkstorage.h"
#include "pubsubmanager.h"
#include "privatexml.h"
#include <QPointer>
#include <QDebug>

namespace Jreen
{
class BookmarkStoragePrivate
{
public:
	Client *client;
	QPointer<PubSub::Manager> pubSubManager;
	QPointer<PrivateXml> privateXml;
};

BookmarkStorage::BookmarkStorage(Client *client) : d_ptr(new BookmarkStoragePrivate)
{
	Q_D(BookmarkStorage);
	d->client = client;
}

BookmarkStorage::~BookmarkStorage()
{
}

void BookmarkStorage::setPubSubManager(PubSub::Manager *manager)
{
	d_func()->pubSubManager = manager;
}

void BookmarkStorage::setPrivateXml(PrivateXml *privateXml)
{
	d_func()->privateXml = privateXml;
}

void BookmarkStorage::requestBookmarks()
{
	Q_D(BookmarkStorage);
	if (!d->privateXml)
		return;
	d->privateXml->request(QLatin1String("storage"), QLatin1String("storage:bookmarks"), this,
						   SLOT(onResultReady(Jreen::StanzaExtension::Ptr,Jreen::PrivateXml::Result,Jreen::Error::Ptr)));
}

void BookmarkStorage::storeBookmarks(const Bookmark::Ptr &bookmarks)
{
	Q_D(BookmarkStorage);
	if (!d->privateXml)
		return;
	d->privateXml->store(bookmarks, this,
						 SLOT(onResultReady(Jreen::StanzaExtension::Ptr,Jreen::PrivateXml::Result,Jreen::Error::Ptr)));
}

void BookmarkStorage::onResultReady(const StanzaExtension::Ptr &node,
									PrivateXml::Result result, const Error::Ptr &error)
{
	Q_UNUSED(error);
	qDebug() << "onResultReady";
	if(result == PrivateXml::RequestOk) {
		Bookmark *bookmark = se_cast<Bookmark*>(node.data());
		qDebug() << "received bookmarks" << bookmark << node.data();
		if (bookmark) {
			qDebug("%s %p %d", Q_FUNC_INFO, bookmark, bookmark->conferences().size());
			emit bookmarksReceived(node.staticCast<Bookmark>());
		}
		else
			emit bookmarksReceived(Bookmark::Ptr::create());
	}
}
}
