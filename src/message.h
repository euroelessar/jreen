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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "stanza.h"
#include "delayeddelivery.h"

namespace Jreen
{

class MessagePrivate;

class JREEN_EXPORT Message : public Stanza
{
	Q_DECLARE_PRIVATE(Message)
public:
	/**
  * Describes the different valid message types.
  */
	enum Type
	{
		Chat = 0,     /**< A chat message. */
		Error,        /**< An error message. */
		Groupchat,    /**< A groupchat message. */
		Headline,     /**< A headline message. */
		Normal,       /**< A normal message. */
		Invalid	      /**< The message stanza is invalid. */
	};
	Message(Type type = Invalid);
	Message(Type type, const JID& to, const QString &body = QString(), const QString &subject = QString(),
			const QString &thread = QString(), const QString &xmllang = QString());
	Message(MessagePrivate &p);
	inline Message &operator =(const Message &stanza)
	{ return *static_cast<Message *>(&Jreen::Stanza::operator =(stanza)); }
	Type subtype() const;
	QString body(const QString &lang = QString()) const;
	void setBody(const QString &text, const QString &lang = QString());
	QString subject(const QString &lang = QString()) const;
	void setSubject(const QString &text, const QString &lang = QString());
	QString thread() const;
	void setThread(const QString &thread);
	void setID(const QString &id);
	DelayedDelivery::Ptr when() const;
};

}

#endif // MESSAGE_H
