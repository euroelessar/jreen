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

J_STRING(field)
J_STRING(option)
J_STRING(label)
J_STRING(desc)
J_STRING(required)

static const QStringList dataformfield_types = QStringList()
											   << QLatin1String("boolean") << QLatin1String("fixed")
											   << QLatin1String("hidden") << QLatin1String("jid-multi")
											   << QLatin1String("jid-single") << QLatin1String("list-multi")
											   << QLatin1String("list-single") << QLatin1String("text-multi")
											   << QLatin1String("text-private") << QLatin1String("text-single")
											   << QString();

static const QStringList dataform_types = QStringList()
										  << QLatin1String("form") << QLatin1String("submit")
										  << QLatin1String("cancel") << QLatin1String("result");

static inline DataFormField::Type getDataFormFieldType(const QString &type)
{
	int t = dataformfield_types.indexOf(type);
	return t < 0 ? DataFormField::Invalid : static_cast<DataFormField::Type>(t);
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
