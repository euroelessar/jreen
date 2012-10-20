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

#include "bookmarkstorage.h"
#include "pubsubmanager.h"
#include "privatexml.h"
#include <QPointer>
#include "logger.h"

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
						   SLOT(onResultReady(Jreen::Payload::Ptr,Jreen::PrivateXml::Result,Jreen::Error::Ptr)));
}

void BookmarkStorage::storeBookmarks(const Bookmark::Ptr &bookmarks)
{
	Q_D(BookmarkStorage);
	if (!d->privateXml)
		return;
	d->privateXml->store(bookmarks, this,
						 SLOT(onResultReady(Jreen::Payload::Ptr,Jreen::PrivateXml::Result,Jreen::Error::Ptr)));
}

void BookmarkStorage::onResultReady(const Payload::Ptr &node,
									PrivateXml::Result result, const Error::Ptr &error)
{
	Q_UNUSED(error);
	Logger::debug() << "onResultReady";
	if(result == PrivateXml::RequestOk) {
		Bookmark *bookmark = se_cast<Bookmark*>(node.data());
		Logger::debug() << "received bookmarks" << bookmark << node.data();
		if (bookmark)
			emit bookmarksReceived(node.staticCast<Bookmark>());
		else
			emit bookmarksReceived(Bookmark::Ptr::create());
	}
}
}
