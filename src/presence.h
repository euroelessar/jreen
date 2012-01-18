/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PRESENCE_H
#define PRESENCE_H

#include "stanza.h"

namespace Jreen
{

class PresencePrivate;
class Capabilities;
class LangMap;

class JREEN_EXPORT Presence : public Stanza
{
	Q_DECLARE_PRIVATE(Presence)
public:
	/**
  * Describes the different valid presence types.
  */
	enum Type
	{
		Invalid		= -1,	/**< Invalid type */
		Unavailable = 0,	/**< The entity is offline. */
		Available,			/**< The entity is online. */
		Subscribe,			/**< Subscribe request. */
		Unsubscribe,		/**< The sender is unsubscribing from another entity's presence.. */
		Subscribed,			/**< The sender has allowed the recipient to receive their presence.. */
		Unsubscribed,		/**< The subscription request has been denied or a previously-granted subscription has been cancelled. */
		Probe,				/**< A request for an entity's current presence; SHOULD be generated only by a server on behalf of a user. */
		Error,				/**< An error has occurred regarding processing or delivery of a previously-sent presence stanza. */
		Empty,				/**< Do not send show state*/
		Chat,				/**< The entity is 'available for chat'. */
		Away,				/**< The entity is away. */
		DND,				/**< The entity is DND (Do Not Disturb). */
		XA					/**< The entity is XA (eXtended Away). */
	};

	Presence(Type type, const JID& to,
			 const QString &status = QString(), int priority = 0,
			 const QString &xmllang = QString());
	Presence(Type type, const JID& to,const LangMap &status,
			 int priority = 0);
	Presence(PresencePrivate &p);
	inline Presence &operator =(const Presence &stanza)
	{ return *static_cast<Presence *>(&Jreen::Stanza::operator =(stanza)); }
	Type subtype() const;
	void setSubtype(Type subtype);
	bool isSubscription() const;
	const Capabilities *capabilities() const;
	const QString &status(const QString &lang = QString()) const;
	void addStatus(const QString &status, const QString &lang = QString());
	void resetStatus();
	int priority() const;
	void setPriority(int priority);
	virtual ~Presence() {};
};

}

#endif // PRESENCE_H
