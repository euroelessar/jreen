/****************************************************************************
 *  multimediadata.cpp
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
