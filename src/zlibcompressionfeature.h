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

#ifndef ZLIBCOMPRESSIONFEATURE_H
#define ZLIBCOMPRESSIONFEATURE_H

#include "streamfeature.h"
#include <QStringList>

namespace Jreen
{
	class ZLibCompressionFeature : public StreamFeature
	{
	public:
		ZLibCompressionFeature();
		int priority() { return 10; }
		void reset();
		bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleEndElement(const QStringRef &name, const QStringRef &uri);
		void handleCharacterData(const QStringRef &text);
		bool isActivatable();
		bool activate();
	private:
		enum State
		{
			AtMethods,
			AtMethod,
			AtStart
		};
		State m_state;
		int m_depth;
		QStringList m_methods;
	};
}

#endif // ZLIBCOMPRESSIONFEATURE_H
