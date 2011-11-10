/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "dataform_p.h"
#include "jstrings.h"
#include <QStringList>

namespace Jreen
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

void DataFormField::setValues(const QStringList &values)
{
	d_ptr->values = values;
}

QStringList DataFormField::values() const
{
	return d_ptr->values;
}

void DataFormField::setValue(const QString &value)
{
	d_ptr->values = QStringList(value);
}

QString DataFormField::value() const
{
	return d_ptr->values.value(0);
}

DataFormFieldBoolean::DataFormFieldBoolean(const QString &var, bool value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	setValue(value);
}

DataFormFieldBoolean::DataFormFieldBoolean(const DataFormField &o) : DataFormField(o)
{
}

bool DataFormFieldBoolean::value() const
{
	// QVariant::toBool() handles both "1" and "true"
	return QVariant(d_ptr->values.value(0)).toBool();
}

void DataFormFieldBoolean::setValue(bool value)
{
	d_ptr->values = QStringList(QLatin1String(value ? "1" : "0"));
}

DataFormFieldFixed::DataFormFieldFixed(const QString &var, const QString &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldFixed::DataFormFieldFixed(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldFixed::value() const
{
	return d_ptr->values.value(0);
}

void DataFormFieldFixed::setValue(const QString &value)
{
	d_ptr->values = QStringList(value);
}


DataFormFieldHidden::DataFormFieldHidden(const QString &var, const QString &value)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldHidden::DataFormFieldHidden(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldHidden::value() const
{
	return d_ptr->values.value(0);
}

void DataFormFieldHidden::setValue(const QString &value)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldJidMulti::DataFormFieldJidMulti(const QString &var, const QList<JID> &values, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	setValues(values);
}

DataFormFieldJidMulti::DataFormFieldJidMulti(const DataFormField &o) : DataFormField(o)
{
}

QList<JID> DataFormFieldJidMulti::values() const
{
	QList<JID> jids;
	for (int i = 0; i < d_ptr->values.size(); i++) {
		JID jid = d_ptr->values.at(i);
		if (jid.isValid())
			jids << jid;
	}
	return jids;
}

void DataFormFieldJidMulti::setValues(const QList<JID> &values)
{
	d_ptr->values.clear();
	for (int i = 0; i < values.size(); i++) {
		if (d_ptr->values.contains(values.at(i)))
			continue;
		d_ptr->values << values.at(i);
	}
}

DataFormFieldJidSingle::DataFormFieldJidSingle(const QString &var, const JID &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QStringList(value);
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
	return d_ptr->values.value(0);
}

void DataFormFieldJidSingle::setValue(const JID &value)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldListMulti::DataFormFieldListMulti(const QString &var, const QStringList &values, const QString &label)
	: DataFormOptionContainer(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = values;
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
	int index(const QVariant &s)
	{
		for (int i = 0; i < options.size(); i++) {
			if (options.at(i).second == s)
				return i;
		}
		return -1;
	}

	bool operator()(const QString &a, const QString &b)
	{
		return index(a) < index(b);
	}
};

void DataFormFieldListMulti::setChecked(int index, bool checked)
{
	QString value = d_ptr->options.value(index).second;
	if (!checked) {
		d_ptr->values.removeOne(value);
	} else {
		if (d_ptr->values.contains(value))
			return;
		DataFormValueLessThen lessThen = { d_ptr->options };
		QStringList::iterator it = qLowerBound(d_ptr->values.begin(), d_ptr->values.end(), value, lessThen);
		d_ptr->values.insert(it, value);
	}
}

DataFormFieldListSingle::DataFormFieldListSingle(const QString &var, const QString &value, const QString &label)
	: DataFormOptionContainer(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldListSingle::DataFormFieldListSingle(const DataFormField &o) : DataFormOptionContainer(o)
{
}

QString DataFormFieldListSingle::value() const
{
	return d_ptr->values.value(0);
}

void DataFormFieldListSingle::setValue(const QString &value)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldTextMulti::DataFormFieldTextMulti(const QString &var, const QString &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldTextMulti::DataFormFieldTextMulti(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldTextMulti::value() const
{
	return d_ptr->values.join(QLatin1String("\n"));
}

void DataFormFieldTextMulti::setValue(const QString &value)
{
	d_ptr->values = value.split(QLatin1Char('\n'));
}

DataFormFieldTextPrivate::DataFormFieldTextPrivate(const QString &var, const QString &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldTextPrivate::DataFormFieldTextPrivate(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldTextPrivate::value() const
{
	return d_ptr->values.value(0);
}

void DataFormFieldTextPrivate::setValue(const QString &value)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldTextSingle::DataFormFieldTextSingle(const QString &var, const QString &value, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldTextSingle::DataFormFieldTextSingle(const DataFormField &o) : DataFormField(o)
{
}

QString DataFormFieldTextSingle::value() const
{
	return d_ptr->values.value(0);
}

void DataFormFieldTextSingle::setValue(const QString &value)
{
	d_ptr->values = QStringList(value);
}

DataFormFieldNone::DataFormFieldNone(const QString &var, const QStringList &values, const QString &label)
	: DataFormField(static_cast<DataFormField::Type>(StaticType), var, label)
{
	d_ptr->values = values;
}

DataFormFieldNone::DataFormFieldNone(const DataFormField &o) : DataFormField(o)
{
}

QStringList DataFormFieldNone::values() const
{
	return d_ptr->values;
}

void DataFormFieldNone::setValues(const QStringList &values)
{
	d_ptr->values = values;
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

DataForm::Type DataForm::type() const
{
	return d_func()->type;
}

void DataForm::setType(Type type)
{
	d_func()->type = type;
}

QString DataForm::typeName() const
{
	return field(QLatin1String("FORM_TYPE")).value();
}

void DataForm::setTypeName(const QString &type)
{
	Q_D(DataForm);
	DataFormField f = field(QLatin1String("FORM_TYPE"));
	if (f.type() == DataFormField::Invalid) {
		f = DataFormFieldHidden(QLatin1String("FORM_TYPE"), type);
		d->fields.prepend(f);
	} else {
		f.setValue(type);
	}
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
