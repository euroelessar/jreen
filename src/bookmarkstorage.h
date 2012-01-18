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
	void onResultReady(const Jreen::Payload::Ptr &,Jreen::PrivateXml::Result,const Jreen::Error::Ptr &);
private:
	QScopedPointer<BookmarkStoragePrivate> d_ptr;
};
}

#endif // BOOKMARKSTORAGE_H
