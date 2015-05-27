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

#ifndef JREEN_PREP_P_H
#define JREEN_PREP_P_H

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

#endif // JREEN_PREP_P_H
