/****************************************************************************
 *  message.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "stanza.h"

namespace Jreen
{

class MessagePrivate;
class DelayedDelivery;

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
	Message(Type type, const JID& to, const QString &body = QString(), const QString &subject = QString(),
			const QString &thread = QString(), const QString &xmllang = QString());
	Message(MessagePrivate &p);
	inline Message &operator =(const Message &stanza)
	{ return *static_cast<Message *>(&Jreen::Stanza::operator =(stanza)); }
	Type subtype() const;
	const QString &body(const QString &lang = QString()) const;
	void setBody(const QString &text, const QString &lang = QString());
	const QString &subject(const QString &lang = QString()) const;
	void setSubject(const QString &text, const QString &lang = QString());
	const QString &thread() const;
	void setThread(const QString &thread);
	void setID(const QString &id);
	const DelayedDelivery *when() const;
};

}

#endif // MESSAGE_H
