/****************************************************************************
 *  dataform.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "dataform.h"
#include "jstrings.h"
#include <QStringList>

namespace jreen
{

class DataFormFieldPrivate
{
public:
	DataFormField::Type type;
	QString var;
	QString desc;
	QString label;
	QStringList values;
	bool required;
	DataFormOptionList options;
};

DataFormField::DataFormField(const QString &var, const QString &label, Type type) :
	d_ptr(new DataFormFieldPrivate)
{
	Q_D(DataFormField);
	d->var = var;
	d->label = label;
	d->type = type;
}

DataFormField::DataFormField(const QString &var, const QString &value,
							 const QString &label, Type type) :
	d_ptr(new DataFormFieldPrivate)
{
	Q_D(DataFormField);
	d->var = var;
	d->label = label;
	d->type = type;
	d->values = QStringList(value);
}

DataFormField::DataFormField(const QString &var, const QStringList &values,
							 const QString &label, Type type) :
	d_ptr(new DataFormFieldPrivate)
{
	Q_D(DataFormField);
	d->var = var;
	d->label = label;
	d->type = type;
	d->values = values;
}

DataFormField::DataFormField(Type type) :
	d_ptr(new DataFormFieldPrivate)
{
	Q_D(DataFormField);
	d->type = type;
}

DataFormField::Type DataFormField::type() const
{
	return d_func()->type;
}

bool DataFormField::isValid() const
{
	return d_func()->type != DataFormField::Invalid;
}

DataFormOptionList DataFormField::options() const
{
	return d_func()->options;
}

DataFormOptionList &DataFormField::options()
{
	return d_func()->options;
}
void DataFormField::setOptions(const DataFormOptionList &options)
{
	d_func()->options = options;
}
QStringList &DataFormField::values()
{
	return d_func()->values;
}

QString DataFormField::value() const
{
	Q_D(const DataFormField);
	return d->values.size() ? d->values.first() : QString();
}
QStringList DataFormField::values() const
{
	return d_func()->values;
}

void DataFormField::setValues(const QStringList &values)
{
	d_func()->values = values;
}

QString DataFormField::var() const
{
	return d_func()->var;
}

void DataFormField::setVar(const QString &var)
{
	d_func()->var = var;
}

bool DataFormField::required() const
{
	return d_func()->required;
}

void DataFormField::setRequired(bool required)
{
	d_func()->required = required;
}

QString DataFormField::desc() const
{
	return d_func()->desc;
}
void DataFormField::setDesc(const QString &desc)
{
	d_func()->desc = desc;
}

DataForm::DataForm(Type type, const QString &title)
	:	m_title(title),m_form_type(type)
{
	m_form_type = Result;
}

DataFormFieldPointer DataFormFieldContainer::field(const QString &var) const
{
	foreach(DataFormFieldPointer field,fields()) {
		if (field->var() == var)
			return field;
	}
	return DataFormFieldPointer(0);
}

}
