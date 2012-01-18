/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef BOOKMARK_H
#define BOOKMARK_H
#include "stanzaextension.h"
#include "jid.h"

namespace Jreen {

//XEP-0048
//http://xmpp.org/extensions/xep-0048.html
class BookmarkPrivate;
/*! For ease-of-use in a Jabber client, it is desirable to have a way to store shortcuts
* to various services and resources (such as conference rooms and web pages) as "bookmarks"
* that can be displayed in the user's client.
* Several Jabber clients have already agreed on and implemented a method to provide this service;
* that informal agreement is documented and expanded upon in this document
*/
class JREEN_EXPORT Bookmark : public Payload
{
	Q_DECLARE_PRIVATE(Bookmark)
	J_PAYLOAD(Jreen::Bookmark) //dummy
public:
	class ConferencePrivate;
	class JREEN_EXPORT Conference
	{
	public:
		Conference();
		Conference(const QString &name, const JID &jid, const QString &nick,
				   const QString &password = QString(), bool autojoin = false);
		Conference(const Conference &o);
		Conference &operator =(const Conference &o);
		~Conference();
		
		bool operator ==(const Conference &o);
		bool operator !=(const Conference &o);
		bool isValid() const;
		
		void setJid(const JID &jid);
		void setName(const QString &name);
		/*! set the user's preferred roomnick for the chatroom
		  */
		void setNick(const QString &nick);
		/*! set an unencrypted string for the password needed to enter a password-protected room.
		 *  \note For security reasons, use of this element is NOT RECOMMENDED.
		 */
		void setPassword(const QString &password);
		/*! set autojoin attribute
		  */
		void setAutojoin(bool set);
		/*! returns friendly name of bookmark
		  */
		QString name() const;
		/*! returns friendly name of bookmark
		  */
		QString nick() const;
		/*! returns the user's preferred roomnick for the chatroom
		  */
		QString password() const;
		/*! returns JabberID of the chat room.
		  */
		JID jid() const;
		/*! returns autojoin attribute
		  */
		bool autojoin() const;
	private:
		QExplicitlySharedDataPointer<ConferencePrivate> d_ptr;
	};
	
	/*! Default constructor \param jid - The JabberID of the chat room
	  * \param name - A friendly name for the bookmark(optional)
	  * \param nick - The user's preferred roomnick for the chatroom(optional).
	  */
	Bookmark();
	/*! set friendly name of bookmark
	  */
	~Bookmark();
	
	QList<Conference> conferences() const;
	void addConference(const Conference &conf);
	void setConferences(const QList<Conference> &conferences);
private:
	QScopedPointer<BookmarkPrivate> d_ptr;
};

} // namespace Jreen

#endif // BOOKMARK_H
