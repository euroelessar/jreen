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

#ifndef JINGLECONTENT_H
#define JINGLECONTENT_H

#include "../stanzaextension.h"
#include "jingletransport.h"
#include <QStringList>

namespace Jreen
{

class JingleSession;
class JingleContentPrivate;

typedef Payload JingleDescription;

class JREEN_EXPORT JingleContent : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JingleContent)
	Q_PROPERTY(Jreen::JingleContent::State state READ state NOTIFY stateChanged)
public:
	enum State {
		Disconnected,
		Gathering,
		Connecting,
		Connected,
		Failed
	};
	
    JingleContent(JingleSession *session);
	~JingleContent();
	
	JingleSession *session() const;
	int componentCount() const;
	virtual JingleDescription::Ptr defaultDescription() = 0;
	virtual JingleDescription::Ptr handleDescription(const JingleDescription::Ptr &description) = 0;
	State state() const;
	bool isAcceptable() const;
	void accept();
	void decline();
	
signals:
	void stateChanged(Jreen::JingleContent::State);
	
protected:
    JingleContent(JingleSession *session, JingleContentPrivate &p);
	void setComponentCount(int count);
	void send(int component, const QByteArray &data);
	void send(int component, const char *data, int size);
	virtual void receive(int component, const QByteArray &data) = 0;
	Q_PRIVATE_SLOT(d_func(), void _q_received(int, const QByteArray &))
	Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(Jreen::JingleTransport::State))
	Q_PRIVATE_SLOT(d_func(), void _q_localInfoReady(const Jreen::JingleTransportInfo::Ptr &))
	
	QScopedPointer<JingleContentPrivate> d_ptr;
};

class JREEN_EXPORT AbstractJingleContentFactory : public AbstractPayloadFactory
{
public:
	inline JingleDescription::Ptr createDescription() { return createPayload(); }
	virtual QString media() const = 0;
	virtual JingleContent *createObject(JingleSession *session) = 0;
};

template <typename Extension>
class JingleContentFactory : public AbstractJingleContentFactory
{
public:
	JingleContentFactory(const QString &uri, const QString &media = QString());
	virtual ~JingleContentFactory();
	
	virtual QString media() const;
	virtual int payloadType() const;
	virtual QStringList features() const;
	virtual bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	
protected:
	const QString m_elementUri;
	const QString m_media;
};

template <typename Extension>
Q_INLINE_TEMPLATE JingleContentFactory<Extension>::JingleContentFactory(const QString &uri, const QString &media)
	: m_elementUri(uri), m_media(media)
{
}

template <typename Extension>
Q_INLINE_TEMPLATE JingleContentFactory<Extension>::~JingleContentFactory()
{
}

template <typename Extension>
Q_INLINE_TEMPLATE QString JingleContentFactory<Extension>::media() const
{
	return m_media;
}

template <typename Extension>
Q_INLINE_TEMPLATE int JingleContentFactory<Extension>::payloadType() const
{
	return Extension::staticPayloadType();
}

template <typename Extension>
Q_INLINE_TEMPLATE QStringList JingleContentFactory<Extension>::features() const
{
	return QStringList(m_elementUri);
}

template <typename Extension>
Q_INLINE_TEMPLATE bool JingleContentFactory<Extension>::canParse(const QStringRef &name, const QStringRef &uri,
                                                                   const QXmlStreamAttributes &attributes)
{
	return name == QLatin1String("description") && uri == m_elementUri
	        && (m_media.isEmpty() || attributes.value(QLatin1String("media")) == m_media);
}

}

#endif // JINGLECONTENT_H
