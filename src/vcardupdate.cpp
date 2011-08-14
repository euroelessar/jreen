/****************************************************************************
 *  vcardupdate.cpp
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

#include "vcardupdate.h"

namespace Jreen {

class VCardUpdatePrivate
{
public:
	VCardUpdatePrivate() : hasPhotoInfo(false) {}
	QString photoHash;
	bool hasPhotoInfo;
};

VCardUpdate::VCardUpdate(const QString &photoHash)
    : d_ptr(new VCardUpdatePrivate)
{
	Q_D(VCardUpdate);
	d->photoHash = photoHash;
	d->hasPhotoInfo = true;
}

VCardUpdate::VCardUpdate()
    : d_ptr(new VCardUpdatePrivate)
{
	Q_D(VCardUpdate);
	d->hasPhotoInfo = false;
}

VCardUpdate::~VCardUpdate()
{
}

QString VCardUpdate::photoHash() const
{
	return d_func()->photoHash;
}

void VCardUpdate::setPhotoHash(const QString &photoHash)
{
	Q_D(VCardUpdate);
	d->photoHash = photoHash;
	d->hasPhotoInfo = true;
}

void VCardUpdate::clear()
{
	Q_D(VCardUpdate);
	d->photoHash.clear();
	d->hasPhotoInfo = false;
}

bool VCardUpdate::hasPhotoInfo() const
{
	return d_func()->hasPhotoInfo;
}

} // namespace Jreen
