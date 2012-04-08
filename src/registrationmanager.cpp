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

#include "registrationmanager_p.h"
#include "registrationquery_p.h"
#include "registrationfeature_p.h"
#include "iqreply.h"
#include "iq_p.h"
#include "logger.h"
#include <QCoreApplication>
#include <QTimer>

namespace Jreen
{

RegistrationData::RegistrationData() : d(new RegistrationDataPrivate)
{
}

RegistrationData::RegistrationData(const RegistrationData &data) : d(data.d)
{
}

RegistrationData::RegistrationData(const QSharedDataPointer<RegistrationDataPrivate> &data) : d(data)
{
}

RegistrationData &RegistrationData::operator =(const RegistrationData &data)
{
	d = data.d;
	return *this;
}

RegistrationData::~RegistrationData()
{
}

DataForm::Ptr RegistrationData::form() const
{
	return d->form;
}

void RegistrationData::setForm(const DataForm::Ptr &form)
{
	d->form = form;
}

bool RegistrationData::hasForm() const
{
	return !d->form.isNull();
}

QString RegistrationData::instructions() const
{
	return d->instructions;
}

QString RegistrationData::fieldValue(FieldType type) const
{
	return d->values.value(type);
}

// It's not good time for localization support yet

//const char *registrationTranslations[] = {
//	QT_TRANSLATE_NOOP("Jreen", "Username"),
//	QT_TRANSLATE_NOOP("Jreen", "Familiar name of the user"),
//	QT_TRANSLATE_NOOP("Jreen", "Password"),
//	QT_TRANSLATE_NOOP("Jreen", "Full name"),
//	QT_TRANSLATE_NOOP("Jreen", "Given name"),
//	QT_TRANSLATE_NOOP("Jreen", "Family name"),
//	QT_TRANSLATE_NOOP("Jreen", "Email"),
//	QT_TRANSLATE_NOOP("Jreen", "Street"),
//	QT_TRANSLATE_NOOP("Jreen", "Locality"),
//	QT_TRANSLATE_NOOP("Jreen", "Region"),
//	QT_TRANSLATE_NOOP("Jreen", "Postal code"),
//	QT_TRANSLATE_NOOP("Jreen", "Phone number"),
//	QT_TRANSLATE_NOOP("Jreen", "Web page url"),
//	QT_TRANSLATE_NOOP("Jreen", "Birth date")
//};

//QString RegistrationData::fieldText(RegistrationData::FieldType type) const
//{
//	size_t count = sizeof(registrationTranslations) / sizeof(registrationTranslations[0]);
//	if (type >= 0 && type < count)
//		return QCoreApplication::translate("Jreen", registrationTranslations[type]);
//	return QString();
//}

bool RegistrationData::hasField(RegistrationData::FieldType type) const
{
	return (d->valuesFlags & (1 << type));
}

bool RegistrationData::hasFields() const
{
	return d->valuesFlags != 0;
}

void RegistrationData::setFieldValue(RegistrationData::FieldType type, const QString &value)
{
	if (d->values.size() <= type) {
		Logger::warning() << "Unkown RegistrationData::ValueType:" << type;
		return;
	}
	d->valuesFlags |= (1 << type);
	d->values[type] = value;
}

QList<BitsOfBinary::Ptr> RegistrationData::bitsOfBinaries() const
{
	return d->bobs;
}

void RegistrationManagerPrivate::requestInfo()
{
	ConnectionIQ iq(IQ::Get, service);
	iq.addPayload(new RegistrationQuery);
	sendIQ(iq, SLOT(_q_form_received(Jreen::IQ)));
}

void RegistrationManagerPrivate::handleConnection()
{
	waitingForConnection = false;
	while (!iqs.isEmpty()) {
		QPair<IQ, QByteArray> iq = iqs.takeFirst();
		sendIQ(iq.first, iq.second);
	}
	if (!fieldsReceived)
		requestInfo();
}

void RegistrationManagerPrivate::sendIQ(const IQ &iq, const char *slot)
{
	Q_Q(RegistrationManager);
	if (waitingForConnection) {
		iqs << qMakePair(iq, QByteArray(slot));
	} else {
		IQReply *reply = client->send(iq);
		QObject::connect(reply, SIGNAL(received(Jreen::IQ)),
		                 q, slot);
	}
}

void RegistrationManagerPrivate::_q_form_received(const Jreen::IQ &iq)
{
	Q_Q(RegistrationManager);
	fieldsReceived = true;
	if (RegistrationQuery::Ptr query = iq.payload<RegistrationQuery>())
		emit q->formReceived(RegistrationData(query->data));
	else
		emit q->error(iq.error());
}

void RegistrationManagerPrivate::_q_result_received(const Jreen::IQ &iq)
{
	Q_Q(RegistrationManager);
	if (iq.error()) {
		emit q->error(iq.error());
	} else {
		QObject::disconnect(client, 0, q, 0);
		for (int i = 0; i < configs.size(); ++i) {
			Client::Feature feature = static_cast<Client::Feature>(i);
			client->setFeatureConfig(feature, configs[i]);
		}
		if (feature) {
			client->removeStreamFeature(feature);
			delete feature;
			feature = 0;
		}
		emit q->success();
	}
}

void RegistrationManagerPrivate::_q_on_disconnect(Client::DisconnectReason reason)
{
	Q_Q(RegistrationManager);
	waitingForConnection = true;
	if (reason == Client::NoSupportedFeature
	        || reason == Client::NoEncryptionSupport
	        || reason == Client::NoCompressionSupport
	        || reason == Client::NoAuthorizationSupport) {
		emit q->unsupported();
	} else {
		QTimer::singleShot(0, client, SLOT(connectToServer()));
	}
}

RegistrationManager::RegistrationManager(const JID &service, Client *client)
    : QObject(client), d_ptr(new RegistrationManagerPrivate(this))
{
	Q_D(RegistrationManager);
	d->service = service;
	d->client = client;
	d->feature = 0;
}

RegistrationManager::~RegistrationManager()
{
}

void RegistrationManager::registerAtServer()
{
	Q_D(RegistrationManager);
	d->fieldsReceived = false;
	d->waitingForConnection = true;
	d->feature = new RegistrationFeature(this);
	connect(d->client, SIGNAL(disconnected(Jreen::Client::DisconnectReason)),
	        SLOT(_q_on_disconnect(Jreen::Client::DisconnectReason)));
	d->client->setJID(d->service);
	d->client->registerStreamFeature(d->feature);
	d->configs.resize(3);
	for (int i = 0; i < 3; ++i) {
		Client::Feature feature = static_cast<Client::Feature>(i);
		d->configs[i] = d->client->featureConfig(feature);
	}
	d->client->setFeatureConfig(Client::Authorization, Client::Disable);
	d->client->connectToServer();
}

void RegistrationManager::registerAtService()
{
	Q_D(RegistrationManager);
	d->waitingForConnection = false;
	d->requestInfo();
}

void RegistrationManager::fetchFields()
{
	Q_D(RegistrationManager);
	d->requestInfo();
}

void RegistrationManager::send(const Jreen::RegistrationData &data)
{
	Q_D(RegistrationManager);
	ConnectionIQ iq(IQ::Set, d->service);
	iq.addPayload(new RegistrationQuery(data));
	d->sendIQ(iq, SLOT(_q_result_received(Jreen::IQ)));
}

}

#include "moc_registrationmanager.cpp"
