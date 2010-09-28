/****************************************************************************
 *  util.h
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

#ifndef UTIL_H
#define UTIL_H

#include "jreen.h"
#include <QDateTime>

J_BEGIN_NAMESPACE

class JID;

namespace Util
{
	JREEN_EXPORT QDateTime fromStamp( const QString &stamp );
	JREEN_EXPORT QString toStamp( const QDateTime &date_time );
	JREEN_EXPORT QString randomHash( const JID &jid );
	inline int log2( register uint n )
	{
		register int pos = 0;
		while( n > 1 ) { ++pos; n >>= 1; }
		return ( n == 0 ) ? (-1) : pos;
	}
}

J_END_NAMESPACE

#endif // UTIL_H
