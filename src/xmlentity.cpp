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

#include "xmlentity.h"
#include "stanzaextension.h"

namespace Jreen
{
	XmlEntity::XmlEntity()
	{
	}
	
	XmlEntity::~XmlEntity()
	{
	}
	
	int XmlEntity::registerXmlEntityType(const char *type)
	{
		return StanzaExtension::registerExtensionType(type);
	}
}	
