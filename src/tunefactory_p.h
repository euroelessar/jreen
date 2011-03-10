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

#ifndef TUNEFACTORY_P_H
#define TUNEFACTORY_P_H

#include "tune.h"
#include <QVector>

namespace Jreen
{
	class TuneFactory : public StanzaExtensionFactory<Tune>
	{
	public:
		TuneFactory();
		QStringList features() const;
		bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleEndElement(const QStringRef &name, const QStringRef &uri);
		void handleCharacterData(const QStringRef &text);
		void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
		StanzaExtension::Ptr createExtension();
	private:
		int m_depth;
		int m_state;
		QVector<QString> m_data;
	};
}

#endif // TUNEFACTORY_P_H
