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

#ifndef STREAMPROCESSOR_H
#define STREAMPROCESSOR_H

#include "jreen.h"
#include <QObject>

namespace Jreen
{
class StreamInfo;
class JID;
class JREEN_EXPORT StreamProcessor
{
public:
    StreamProcessor();
	virtual ~StreamProcessor();

	virtual void authorized() = 0;
	virtual QString sessionID() const = 0;
	virtual void setJID(const JID &jid) = 0;
	virtual void setStreamParser(XmlStreamParser *parser) = 0;
	virtual void restartStream() = 0;
};
}

Q_DECLARE_INTERFACE(Jreen::StreamProcessor, "org.qutim.Jreen.StreamProcessor")

#endif // STREAMPROCESSOR_H
