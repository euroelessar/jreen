/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef PUBSUBMANAGER_P_H
#define PUBSUBMANAGER_P_H

#include "pubsubmanager.h"
#include "dataform.h"

namespace Jreen
{
	namespace PubSub
	{
		class ManagerPrivate
		{
		public:
			Client *client;
			QList<AbstractStanzaExtensionFactory *> factories;
		};
		
		class Publish : public StanzaExtension
		{
			J_EXTENSION(Jreen::PubSub::Publish, "")
		public:
			Publish() {}
			Publish(const QList<StanzaExtension::Ptr> &i, const DataForm::Ptr &f) : items(i), form(f) {}
			QList<StanzaExtension::Ptr> items;
			QString node;
			DataForm::Ptr form;
		};
	}
}
#endif // PUBSUBMANAGER_P_H
