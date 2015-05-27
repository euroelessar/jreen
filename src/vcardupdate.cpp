/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

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
