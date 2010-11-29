/****************************************************************************
 *  activityfactory.cpp
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

#include "activityfactory_p.h"

namespace jreen {

static const char* general_types[]= {
	"doing_chores",
	"drinking",
	"eating",
	"exercising",
	"grooming",
	"having_appointment",
	"inactive",
	"relaxing",
	"talking",
	"traveling",
	"undefined",
	"working"
};

ActivityFactory::ActivityFactory()
{
}

} // namespace jreen
