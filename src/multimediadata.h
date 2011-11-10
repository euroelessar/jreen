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
