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

#ifndef REGISTRATIONMANAGER_H
#define REGISTRATIONMANAGER_H

#include "dataform.h"
#include "error.h"
#include "bitsofbinary.h"
#include "iq.h"

namespace Jreen
{

class RegistrationManager;
class RegistrationManagerPrivate;
class RegistrationDataPrivate;

class JREEN_EXPORT RegistrationData
{
public:
	enum FieldType {
		UsernameField,
		NickField,
		PasswordField,
		FullNameField,
		FirstNameField,
		LastNameField,
		EmailField,
		AddressField,
		CityField,
		StateField,
		ZipField,
		PhoneField,
		UrlField,
		DateField,
//		MiscField,
//		TextField,
//		KeyField,
		LastFieldType = DateField
	};
	
	RegistrationData();
	RegistrationData(const RegistrationData &data);
	RegistrationData &operator =(const RegistrationData &data);
	~RegistrationData();

	DataForm::Ptr form() const;
	void setForm(const DataForm::Ptr &form);
	bool hasForm() const;
	QString instructions() const;
	QString fieldValue(FieldType type) const;
	bool hasField(FieldType type) const;
	bool hasFields() const;
	void setFieldValue(FieldType type, const QString &value);
	QList<Jreen::BitsOfBinary::Ptr> bitsOfBinaries() const;
	
private:
	RegistrationData(const QSharedDataPointer<RegistrationDataPrivate> &data);
	
	friend class RegistrationManager;
	friend class RegistrationManagerPrivate;
	friend class RegistrationDataPrivate;
	QSharedDataPointer<RegistrationDataPrivate> d;
};

class JREEN_EXPORT RegistrationManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(RegistrationManager)
public:
	RegistrationManager(const JID &service, Client *client);
	~RegistrationManager();
	
signals:
	void formReceived(const Jreen::RegistrationData &data);
	void error(const Jreen::Error::Ptr &error);
	void success();
	void unsupported();
	
public slots:
	void registerAtServer();
	void registerAtService();
	void fetchFields();
	void send(const Jreen::RegistrationData &data);
	
private:
	Q_PRIVATE_SLOT(d_func(), void _q_form_received(const Jreen::IQ &iq))
	Q_PRIVATE_SLOT(d_func(), void _q_result_received(const Jreen::IQ &iq))
	Q_PRIVATE_SLOT(d_func(), void _q_on_disconnect(Jreen::Client::DisconnectReason))
	QScopedPointer<RegistrationManagerPrivate> d_ptr;
};

}

#endif // REGISTRATIONMANAGER_H
