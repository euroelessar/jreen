/****************************************************************************
 *  multimediadata.h
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

#ifndef MULTIMEDIADATA_H
#define MULTIMEDIADATA_H
#include <QVariantMap>
#include "stanzaextension.h"

namespace Jreen
{

// XEP-0221
// http://xmpp.org/extensions/xep-0221.html
class MultimediaDataPrivate;
class JREEN_EXPORT MultimediaData
{
public:
	enum Type
	{
		Audio,
		Image
	};
	MultimediaData(Type type,const QVariantList &data,const QVariantMap &attributes = QVariantMap());
	MultimediaData(const MultimediaData &other);
	~MultimediaData();
	MultimediaData &operator =(const MultimediaData &o);
	QVariantMap attributes() const;
	QVariantList data() const;
	void setData(const QVariantList &data);
	void setAttributes(const QVariantMap &attributes);
private:
	QSharedDataPointer<MultimediaDataPrivate> d_ptr;
	friend class MultimediaDataPrivate;
};

} // namespace Jreen
#endif // MULTIMEDIADATA_H
