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

#ifndef JINGLEICE_H
#define JINGLEICE_H

#include "jingletransport.h"

#ifdef HAVE_IRISICE

#include <QList>
#include "3rdparty/icesupport/ice176.h"
#include "3rdparty/icesupport/udpportreserver.h"
//#include <nice.h>

namespace Jreen
{
namespace JingleIce
{

class Transport : public JingleTransport
{
	Q_OBJECT
public:
    Transport(JingleContent *content);
	~Transport();
	
	virtual void send(int component, const QByteArray &data);
	virtual void setRemoteInfo(const JingleTransportInfo::Ptr &info, bool final);
	
private slots:
	void onIceStarted();
	void onIceError(XMPP::Ice176::Error error);
	void onIceLocalCandidatesReady(const QList<XMPP::Ice176::Candidate> &candidates);
	void onIceComponentReady(int component);
	void onIceReadyRead(int);
	
private:
	XMPP::Ice176 *m_ice;
	XMPP::UdpPortReserver *m_portReserver;
	QSet<int> m_ready;
};

typedef XMPP::Ice176::Candidate Candidate;

class TransportInfo : public JingleTransportInfo
{
	J_PAYLOAD(Jreen::JingleIce::TransportInfo)
public:
	QList<Candidate> candidates;
	QString pwd;
	QString ufrag;
};

class TransportFactory : public JingleTransportFactory<TransportInfo>
{
public:
    TransportFactory();
	
	virtual JingleTransport *createObject(JingleContent *content);
	virtual void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	virtual void handleEndElement(const QStringRef &name, const QStringRef &uri);
	virtual void handleCharacterData(const QStringRef &text);
	virtual void serialize(Payload *obj, QXmlStreamWriter *writer);
	virtual Payload::Ptr createPayload();
	
private:
	int m_depth;
	TransportInfo::Ptr m_info;
};

}
}

#endif // HAVE_IRISICE

#endif // JINGLEICE_H
