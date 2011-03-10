/****************************************************************************
 *  bookmarkfactory.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef BOOKMARKFACTORY_H
#define BOOKMARKFACTORY_H
#include "bookmark.h"

namespace Jreen {

class BookmarkFactory : public StanzaExtensionFactory<Bookmark>
{
public:
	BookmarkFactory();
	virtual ~BookmarkFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	enum State { AtNick, AtPassword, AtNowhere, AtConference };
	State m_state;
	int m_depth;
	QScopedPointer<Bookmark> m_bookmark;
	Bookmark::Conference m_conference;
};

} // namespace Jreen

#endif // BOOKMARKFACTORY_H
