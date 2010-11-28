/****************************************************************************
 *  vcard.cpp
 *
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

#include "vcard.h"

namespace jreen {

void VCard::setName(const QString &family,
					const QString &given,
					const QString &middle,
					const QString &prefix,
					const QString &suffix)
{
	m_name.family = family;
	m_name.given = given;
	m_name.middle = middle;
	m_name.prefix = prefix;
	m_name.suffix = suffix;
}

void VCard::setPhoto(const QString &extval)
{
	m_photo.extval = extval;
	m_photo.binval.clear();
	m_photo.type.clear();
}

void VCard::setPhoto(const Photo &photo)
{
	m_photo = photo;
}

} // namespace jreen
