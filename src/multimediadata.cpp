/****************************************************************************
**
** Jreen
**
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

#include "multimediadata.h"

namespace Jreen
{

class MultimediaDataPrivate : public QSharedData
{
public:
	MultimediaDataPrivate() : QSharedData() {}
	MultimediaDataPrivate(const MultimediaDataPrivate &other) :
		QSharedData(other),
		type(other.type),
		data(other.data),
		attributes(other.attributes)
	{
	}
	MultimediaData::Type type;
	QVariantList data;
	QVariantMap attributes;
};

MultimediaData::MultimediaData(Type type,const QVariantList &data,const QVariantMap &attributes) :
	d_ptr(new MultimediaDataPrivate)
{
	d_ptr->type = type;
	d_ptr->data = data;
	d_ptr->attributes = attributes;
}

MultimediaData::MultimediaData(const MultimediaData &other) :
	d_ptr(other.d_ptr)
{
}

MultimediaData::~MultimediaData()
{

}

MultimediaData &MultimediaData::operator =(const MultimediaData &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

QVariantMap MultimediaData::attributes() const
{
	return d_ptr->attributes;
}
QVariantList MultimediaData::data() const
{
	return d_ptr->data;
}
void MultimediaData::setData(const QVariantList &data)
{
	d_ptr->data = data;
}

void MultimediaData::setAttributes(const QVariantMap &attributes)
{
	d_ptr->attributes = attributes;
}

} // namespace Jreen
