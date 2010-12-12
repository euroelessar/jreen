/****************************************************************************
 *  dataformfactory.cpp
 *
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

#include "dataformfactory_p.h"
#include "jstrings.h"
#include <QXmlStreamWriter>
#include "multimediadatafactory_p.h"
#define NS_DATAFORM QLatin1String("jabber:x:data")

namespace jreen {

inline void writeAttribute(QXmlStreamWriter *writer,
						   const QLatin1String &name,const QString &value)
{
	if(!value.isEmpty())
		writer->writeAttribute(name,value);
}
inline void writeTextElement(QXmlStreamWriter *writer,
							 const QLatin1String &name,const QString &value)
{
	if(!value.isEmpty())
		writer->writeTextElement(name,value);
}



class DataFormOptionParser : XmlStreamFactory<DataFormOption>
{
public:
	DataFormOptionParser()
	{
		clear();
	}
	virtual ~DataFormOptionParser()
	{

	}
	virtual bool canParse(const QStringRef &name, const QStringRef &uri,
						  const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
		return name == QLatin1String("option");
	}
	virtual void handleStartElement(const QStringRef &name, const QStringRef &uri,
									const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
		m_depth++;
		if(m_depth == 1)
			m_label = attributes.value(QLatin1String("label")).toString();
	}
	virtual void handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
		m_depth--;
	}
	virtual void handleCharacterData(const QStringRef &text)
	{
		if(m_depth == 1)
			m_value = text.toString();
	}
	virtual void serialize(DataFormOption *option, QXmlStreamWriter *writer)
	{
		writer->writeStartElement(QLatin1String("option"));
		writeAttribute(writer,QLatin1String("label"),option->label);
		writeTextElement(writer,QLatin1String("value"),option->value);
		writer->writeEndElement();
	}
	void serialize(const DataFormOptionList &options, QXmlStreamWriter *writer)
	{
		foreach(DataFormOptionPointer optionPtr,options)
			serialize(optionPtr.data(),writer);
	}
	DataFormOptionPointer create()
	{
		DataFormOption *opt = new DataFormOption();
		opt->label = m_label;
		opt->value = m_value;
		clear();
		return DataFormOptionPointer(opt);
	}
private:
	void clear()
	{
		m_label.clear();
		m_value.clear();
	}
	int m_depth;
	QString m_label;
	QString m_value;
};

static const char* datafield_types[] = {
	"boolean",
	"fixed",
	"hidden",
	"jid-multi",
	"jid-single",
	"list-multi",
	"list-single",
	"text-multi",
	"text-private",
	"text-single"
};

class DataFormFieldParser : XmlStreamFactory<DataFormField>
{
public:
	DataFormFieldParser()
	{
		m_depth = 0;
	}
	virtual ~DataFormFieldParser()
	{

	}
	virtual bool canParse(const QStringRef &name, const QStringRef &uri,
						  const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
		return name == QLatin1String("field");
	}
	virtual void handleStartElement(const QStringRef &name, const QStringRef &uri,
									const QXmlStreamAttributes &attributes)
	{
		m_depth++;
		if(m_depth == 1) {
			m_type = strToEnum<DataFormField::Type>(attributes.value(QLatin1String("type")),datafield_types);
			m_var = attributes.value(QLatin1String("var")).toString();
			m_label = attributes.value(QLatin1String("label")).toString();
		} else if (m_depth == 2) {
			if(name == QLatin1String("value"))
				m_state = AtValue;
			else if(m_optionParser.canParse(name,uri,attributes))
				m_state = AtOption;
			else if(name == QLatin1String("required")) {
				m_state = AtRequied;
				m_required = true;
			}
		}
		if(m_state == AtOption)
			m_optionParser.handleStartElement(name,uri,attributes);
	}
	virtual void handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		if(m_state == AtOption) {
			if(m_depth == 2)
				m_options.append(m_optionParser.create());
			else
				m_optionParser.handleEndElement(name,uri);
		}
		m_depth--;
	}
	virtual void handleCharacterData(const QStringRef &text)
	{
		switch(m_state) {
		case AtValue:
			m_values.append(text.toString());
			break;
		case AtOption:
			m_optionParser.handleCharacterData(text);
		default:
			break;
		}
	}
	virtual void serialize(DataFormField *field, QXmlStreamWriter *writer)
	{
		writer->writeStartElement(QLatin1String("field"));
		writeAttribute(writer,QLatin1String("type"),enumToStr(field->type(),datafield_types));
		writeAttribute(writer,QLatin1String("label"),field->desc());
		writeAttribute(writer,QLatin1String("var"),field->var());
		m_optionParser.serialize(field->options(),writer);
		foreach(const QString &value,field->values())
			writeTextElement(writer,QLatin1String("value"),value);
		if(field->required())
			writer->writeEmptyElement(QLatin1String("required"));
		writer->writeEndElement();
	}
	void serialize(const DataFormFieldList &fields, QXmlStreamWriter *writer)
	{
		foreach(DataFormFieldPointer fieldPtr,fields)
			serialize(fieldPtr.data(),writer);
	}
	DataFormFieldPointer create()
	{
		DataFormField *field = new DataFormField(m_var,m_values,m_label,m_type);
		clear();
		return DataFormFieldPointer(field);
	}
private:
	enum State {
		AtValue,
		AtOption,
		AtRequied
	};
	void clear() {
		m_options.clear();
		m_type = DataFormField::Invalid;
		m_label.clear();
		m_var.clear();
		m_values.clear();
		m_required = false;
	}
	State m_state;
	int m_depth;
	DataFormField::Type m_type;
	QString m_label;
	QString m_var;
	QStringList m_values;
	DataFormOptionList m_options;
	bool m_required;
	DataFormOptionParser m_optionParser;
	MultimediaDataFactory m_multimediaDataFactory;
};

enum DataFormState { AtTitle, AtInstruction, AtField };

//static const char* dataform_states[] = {
//	"title",
//	"instruction",
//	"field"
//};

static const char* dataform_types[] = {
	"form",
	"submit",
	"cancel",
	"result"
};

class DataFormFactoryPrivate
{
public:
	void clear()
	{
		title.clear();
		instruction.clear();
		fields.clear();
		formType = DataForm::Invalid;
	}
	int depth;
	DataFormState state;
	QString title;
	QString instruction;
	//DataFormReportedList reported;
	DataFormFieldList fields;
	DataForm::Type formType;
	DataFormFieldParser fieldParser;
};

DataFormFactory::DataFormFactory() : d_ptr(new DataFormFactoryPrivate)
{
	Q_D(DataFormFactory);
	d->depth = 0;
	d->clear();
}

DataFormFactory::~DataFormFactory()
{
}

QStringList DataFormFactory::features() const
{
	return QStringList(NS_DATAFORM);
}

bool DataFormFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("x") && uri == QLatin1String(NS_DATAFORM);
}

void DataFormFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_D(DataFormFactory);
	d->depth++;
	if(d->depth == 1) {
		d->formType = strToEnum<DataForm::Type>(attributes.value(QLatin1String("type")),dataform_types);
	} else if(d->depth == 2) {
		if(d->fieldParser.canParse(name,uri,attributes))
			d->state = AtField;
		else if(name == QLatin1String("title"))
			d->state = AtTitle;
		else if(name == QLatin1String("instruction"))
			d->state = AtInstruction;
	}
	if(d->state == AtField)
		d->fieldParser.handleStartElement(name,uri,attributes);
}

void DataFormFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_D(DataFormFactory);
	if(d->state == AtField) {
		if(d->depth == 2)
			d->fields.append(d->fieldParser.create());
		else
			d->fieldParser.handleEndElement(name,uri);
	}
	d->depth--;
}

void DataFormFactory::handleCharacterData(const QStringRef &text)
{
	Q_D(DataFormFactory);
	switch(d->state) {
	case AtTitle:
		d->title = text.toString();
	case AtInstruction:
		d->instruction = text.toString();
	case AtField:
		d->fieldParser.handleCharacterData(text);
	}
}

void DataFormFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Q_D(DataFormFactory);
	DataForm *form = se_cast<DataForm*>(extension);
	writer->writeStartElement(QLatin1String("x"));
	writer->writeDefaultNamespace(NS_DATAFORM);
	writeTextElement(writer,QLatin1String("title"),form->title());
	//writer->writeTextElement(QLatin1String("instruction"),form->));
	d->fieldParser.serialize(form->fields(),writer);
	writer->writeEndElement();
}

StanzaExtension::Ptr DataFormFactory::createExtension()
{
	Q_D(DataFormFactory);
	DataForm *form = new DataForm(d->formType,d->title);
	form->setFields(d->fields);
	d->clear();
	return StanzaExtension::Ptr(form);
}

} // namespace jreen
