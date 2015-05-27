/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_REGISTRATIONMANAGER_P_H
#define JREEN_REGISTRATIONMANAGER_P_H

#include "registrationmanager.h"
#include "client.h"
#include <QQueue>

namespace Jreen
{

class RegistrationFeature;
class RegistrationQuery;

class RegistrationDataPrivate : public QSharedData
{
public:
	typedef QSharedDataPointer<RegistrationDataPrivate> Ptr;

	enum Flag {
		Registered = 0x01,
		Remove     = 0x02
	};
	Q_DECLARE_FLAGS(Flags, Flag)
	
	RegistrationDataPrivate() : valuesFlags(0) { values.resize(RegistrationData::LastFieldType + 1); }
	RegistrationDataPrivate(const RegistrationDataPrivate &o)
	    : QSharedData(o), form(o.form), flags(o.flags), instructions(o.instructions),
	      valuesFlags(o.valuesFlags), values(o.values), bobs(o.bobs) {}
	static Ptr get(const RegistrationData &data) { return data.d; }

	DataForm::Ptr form;
	Flags flags;
	QString instructions;
	int valuesFlags;
	QVector<QString> values;
	QList<BitsOfBinary::Ptr> bobs;
};

class RegistrationManagerPrivate
{
	Q_DECLARE_PUBLIC(RegistrationManager)
public:
	RegistrationManagerPrivate(RegistrationManager *q) : q_ptr(q), fieldsReceived(false), waitingForConnection(true) {}
	
	static RegistrationManagerPrivate *get(RegistrationManager *manager) { return manager->d_func(); }
	void requestInfo();
	void handleConnection();
	void sendIQ(const Jreen::IQ &iq, const char *slot);
	void _q_form_received(const Jreen::IQ &iq);
	void _q_result_received(const Jreen::IQ &iq);
	void _q_on_disconnect(Jreen::Client::DisconnectReason reason);
	
	RegistrationManager *q_ptr;
	Client *client;
	RegistrationFeature *feature;
	bool fieldsReceived;
	bool waitingForConnection;
	QQueue<QPair<Jreen::IQ, QByteArray> > iqs;
	QVector<Client::FeatureConfig> configs;
	JID service;
};

}

#endif // JREEN_REGISTRATIONMANAGER_P_H
