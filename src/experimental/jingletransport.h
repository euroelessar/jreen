/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JINGLETRANSPORT_H
#define JINGLETRANSPORT_H

#include "jinglemanager.h"
#include "../stanzaextension.h"
#include <QSharedPointer>

namespace Jreen
{

class JingleTransportPrivate;
class JingleContent;
typedef Payload JingleTransportInfo;

class JingleTransport : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JingleTransport)
	Q_PROPERTY(Jreen::JingleTransport::State state READ state NOTIFY stateChanged)
public:
	enum State {
		Disconnected,
		Gathering,
		Connecting,
		Connected,
		Failed
	};
	typedef QSharedPointer<JingleTransport> Ptr;
	
    JingleTransport(JingleContent *content);
	virtual ~JingleTransport();
	
	State state() const;
	virtual void send(int component, const QByteArray &data) = 0;
	JingleTransportInfo::Ptr localInfo() const;
	virtual void setRemoteInfo(const JingleTransportInfo::Ptr &info, bool final) = 0;
	
protected:
	void setState(State state);
	void setLocalInfo(const JingleTransportInfo::Ptr &info);
	
signals:
	void received(int component, const QByteArray &data);
	void localInfoReady(const Jreen::JingleTransportInfo::Ptr &info);
	void stateChanged(Jreen::JingleTransport::State);
	
private:
	QScopedPointer<JingleTransportPrivate> d_ptr;
};

class AbstractJingleTransportFactory : public AbstractPayloadFactory
{
public:
	inline JingleTransportInfo::Ptr createInfo() { return createPayload(); }
	virtual JingleTransport *createObject(JingleContent *content) = 0;
};

template <typename Extension>
class JingleTransportFactory : public AbstractJingleTransportFactory
{
public:
	JingleTransportFactory(const QString &uri);
	virtual ~JingleTransportFactory();
	
	virtual int payloadType() const;
	virtual QStringList features() const;
	virtual bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	
protected:
	const QString m_elementUri;
};

template <typename Extension>
Q_INLINE_TEMPLATE JingleTransportFactory<Extension>::JingleTransportFactory(const QString &uri)
	: m_elementUri(uri)
{
}

template <typename Extension>
Q_INLINE_TEMPLATE JingleTransportFactory<Extension>::~JingleTransportFactory()
{
}

template <typename Extension>
Q_INLINE_TEMPLATE int JingleTransportFactory<Extension>::payloadType() const
{
	return Extension::staticPayloadType();
}

template <typename Extension>
Q_INLINE_TEMPLATE QStringList JingleTransportFactory<Extension>::features() const
{
	return QStringList(m_elementUri);
}

template <typename Extension>
Q_INLINE_TEMPLATE bool JingleTransportFactory<Extension>::canParse(const QStringRef &name, const QStringRef &uri,
                                                                   const QXmlStreamAttributes &)
{
	return name == QLatin1String("transport") && uri == m_elementUri;
}

}

#endif // JINGLETRANSPORT_H
