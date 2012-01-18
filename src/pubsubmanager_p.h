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
			QList<AbstractPayloadFactory *> factories;
		};
		
		class Publish : public Payload
		{
			J_PAYLOAD(Jreen::PubSub::Publish)
		public:
			Publish() {}
			Publish(const QList<Payload::Ptr> &i, const DataForm::Ptr &f) : items(i), form(f) {}
			QList<Payload::Ptr> items;
			QString node;
			DataForm::Ptr form;
		};
	}
}
#endif // PUBSUBMANAGER_P_H
