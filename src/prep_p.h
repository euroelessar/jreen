/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef PREP_H
#define PREP_H

#include "jreen.h"

namespace Jreen
{

class JREEN_EXPORT Prep
{
	J_SINGLETON(Prep)
public:
	QString nodePrep(const QString &node, bool *ok = 0);
	QString namePrep(const QString &domain, bool *ok = 0);
	QString resourcePrep(const QString &resource, bool *ok = 0);
};

}

#endif // PREP_H
