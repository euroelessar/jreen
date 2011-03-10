/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef IQ_P_H
#define IQ_P_H

#include "iq.h"
#include "stanza_p.h"

namespace Jreen
{
class IQPrivate : public StanzaPrivate
{
public:
	IQPrivate() : StanzaPrivate(StanzaIq), accepted(false) {}
	IQ::Type subtype;
	mutable bool accepted;
};
}

#endif // IQ_P_H
