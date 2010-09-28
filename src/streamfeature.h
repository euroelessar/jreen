/****************************************************************************
 *  streamfeature.h
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

#ifndef STREAMFEATURE_H
#define STREAMFEATURE_H

#include <QObject>
#include <QDomElement>
#include "jid.h"

namespace jreen
{

class DataStream;
class Client;

class JREEN_EXPORT StreamInfo
{
public:
	virtual QString streamID() = 0;
	virtual QString connectionServer() = 0;
	virtual JID jid() = 0;
	virtual QString password() = 0;
	virtual Client *client() = 0;
	virtual void completed() = 0;
	virtual void addDataStream(DataStream *data_stream) = 0;
};

class JREEN_EXPORT StreamFeature : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StreamFeature)
public:
	enum Type
	{
		SimpleAuthorization,
		SASL,
		CompressionLayer,
		SecurityLayer,
		Invalid
	};
	StreamFeature(Type type) : m_info(0), m_client(0), m_type(type) {}
	virtual ~StreamFeature() {}
	virtual int priority() = 0;
	virtual void setStreamInfo(StreamInfo *info) { if(info) m_client = (m_info = info)->client(); else { m_info = 0; m_client = 0; } }
	virtual void reset() {}
	virtual void processElement(const QDomElement &node) = 0;
	virtual const QString &xPath() const = 0;
	inline Type type() const { return m_type; }
protected:
	StreamInfo *m_info;
	Client *m_client;
	inline void completed() { if(m_info) m_info->completed(); }
	inline void addDataStream(DataStream *data_stream) { if(m_info) m_info->addDataStream(data_stream); }
private:
	const Type m_type;
};

}

Q_DECLARE_INTERFACE(jreen::StreamFeature,"org.qutim.JReen.StreamFeature");

#endif // STREAMFEATURE_H
