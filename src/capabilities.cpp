/****************************************************************************
 *  capabilities.cpp
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

#include <QStringList>
#include <QMap>
#include <QStringBuilder>
#include <QXmlStreamWriter>
#include "capabilities.h"
#include "disco.h"
#include "jstrings.h"

namespace jreen
{

Capabilities::Capabilities(const QString &ver) :
	m_ver(ver),
	m_node(QLatin1String("http://jreen.org/"))
{
}

}
