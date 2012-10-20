/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "disco_p.h"
#include "iq.h"
#include "iqreply.h"
#include "jstrings.h"
#include "dataform.h"
#include "softwareversion.h"
#include "client.h"
#include <QXmlStreamWriter>

#define NS_DISCO_INFO QLatin1String("http://jabber.org/protocol/disco#info")
#define NS_DISCO_ITEMS QLatin1String("http://jabber.org/protocol/disco#items")

namespace Jreen
{

DiscoInfoFactory::DiscoInfoFactory()
{
	m_depth = 0;
	m_state = AtStart;
	m_hasDataForm = false;
}

QStringList DiscoInfoFactory::features() const
{
	return QStringList(NS_DISCO_INFO);
}

bool DiscoInfoFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_DISCO_INFO;
}

void DiscoInfoFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	m_depth++;
	if (m_depth == 1) {
		m_node = attributes.value(QLatin1String("node")).toString();
		m_identities.clear();
		m_features.clear();
		m_hasDataForm = false;
		m_state = AtInfo;
	} else if (m_depth == 2) {
		if (name == QLatin1String("identity")) {
			Disco::Identity identity(attributes.value(QLatin1String("category")).toString(),
									 attributes.value(QLatin1String("type")).toString(),
									 attributes.value(QLatin1String("name")).toString(),
									 attributes.value(QLatin1String("xml:lang")).toString());
			m_identities.append(identity);
		} else if (name == QLatin1String("feature")) {
			m_features.insert(attributes.value(QLatin1String("var")).toString());
		} else if (m_factory.canParse(name, uri, attributes)) {
			m_hasDataForm = true;
			m_state = AtDataForm;
		}
	}
	if (m_state == AtDataForm)
		m_factory.handleStartElement(name, uri, attributes);
}

void DiscoInfoFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtDataForm)
		m_factory.handleEndElement(name, uri);
	if (m_depth == 2 && m_state == AtDataForm) {
		m_state = AtInfo;
	} else if (m_depth == 1) {
		m_state = AtStart;
	}
	m_depth--;
}

void DiscoInfoFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtDataForm)
		m_factory.handleCharacterData(text);
}

void DiscoInfoFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Disco::Info *info = se_cast<Disco::Info*>(extension);
	if (!info)
		return;
	writer->writeStartElement(QLatin1String("query"));
	if (!info->node().isEmpty())
		writer->writeAttribute(QLatin1String("node"),info->node());
	writer->writeDefaultNamespace(NS_DISCO_INFO);
	foreach (const Disco::Identity &identity, info->identities()) {
		writer->writeEmptyElement(QLatin1String("identity"));
		writer->writeAttribute(QLatin1String("category"), identity.category());
		writer->writeAttribute(QLatin1String("type"), identity.type());
		writer->writeAttribute(QLatin1String("name"), identity.name());
		if (!identity.lang().isEmpty())
			writer->writeAttribute(QLatin1String("xml:lang"), identity.lang());
	}
	foreach (const QString &feature, info->features()) {
		writer->writeEmptyElement(QLatin1String("feature"));
		writer->writeAttribute(QLatin1String("var"), feature);
	}
	if (info->form())
		m_factory.serialize(info->form().data(), writer);
	writer->writeEndElement();
}

Payload::Ptr DiscoInfoFactory::createPayload()
{
	Payload::Ptr dataForm;
	if (m_hasDataForm)
		dataForm = m_factory.createPayload();
	return Payload::Ptr(new Disco::Info(m_node, m_identities, m_features,
	                                    dataForm.staticCast<DataForm>()));
}

DiscoItemsFactory::DiscoItemsFactory()
{

}

QStringList DiscoItemsFactory::features() const
{
	return QStringList(NS_DISCO_ITEMS);
}

bool DiscoItemsFactory::canParse(const QStringRef &name, const QStringRef &uri,
			  const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_DISCO_ITEMS;
}

void DiscoItemsFactory::handleStartElement(const QStringRef &name,
						const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth++;
	if(m_depth == 1) {
		m_items.clear();
		m_node = attributes.value("node").toString();
	} else if(m_depth == 2) {
		Disco::Item item;
		item.setJid(attributes.value("jid").toString());
		item.setName(attributes.value("name").toString());
		item.setNode(attributes.value("node").toString());
		m_items.append(item);
	}
}

void DiscoItemsFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void DiscoItemsFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void DiscoItemsFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Disco::Items *items = se_cast<Disco::Items*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_DISCO_ITEMS);
	if (!items->node().isEmpty())
		writer->writeAttribute(QLatin1String("node"), items->node());
	foreach(const Disco::Item &item, items->items()) {
		writer->writeEmptyElement(QLatin1String("item"));
		writer->writeAttribute(QLatin1String("jid"), item.jid());
		if (!item.node().isEmpty())
			writer->writeAttribute(QLatin1String("node"), item.node());
		writer->writeAttribute(QLatin1String("name"), item.name());
	}
	writer->writeEndElement();
}

Payload::Ptr DiscoItemsFactory::createPayload()
{
	return Payload::Ptr(new Disco::Items(m_node, m_items));
}

enum { ActionsNotInitialized = 0x1000 };

class Disco::ItemData : public QSharedData
{
public:
	inline ItemData() : actions(baseAction()) {}
	inline ItemData(const ItemData &o)
	    : QSharedData(o), jid(o.jid), node(o.node), name(o.name),
	      features(o.features), identities(o.identities), actions(baseAction()) {}
	inline static Disco::Item::Action baseAction() { return static_cast<Disco::Item::Action>(ActionsNotInitialized); }
	inline void clearActions() { actions = baseAction(); }

	JID jid;
	QString node;
	QString name;
	QSet<QString> features;
	Disco::IdentityList identities;
	DataForm::Ptr form;
	mutable Disco::Item::Actions actions;
};

class Disco::IdentityData : public QSharedData
{
public:
	IdentityData() {}
	IdentityData(const IdentityData &o)
	    : QSharedData(o), category(o.category), type(o.type), name(o.name), lang(o.lang) {}
	
	QString category;
	QString type;
	QString name;
	QString lang;
};

class Disco::InfoPrivate
{
public:
	QString node;
	Disco::IdentityList identities;
	QSet<QString> features;
	DataForm::Ptr form;
};

class Disco::ItemsPrivate
{
public:
	Disco::ItemList items;
	QString node;
};

Disco::Identity::Identity() : d(new Disco::IdentityData)
{
}

Disco::Identity::Identity(const QString &category, const QString &type, const QString &name, const QString &lang)
    : d(new Disco::IdentityData)
{
	d->category = category;
	d->type = type;
	d->name = name;
	d->lang = lang;
}

Disco::Identity::Identity(const Disco::Identity &item) : d(item.d)
{
}

Disco::Identity &Disco::Identity::operator =(const Disco::Identity &item)
{
	d = item.d;
	return *this;
}

Disco::Identity::~Identity()
{
}

QString Disco::Identity::category() const
{
	return d->category;
}

QString Disco::Identity::type() const
{
	return d->type;
}

QString Disco::Identity::name() const
{
	return d->name;
}

QString Disco::Identity::lang() const
{
	return d->lang;
}

Disco::Info::Info(const QString &node, const Disco::IdentityList &identities,
        const QSet<QString> &features, QSharedPointer<DataForm> form)
    : d_ptr(new InfoPrivate)
{
	Q_D(Info);
	d->node = node;
	d->identities = identities;
	d->features = features;
	d->form = form;
}

Disco::Info::~Info()
{
}

QString Disco::Info::node() const
{
	return d_func()->node;
}

Disco::IdentityList Disco::Info::identities() const
{
	return d_func()->identities;
}

QSet<QString> Disco::Info::features() const
{
	return d_func()->features;
}

DataForm::Ptr Disco::Info::form() const
{
	return d_func()->form;
}

Disco::Item::Item() : d(new Disco::ItemData)
{
}

Disco::Item::Item(const JID &jid, const QString &node, const QString &name)
    : d(new Disco::ItemData)
{
	d->jid = jid;
	d->node = node;
	d->name = name;
}

Disco::Item::Item(const Disco::Item &item) : d(item.d)
{
}

Disco::Item &Disco::Item::operator =(const Disco::Item &item)
{
	d = item.d;
	return *this;
}

Disco::Item::~Item()
{
}

JID Disco::Item::jid() const
{
	return d->jid;
}

void Disco::Item::setJid(const JID &jid)
{
	d->jid = jid;
}

QString Disco::Item::node() const
{
	return d->node;
}

void Disco::Item::setNode(const QString &node)
{
	d->node = node;
}

QString Disco::Item::name() const
{
	return d->name;
}

void Disco::Item::setName(const QString &name)
{
	d->name = name;
}

Disco::IdentityList Disco::Item::identities() const
{
	return d->identities;
}

bool Disco::Item::hasIdentity(const QString &category, const QString &type) const
{
	Q_ASSERT(!category.isEmpty() || !type.isEmpty());
	foreach (const Disco::Identity &identity, d->identities) {
		if ((category.isEmpty() || identity.category() == category)
		        && (type.isEmpty() || identity.type() == type)) {
			return true;
		}
	}
	return false;
}

void Disco::Item::addIdentity(const Disco::Identity &identity)
{
	d->identities << identity;
	d->clearActions();
}

void Disco::Item::setIdentities(const Disco::IdentityList &identities)
{
	d->identities = identities;
	d->clearActions();
}

QSet<QString> Disco::Item::features() const
{
	return d->features;
}

void Disco::Item::setFeatures(const QSet<QString> &features)
{
	d->features = features;
	d->clearActions();
}

DataForm::Ptr Disco::Item::form() const
{
	return d->form;
}

void Disco::Item::setForm(const DataForm::Ptr &form)
{
	d->form = form;
}

Disco::Item::Actions Disco::Item::actions() const
{
	if (d->actions & ActionsNotInitialized) {
		// Some kind of magic for client support
		d->actions = Disco::Item::Actions();
		bool isIRC = hasIdentity(QLatin1String("conference"), QLatin1String("irc"));
		d->actions |= ActionAdd;
		if (d->features.contains(QLatin1String("http://jabber.org/protocol/muc"))
			&& (!d->jid.node().isEmpty() || isIRC)) {
			d->actions |= ActionJoin;
		}
		if (d->features.contains(QLatin1String("http://jabber.org/protocol/bytestreams")))
			d->actions |= ActionProxy;
		if(d->features.contains("http://jabber.org/protocol/muc#register")
				|| d->features.contains(QLatin1String("jabber:iq:register"))) {
			d->actions |= ActionRegister;
		}
		if (d->features.contains(QLatin1String("jabber:iq:search")))
			d->actions |= ActionSearch;
		if (d->features.contains(QLatin1String("vcard-temp")))
			d->actions |= ActionVCard;
		if (d->features.contains(NS_DISCO_ITEMS)
				|| (d->features.contains(QLatin1String("http://jabber.org/protocol/muc")) && !isIRC)
				|| (d->features.isEmpty() && d->identities.isEmpty())) {
			d->actions |= ActionExpand;
		}
		if (hasIdentity(QLatin1String("automation"))) {
			if (hasIdentity(QString(), QLatin1String("command-list")))
				d->actions |= ActionExpand;
			d->actions |= ActionExecute;
		} else if (d->features.contains(QLatin1String("http://jabber.org/protocol/commands"))) {
			d->actions |= ActionExpand;
			d->actions |= ActionExecute;
		}
	}
	return d->actions;
}

Disco::Items::Items(const QString &node, const Disco::ItemList &items)
    : d_ptr(new ItemsPrivate)
{
	Q_D(Items);
	d->node = node;
	d->items = items;
}

Disco::Items::~Items()
{
}

QString Disco::Items::node() const
{
	return d_func()->node;
}

Disco::ItemList Disco::Items::items() const
{
	return d_func()->items;
}

Disco::Disco(Client *client) : d_ptr(new DiscoPrivate)
{
	Q_D(Disco);
	d->client = client;
	connect(d->client, SIGNAL(iqReceived(Jreen::IQ)), this, SLOT(_q_received(Jreen::IQ)));
}

Disco::~Disco()
{
}

const Disco::IdentityList &Disco::identities() const
{
	Q_D(const Disco);
	return d->identities;
}

Disco::IdentityList &Disco::identities()
{
	Q_D(Disco);
	return d->identities;
}

DiscoReply *Disco::requestInfo(const Disco::Item &item)
{
	IQ info(IQ::Get, item.jid());
	info.addExtension(new Info(item.node()));
	return new DiscoReply(item, d_func()->client->send(info));
}

DiscoReply *Disco::requestItems(const Disco::Item &item)
{
	IQ info(IQ::Get, item.jid());
	info.addExtension(new Items(item.node()));
	return new DiscoReply(item, d_func()->client->send(info));
}

void Disco::addIdentity(const Identity &identity)
{
	d_func()->identities.append(identity);
}

void Disco::addIdentity(const QString &category, const QString &type, const QString &name, const QString &lang)
{
	d_func()->identities.append(Identity(category, type, name, lang));
}

const QSet<QString> &Disco::features() const
{
	Q_D(const Disco);
	return d->features;
}

void Disco::addFeature(const QString &feature)
{
	d_func()->features << feature;
}

QSet<QString> &Disco::features()
{
	return d_func()->features;
}

void DiscoPrivate::_q_received(const Jreen::IQ &iq)
{
	if(iq.subtype() != IQ::Get)
		return;
	QList<Payload*> payloads;
	if(const Disco::Info::Ptr info = iq.payload<Disco::Info>())
		payloads << new Disco::Info(info->node(), identities, features, form);
	if(const Disco::Items::Ptr items = iq.payload<Disco::Items>())
		payloads << new Disco::Items(items->node());
	if(iq.payload<SoftwareVersion>())
		payloads << new SoftwareVersion(software_name, software_version, os);
	if (!payloads.isEmpty()) {
		iq.accept();
		IQ receipt(IQ::Result, iq.from(), iq.id());
		foreach (Payload *payload, payloads)
			receipt.addExtension(payload);
		client->send(receipt);
	}
}

void Disco::setSoftwareVersion(const QString &name, const QString &version, const QString &os)
{
	setSoftwareVersion(name, version, os, QString());
}

void Disco::setSoftwareVersion(const QString &name, const QString &version, const QString &os, const QString &osVersion)
{
	Q_D(Disco);
	d->software_name = name;
	d->software_version = version;
	d->os = os;
	DataForm::Ptr form = DataForm::Ptr::create();
	form->setType(DataForm::Result);
	form->appendField(DataFormFieldHidden(QLatin1String("FORM_TYPE"), QLatin1String("urn:xmpp:dataforms:softwareinfo")));
	form->appendField(DataFormFieldNone(QLatin1String("ip_version"), QStringList() << QLatin1String("ipv4") << QLatin1String("ipv6")));
	form->appendField(DataFormFieldNone(QLatin1String("os"), QStringList(os)));
	if (!osVersion.isEmpty())
		form->appendField(DataFormFieldNone(QLatin1String("os_version"), QStringList(osVersion)));
	form->appendField(DataFormFieldNone(QLatin1String("software"), QStringList(name)));
	form->appendField(DataFormFieldNone(QLatin1String("software_version"), QStringList(version)));
	d->form = form;
}

const DataForm *Disco::form() const
{
	Q_D(const Disco);
	return d->form.data();
}

void Disco::setForm(DataForm *form)
{
	Q_D(Disco);
	d->form = QSharedPointer<DataForm>(form);
}

class DiscoReplyPrivate
{
	Q_DECLARE_PUBLIC(DiscoReply)
public:
	DiscoReplyPrivate(DiscoReply *q) : q_ptr(q) {}
	void _q_received(const Jreen::IQ &iq);

	DiscoReply *q_ptr;
	Disco::Item item;
	Disco::ItemList subitems;
	Jreen::Error::Ptr error;
};

void DiscoReplyPrivate::_q_received(const Jreen::IQ &iq)
{
	if (iq.error()) {
		error = iq.error();
		emit q_ptr->error(error);
	} else if (Jreen::Disco::Info::Ptr info = iq.payload<Jreen::Disco::Info>()) {
		item.setJid(iq.from().full());
		item.setNode(info->node());
		item.setFeatures(info->features());
		item.setIdentities(info->identities());
		item.setForm(info->form());
		emit q_ptr->infoReceived(item);
	} else if (Jreen::Disco::Items::Ptr items = iq.payload<Jreen::Disco::Items>()) {
		subitems = items->items();
		emit q_ptr->itemsReceived(subitems);
	}
	emit q_ptr->finished();
}

DiscoReply::DiscoReply(const Disco::Item &item, Jreen::IQReply *reply)
    : QObject(reply), d_ptr(new DiscoReplyPrivate(this))
{
	d_func()->item = item;
	connect(reply, SIGNAL(received(Jreen::IQ)), SLOT(_q_received(Jreen::IQ)));
}

DiscoReply::~DiscoReply()
{
}

Disco::Item DiscoReply::item() const
{
	return d_func()->item;
}

Disco::ItemList DiscoReply::subitems() const
{
	return d_func()->subitems;
}

QString DiscoReply::errorText() const
{
	Q_D(const DiscoReply);
	if (!d->error)
		return QString();
	return d->error->conditionText();
}

Jreen::Error::Ptr DiscoReply::error() const
{
	return d_func()->error;
}

}

#include "moc_disco.cpp"
