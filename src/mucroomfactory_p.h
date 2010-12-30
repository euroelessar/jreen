/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef MUCROOMFACTORY_P_H
#define MUCROOMFACTORY_P_H

#include "mucroom_p.h"

namespace jreen
{
	class MUCRoomQueryFactory : public StanzaExtensionFactory<MUCRoomQuery>
	{
	public:
		MUCRoomQueryFactory();
		virtual ~MUCRoomQueryFactory();
		QStringList features() const;
		bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleEndElement(const QStringRef &name, const QStringRef &uri);
		void handleCharacterData(const QStringRef &text);
		void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
		StanzaExtension::Ptr createExtension();
	};
	
	class MUCRoomUserQueryFactory : public StanzaExtensionFactory<MUCRoomUserQuery>
	{
	public:
		MUCRoomUserQueryFactory();
		virtual ~MUCRoomUserQueryFactory();
		QStringList features() const;
		bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleEndElement(const QStringRef &name, const QStringRef &uri);
		void handleCharacterData(const QStringRef &text);
		void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
		StanzaExtension::Ptr createExtension();
	};
}
#endif // MUCROOMFACTORY_P_H
