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
#define NS_DATAFORM QLatin1String("jabber:x:data")

namespace jreen {

static const char* dataform_types[] = {
	"form",
	"submit",
	"cancel",
	"result"
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

enum DataFormState { AtStart, AtResource, AtJid };

class DataFormFactoryPrivate
{
public:
	int depth;
	DataFormState state;
	QString title;
	DataFormReportedList reported;
	DataFormItemList items;
	DataForm::Type formType;
};

DataFormFactory::DataFormFactory() : d_ptr(new DataFormFactoryPrivate)
{
	Q_D(DataFormFactory);
	d->state = AtStart;
	d->depth = 0;
	d->formType = DataForm::Invalid;
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
}

void DataFormFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
}

void DataFormFactory::handleCharacterData(const QStringRef &text)
{
}

void DataFormFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
}

StanzaExtension::Ptr DataFormFactory::createExtension()
{
	Q_D(DataFormFactory);
	DataForm *form = new DataForm(d->formType,d->title);
//	form->setFields(d->items);
	return StanzaExtension::Ptr(form);
}

} // namespace jreen
