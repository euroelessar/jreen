/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef PRESENCE_P_H
#define PRESENCE_P_H

#include "stanza_p.h"
#include "presence.h"
#include "langmap.h"

namespace Jreen
{
class PresencePrivate : public StanzaPrivate
{
public:
	PresencePrivate() : StanzaPrivate(StanzaPresence) {}
	Presence::Type subtype;
	LangMap status;
	int priority;
};
}

#endif // PRESENCE_P_H
