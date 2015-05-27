/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "softwareversion.h"

namespace Jreen
{

class SoftwareVersionPrivate
{
public:
	QString name;
	QString version;
	QString os;
};

SoftwareVersion::SoftwareVersion()
    : d_ptr(new SoftwareVersionPrivate)
{
}

SoftwareVersion::SoftwareVersion(const QString &name, const QString &version, const QString &os)
    : d_ptr(new SoftwareVersionPrivate)
{
	Q_D(SoftwareVersion);
	d->name = name;
	d->version = version;
	d->os = os;
}

SoftwareVersion::~SoftwareVersion()
{
}

QString SoftwareVersion::name() const
{
	return d_func()->name; 
}

QString SoftwareVersion::version() const
{
	return d_func()->version;
}

QString SoftwareVersion::os() const
{
	return d_func()->os;
}

} //namespace Jreen

