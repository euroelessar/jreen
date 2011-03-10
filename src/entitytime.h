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

#ifndef ENTITYTIME_H
#define ENTITYTIME_H

#include "stanzaextension.h"
#include <QDateTime>

namespace Jreen
{
	class EntityTimePrivate;
	class EntityTime : public StanzaExtension
	{
		J_EXTENSION(Jreen::EntityTime, "")
		Q_DECLARE_PRIVATE(EntityTime)
	public:
		EntityTime(int tzo, const QDateTime &dateTime);
		EntityTime(const QDateTime &dateTime);
		EntityTime();
		~EntityTime();
		
		int timeZoneOffset() const;
		QDateTime localTime() const;
		QDateTime universalTime() const;
	private:
		QScopedPointer<EntityTimePrivate> d_ptr;
	};
}

#endif // ENTITYTIME_H
