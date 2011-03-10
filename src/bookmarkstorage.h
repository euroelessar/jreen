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

#ifndef BOOKMARKSTORAGE_H
#define BOOKMARKSTORAGE_H

#include "bookmark.h"
#include "privatexml.h"
#include "error.h"

namespace Jreen
{
class PrivateXml;
namespace PubSub { class Manager; }

class BookmarkStoragePrivate;
class JREEN_EXPORT BookmarkStorage : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(BookmarkStorage)
public:
	BookmarkStorage(Client *client);
	~BookmarkStorage();

	void setPubSubManager(PubSub::Manager *manager);
	void setPrivateXml(PrivateXml *privateXml);

	void requestBookmarks();
	void storeBookmarks(const Bookmark::Ptr &bookmarks);
signals:
	void bookmarksReceived(const Jreen::Bookmark::Ptr &bookrmark);
private slots:
	void onResultReady(const Jreen::StanzaExtension::Ptr &,Jreen::PrivateXml::Result,const Jreen::Error::Ptr &);
private:
	QScopedPointer<BookmarkStoragePrivate> d_ptr;
};
}

#endif // BOOKMARKSTORAGE_H
