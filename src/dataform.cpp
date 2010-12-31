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

#include "dataform_p.h"
#include "jstrings.h"
#include <QStringList>

namespace jreen
{

DataFormField::DataFormField(Type type, const QString &var, const QString &label) : d_ptr(new DataFormFieldPrivate)
{
	d_ptr->type = type;
	d_ptr->var = var;
	d_ptr->label = label;
}

DataFormField::DataFormField(const DataFormField &o) : d_ptr(o.d_ptr)
{
}

DataFormField &DataFormField::operator =(const DataFormField &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

DataFormField::~DataFormField()
{
}

DataFormField::Type DataFormField::type() const
{
	return d_ptr->type;
}

bool DataFormField::isRequired() const
{
	return d_ptr->required;
}

void DataFormField::setRequired(bool required)
{
	d_ptr->required = required;
}

QString DataFormField::var() const
{
	return d_ptr->var;
}

void DataFormField::setVar(const QString &var)
{
	d_ptr->var = var;
}

QString DataFormField::label() const
{
	return d_ptr->label;
}

void DataFormField::setLabel(const QString &label)
{
	d_ptr->label = label;
}

QString DataFormField::description() const
{
	return d_ptr->desc;
}

void DataFormField::setDescription(const QString &desc)
{
	d_ptr->desc = desc;
}

QVariantList DataFormField::values() const
{
	return d_ptr->values;
}

QVariant DataFormField::value() const
{
	return d_ptr->values.value(0);
}

DataFormFieldBoolean::DataFormFieldBoolean(const QString &var, bool value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values << QLatin1String(value ? "1" : "0");
}

DataFormFieldBoolean::DataFormFieldBoolean(const DataFormField &o) : DataFormField(o)
{
}

bool DataFormFieldBoolean::value() const
{
	// QVariant::toBool() handles both "1" and "true"
	return d_ptr->values.value(0).toBool();
}

void DataFormFieldBoolean::setValue(bool value)
{
	d_ptr->values = QVariantList() << QLatin1String(value ? "1" : "0");
}

DataFormFieldFixed::DataFormFieldFixed(const QString &var, const QString &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldFixed::DataFormFieldFixed(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldFixed::value() const
{
	return d_ptr->values.value(0).toString();
}

void DataFormFieldFixed::setValue(const QString &value)
{
	d_ptr->values = QVariantList() << value;
}


DataFormFieldHidden::DataFormFieldHidden(const QString &var, const QString &value)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldHidden::DataFormFieldHidden(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldHidden::value() const
{
	return d_ptr->values.value(0).toString();
}

void DataFormFieldHidden::setValue(const QString &value)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldJidMulti::DataFormFieldJidMulti(const QString &var, const QList<JID> &values, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
}

DataFormFieldJidMulti::DataFormFieldJidMulti(const DataFormField &o) : DataFormField(o)
{
}

QList<JID> DataFormFieldJidMulti::values() const
{
	QList<JID> jids;
	for (int i = 0; i < d_ptr->values.size(); i++) {
		JID jid = d_ptr->values.at(i).value<JID>();
		if (jid.isValid())
			jids << jid;
	}
	return jids;
}

void DataFormFieldJidMulti::setValues(const QList<JID> &values)
{
	d_ptr->values.clear();
	for (int i = 0; i < values.size(); i++) {
		QVariant var = qVariantFromValue(values.at(i));
		if (d_ptr->values.contains(var))
			continue;
		d_ptr->values << var;
	}
}

DataFormFieldJidSingle::DataFormFieldJidSingle(const QString &var, const JID &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QVariantList() << qVariantFromValue(value);
}

DataFormFieldJidSingle::DataFormFieldJidSingle(const DataFormField &o) : DataFormField(o)
{
}

DataFormOptionContainer::DataFormOptionContainer(const DataFormField &that)
	: DataFormField(that)
{
}

DataFormOptionContainer::DataFormOptionContainer(Type type, const QString &var, const QString &label)
	: DataFormField(type, var, label)
{
}

int DataFormOptionContainer::optionsCount() const
{
	return d_ptr->options.count();
}

QString DataFormOptionContainer::optionLabel(int index) const
{
	return d_ptr->options.value(index).first;
}

QString DataFormOptionContainer::optionValue(int index) const
{
	return d_ptr->options.value(index).second;
}

void DataFormOptionContainer::addOption(const QString &label, const QString &value)
{
	d_ptr->options << qMakePair(label, value);
}

void DataFormOptionContainer::removeOption(int index)
{
	d_ptr->options.removeAt(index);
}

JID DataFormFieldJidSingle::value() const
{
	return d_ptr->values.value(0).value<JID>();
}

void DataFormFieldJidSingle::setValue(const JID &value)
{
	d_ptr->values = QVariantList() << qVariantFromValue(value);
}

DataFormFieldListMulti::DataFormFieldListMulti(const QString &var, const QStringList &values, const QString &label)
	: DataFormOptionContainer(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QVariant(values).toList();
}

DataFormFieldListMulti::DataFormFieldListMulti(const DataFormField &o) : DataFormOptionContainer(o)
{
}

bool DataFormFieldListMulti::isChecked(int index) const
{
	return d_ptr->values.contains(d_ptr->options.value(index).second);
}

struct DataFormValueLessThen
{
	const QList<QPair<QString, QString> > &options;
	int index(const QVariant &v)
	{
		const QString s = v.toString();
		for (int i = 0; i < options.size(); i++) {
			if (options.at(i).second == s)
				return i;
		}
		return -1;
	}

	bool operator()(const QVariant &a, const QVariant &b)
	{
		return index(a) < index(b);
	}
};

void DataFormFieldListMulti::setChecked(int index, bool checked)
{
	QVariant value = d_ptr->options.value(index).second;
	if (!checked) {
		d_ptr->values.removeOne(value);
	} else {
		if (d_ptr->values.contains(value))
			return;
		DataFormValueLessThen lessThen = { d_ptr->options };
		QVariantList::iterator it = qLowerBound(d_ptr->values.begin(), d_ptr->values.end(), value, lessThen);
		d_ptr->values.insert(it, value);
	}
}

DataFormFieldListSingle::DataFormFieldListSingle(const QString &var, const QString &value, const QString &label)
	: DataFormOptionContainer(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldListSingle::DataFormFieldListSingle(const DataFormField &o) : DataFormOptionContainer(o)
{
}

QString DataFormFieldListSingle::value() const
{
	return d_ptr->values.value(0).toString();
}

void DataFormFieldListSingle::setValue(const QString &value)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldTextMulti::DataFormFieldTextMulti(const QString &var, const QString &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldTextMulti::DataFormFieldTextMulti(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldTextMulti::value() const
{
	QString result;
	for (int i = 0; i < d_ptr->values.size(); i++) {
		if (i)
			result += QLatin1Char('\n');
		result += d_ptr->values.at(i).toString();
	}
	return result;
}

void DataFormFieldTextMulti::setValue(const QString &value)
{
	d_ptr->values.clear();
	foreach (const QString &str, value.split(QLatin1Char('\n')))
		d_ptr->values << str;
}


DataFormFieldTextPrivate::DataFormFieldTextPrivate(const QString &var, const QString &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldTextPrivate::DataFormFieldTextPrivate(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldTextPrivate::value() const
{
	return d_ptr->values.value(0).toString();
}

void DataFormFieldTextPrivate::setValue(const QString &value)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldTextSingle::DataFormFieldTextSingle(const QString &var, const QString &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldTextSingle::DataFormFieldTextSingle(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldTextSingle::value() const
{
	return d_ptr->values.value(0).toString();
}

void DataFormFieldTextSingle::setValue(const QString &value)
{
	d_ptr->values = QVariantList() << value;
}

DataFormFieldNone::DataFormFieldNone(const QString &var, const QStringList &values, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QVariant(values).toList();
}

DataFormFieldNone::DataFormFieldNone(const DataFormField &o) : DataFormField(o)
{
}

QStringList DataFormFieldNone::values() const
{
	return QVariant(d_ptr->values).toStringList();
}

void DataFormFieldNone::setValues(const QStringList &values)
{
	d_ptr->values = QVariant(values).toList();
}

DataFormFieldContainer::DataFormFieldContainer() : d_ptr(new DataFormFieldContainerPrivate)
{
}

DataFormFieldContainer::DataFormFieldContainer(DataFormFieldContainerPrivate &d) : d_ptr(&d)
{
}

DataFormFieldContainer::~DataFormFieldContainer()
{
}

int DataFormFieldContainer::fieldsCount() const
{
	return d_func()->fields.size();
}

DataFormField DataFormFieldContainer::field(int index) const
{
	return d_func()->fields.value(index);
}

DataFormField DataFormFieldContainer::field(const QString &name) const
{
	return d_func()->field(name);
}

DataFormField DataFormFieldContainer::field(const QLatin1String &name) const
{
	return d_func()->field(name);
}

void DataFormFieldContainer::removeField(int index)
{
	d_func()->fields.removeAt(index);
}

void DataFormFieldContainer::appendField(const DataFormField &field)
{
	d_func()->fields.append(field);
}

void DataFormFieldContainer::setFields(const QList<DataFormField> &fields)
{
	d_func()->fields = fields;
}

DataForm::DataForm(Type type, const QString &title) : DataFormFieldContainer(*new DataFormPrivate)
{
	Q_D(DataForm);
	d->type = type;
	d->title = title;
}

DataForm::~DataForm()
{
}

QString DataForm::title() const
{
	return d_func()->title;
}

QList<DataFormItem::Ptr> DataForm::items() const
{
	return d_func()->items;
}

DataFormReported::Ptr DataForm::reported() const
{
	return d_func()->reported;
}

}
