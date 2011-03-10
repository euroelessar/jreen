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

#ifndef SESSIONFEATURE_P_H
#define SESSIONFEATURE_P_H

#include "streamfeature.h"

namespace Jreen
{
	class IQ;
	
	class SessionFeature : public QObject, public StreamFeature
	{
		Q_OBJECT
	public:
		SessionFeature();
		int priority() { return 9; }
		void setStreamInfo(StreamInfo *info);
		void reset();
		bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleEndElement(const QStringRef &name, const QStringRef &uri);
		void handleCharacterData(const QStringRef &text);
		bool isActivatable();
		bool activate();
	public slots:
		void onIQResult(const Jreen::IQ &iq, int context);
	private:
		int m_depth;
		bool m_hasFeature;
	};
}

#endif // SESSIONFEATURE_P_H
