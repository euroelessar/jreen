/****************************************************************************
 *  presence.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef PRESENCE_H
#define PRESENCE_H

#include "stanza.h"

J_BEGIN_NAMESPACE

struct PresencePrivate;
class Capabilities;

class JREEN_EXPORT Presence : public Stanza
{
	J_DECLARE_PRIVATE(Presence)
public:
	/**
	 * Describes the different valid presence types.
	 */
	enum Type
	{
		Available,                  /**< The entity is online. */
		Chat,                       /**< The entity is 'available for chat'. */
		Away,                       /**< The entity is away. */
		DND,                        /**< The entity is DND (Do Not Disturb). */
		XA,                         /**< The entity is XA (eXtended Away). */
		Unavailable,                /**< The entity is offline. */
		Probe,                      /**< This is a presence probe. */
		Error,                      /**< This is a presence error. */
		Invalid,                    /**< The stanza is invalid. */
		Online = Available,
		Offline = Unavailable
	};
	Presence( Type type, const JID& to, const QString &status = QString(),
			  int priority = 0, const QString &xmllang = QString() );
	Presence( const QDomElement &node );
	inline Presence &operator =( const Presence &stanza )
		{ return *static_cast<Presence *>( &J_NAMESPACE::Stanza::operator =( stanza ) ); }
	Type subtype() const;
	const Capabilities *capabilities() const;
	Type presence() const;
	void setPresence( Type type );
	const QString &status( const QString &lang = QString() ) const;
	void addStatus( const QString &status, const QString &lang = QString() );
	void resetStatus();
	int priority() const;
	void setPriority( int priority );
	QDomElement node() const;
};

J_END_NAMESPACE

#endif // PRESENCE_H
