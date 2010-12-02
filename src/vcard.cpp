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

#include "vcard_p.h"

namespace jreen {

VCard::VCard(const QString &formattedName, Classification classification)
	: d_ptr(new VCardPrivate)
{
	Q_D(VCard);
	d->formattedName = formattedName;
	d->classification = classification;
}

VCard::VCard(VCardPrivate &p) : d_ptr(&p)
{
}

VCard::~VCard()
{
}

void VCard::setFormattedName(const QString &fn)
{
	d_func()->formattedName = fn;
}

const QString &VCard::formattedName() const
{
	return d_func()->formattedName;
}

void VCard::setName(const Name &name)
{
	d_func()->name = name;
}

const VCard::Name& VCard::name() const
{
	return d_func()->name;
}

void VCard::setNickname(const QString& nickname)
{
	d_func()->nickname = nickname;
}

const QString& VCard::nickname() const
{
	return d_func()->nickname;
}

void VCard::setName(const QString &family,
					const QString &given,
					const QString &middle,
					const QString &prefix,
					const QString &suffix)
{
	Q_D(VCard);
	d->name.family = family;
	d->name.given = given;
	d->name.middle = middle;
	d->name.prefix = prefix;
	d->name.suffix = suffix;
}

void VCard::setPhoto(const QString &extval)
{
	Q_D(VCard);
	d->photo.extval = extval;
	d->photo.binval.clear();
	d->photo.type.clear();
}

void VCard::setPhoto(const Photo &photo)
{
	d_func()->photo = photo;
}

const VCard::Photo& VCard::photo() const
{
	return d_func()->photo;
}

void VCard::setBday(const QDateTime& bday)
{
	d_func()->bday = bday;
}

const QDateTime& VCard::bday() const
{
	return d_func()->bday;
}

} // namespace jreen
