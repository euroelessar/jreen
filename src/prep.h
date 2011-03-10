/****************************************************************************
 *  prep.h
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
