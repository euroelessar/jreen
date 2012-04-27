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

#include "jingletransportice_p.h"

#ifdef HAVE_IRISICE

#include <QHostAddress>
#include <QNetworkInterface>
#include "jinglecontent.h"
#include "jinglesession.h"
#include "../jstrings.h"
#include "../util.h"
#include "../logger.h"

#define NS_ICE_UDP QLatin1String("urn:xmpp:jingle:transports:ice-udp:1")

namespace Jreen
{
namespace JingleIce
{

static inline int addressPriority(const QHostAddress &address)
{
	if (address.protocol() == QAbstractSocket::IPv6Protocol) {
		if (address == QHostAddress(QHostAddress::LocalHostIPv6))
			return 0;
		if (XMPP::Ice176::isIPv6LinkLocalAddress(address))
			return 1;
	} else if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        quint32 v4 = address.toIPv4Address();
        quint8 a0 = v4 >> 24;
        quint8 a1 = (v4 >> 16) & 0xff;
        if(a0 == 127)
            return 0;
        else if(a0 == 169 && a1 == 254)
            return 1;
        else if(a0 == 10)
            return 2;
        else if(a0 == 172 && a1 >= 16 && a1 <= 31)
            return 2;
        else if(a0 == 192 && a1 == 168)
            return 2;
	}
	return 3;
}

static inline bool addressLessThen(const QHostAddress &a, const QHostAddress &b)
{
	int cmp = addressPriority(a) - addressPriority(b);
	if (cmp != 0)
		return cmp < 0;
	return a.protocol() == QAbstractSocket::IPv6Protocol
	        && b.protocol() != QAbstractSocket::IPv6Protocol;
}

Transport::Transport(JingleContent *content)
    : JingleTransport(content)
{
	QList<QHostAddress> addresses;
	foreach (const QNetworkInterface &networkInterface, QNetworkInterface::allInterfaces()) {
		if (networkInterface.flags() & QNetworkInterface::IsLoopBack)
			continue;
		foreach (const QNetworkAddressEntry &addressEntry, networkInterface.addressEntries()) {
			QHostAddress address = addressEntry.ip();
			if(address.protocol() == QAbstractSocket::IPv6Protocol && XMPP::Ice176::isIPv6LinkLocalAddress(address))
				address.setScopeId(networkInterface.name());
			addresses << address;
		}
	}
	qSort(addresses.begin(), addresses.end(), addressLessThen);
	QList<XMPP::Ice176::LocalAddress> localAddresses;
	foreach (const QHostAddress &address, addresses) {
		XMPP::Ice176::LocalAddress localAddress;
		localAddress.addr = address;
		localAddresses << localAddress;
	}

	m_ice = new XMPP::Ice176(this);
	m_ice->setComponentCount(content->componentCount());
	for (int i = 0; i < content->componentCount(); ++i)
		m_ready.insert(i);
	m_ice->setLocalCandidateTrickle(true);
	m_ice->setLocalAddresses(localAddresses);
	m_portReserver = new XMPP::UdpPortReserver(m_ice);
	m_portReserver->setAddresses(addresses);
	m_portReserver->setPorts(qrand() & 0xffff, 4);
	m_ice->setPortReserver(m_portReserver);
	
	connect(m_ice, SIGNAL(started()), SLOT(onIceStarted()));
    connect(m_ice, SIGNAL(error(XMPP::Ice176::Error)), SLOT(onIceError(XMPP::Ice176::Error)));
    connect(m_ice, SIGNAL(localCandidatesReady(const QList<XMPP::Ice176::Candidate> &)), SLOT(onIceLocalCandidatesReady(const QList<XMPP::Ice176::Candidate> &)));
    connect(m_ice, SIGNAL(componentReady(int)), SLOT(onIceComponentReady(int)), Qt::QueuedConnection); // signal is not DOR-SS
	connect(m_ice, SIGNAL(readyRead(int)), SLOT(onIceReadyRead(int)));
	
	m_ice->start(content->isAcceptable() ? XMPP::Ice176::Responder : XMPP::Ice176::Initiator);
}

Transport::~Transport()
{
}

void Transport::send(int component, const QByteArray &data)
{
	m_ice->writeDatagram(component, data);
}

void Transport::setRemoteInfo(const JingleTransportInfo::Ptr &genericInfo, bool final)
{
	Q_UNUSED(final);
	Logger::debug() << Q_FUNC_INFO;
	TransportInfo::Ptr info = genericInfo.staticCast<TransportInfo>();
	if (!info->ufrag.isEmpty())
		m_ice->setPeerUfrag(info->ufrag);
	if (!info->pwd.isEmpty())
		m_ice->setPeerPassword(info->pwd);
	m_ice->addRemoteCandidates(info->candidates);
	setState(Connecting);
}

void Transport::onIceStarted()
{
	Logger::debug() << Q_FUNC_INFO;
}

void Transport::onIceError(XMPP::Ice176::Error error)
{
	Logger::debug() << Q_FUNC_INFO << error;
	setState(Failed);
}

void Transport::onIceLocalCandidatesReady(const QList<XMPP::Ice176::Candidate> &candidates)
{
	Logger::debug() << Q_FUNC_INFO;
	TransportInfo::Ptr info = TransportInfo::Ptr::create();
	info->candidates = candidates;
	info->ufrag = m_ice->localUfrag();
	info->pwd = m_ice->localPassword();
	setLocalInfo(info);
}

void Transport::onIceComponentReady(int component)
{
	m_ready.remove(component);
	Logger::debug() << Q_FUNC_INFO << component;
	if (m_ready.isEmpty())
		setState(Connected);
}

void Transport::onIceReadyRead(int component)
{
	while (m_ice->hasPendingDatagrams(component)) {
		emit received(component, m_ice->readDatagram(component));
	}
}

TransportFactory::TransportFactory()
    : JingleTransportFactory<TransportInfo>(NS_ICE_UDP), m_depth(0)
{
}

JingleTransport *TransportFactory::createObject(JingleContent *content)
{
	return new Transport(content);
}

void TransportFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attr)
{
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		m_info = TransportInfo::Ptr::create();
		m_info->pwd = attr.value(QLatin1String("pwd")).toString();
		m_info->ufrag = attr.value(QLatin1String("ufrag")).toString();
	} else if (m_depth == 2 && name == QLatin1String("candidate")) {
		m_info->candidates << Candidate();
		Candidate &cand = m_info->candidates.last();
		cand.type = attr.value(QLatin1String("type")).toString();
		cand.ip = QHostAddress(attr.value(QLatin1String("ip")).toString());
		cand.port = attr.value(QLatin1String("port")).toString().toInt();
		cand.network = attr.value(QLatin1String("network")).toString().toInt();
		cand.component = attr.value(QLatin1String("component")).toString().toInt();
		cand.priority = attr.value(QLatin1String("priority")).toString().toInt();
		cand.protocol = attr.value(QLatin1String("protocol")).toString();
		cand.foundation = attr.value(QLatin1String("foundation")).toString();
		cand.rel_addr = QHostAddress(attr.value(QLatin1String("rel-addr")).toString());
		cand.rel_port = attr.value(QLatin1String("rel-port")).toString().toInt();
		cand.generation = attr.value(QLatin1String("generation")).toString().toInt();
		cand.id = attr.value(QLatin1String("id")).toString();
	}
}

void TransportFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void TransportFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void TransportFactory::serialize(Payload *obj, QXmlStreamWriter *writer)
{
	TransportInfo *info = se_cast<TransportInfo*>(obj);
	Q_ASSERT(info);
	writer->writeStartElement(QLatin1String("transport"));
	writer->writeDefaultNamespace(m_elementUri);
	if (!info->pwd.isEmpty())
		writer->writeAttribute(QLatin1String("pwd"), info->pwd);
	if (!info->ufrag.isEmpty())
		writer->writeAttribute(QLatin1String("ufrag"), info->ufrag);
	for (int i = 0; i < info->candidates.size(); ++i) {
		const Candidate &c = info->candidates.at(i);
		writer->writeEmptyElement(QLatin1String("candidate"));
	    writer->writeAttribute(QLatin1String("component"), QString::number(c.component));
	    writer->writeAttribute(QLatin1String("foundation"), c.foundation);
	    writer->writeAttribute(QLatin1String("generation"), QString::number(c.generation));
	    if(!c.id.isEmpty())
	        writer->writeAttribute(QLatin1String("id"), c.id);
	    writer->writeAttribute(QLatin1String("ip"), c.ip.toString());
	    if(c.network != -1)
	        writer->writeAttribute(QLatin1String("network"), QString::number(c.network));
	    else // weird?
	        writer->writeAttribute(QLatin1String("network"), QString::number(0));
	    writer->writeAttribute(QLatin1String("port"), QString::number(c.port));
	    writer->writeAttribute(QLatin1String("priority"), QString::number(c.priority));
	    writer->writeAttribute(QLatin1String("protocol"), c.protocol);
	    if(!c.rel_addr.isNull())
	        writer->writeAttribute(QLatin1String("rel-addr"), c.rel_addr.toString());
	    if(c.rel_port != -1)
	        writer->writeAttribute(QLatin1String("rel-port"), QString::number(c.rel_port));
	    writer->writeAttribute("type", c.type);
	}
	writer->writeEndElement();
}

Payload::Ptr TransportFactory::createPayload()
{
	Logger::debug() << Q_FUNC_INFO << m_info->pwd << m_info->ufrag;
	Logger::debug() << Q_FUNC_INFO << m_info->candidates.size();
	Payload::Ptr result = m_info;
	m_info.clear();
	return result;
}

}

}

#endif
