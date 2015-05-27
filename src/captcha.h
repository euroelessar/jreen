/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_CAPTCHA_H
#define JREEN_CAPTCHA_H

#include "dataform.h"

namespace Jreen {

class CaptchaPrivate;

class JREEN_EXPORT Captcha : public Payload
{
	J_PAYLOAD(Jreen::Captcha)
	Q_DECLARE_PRIVATE(Captcha)
public:
	Captcha(DataForm::Ptr form = DataForm::Ptr());
	~Captcha();
	
	DataForm::Ptr form() const;
	void setForm(const DataForm::Ptr &form);
	
private:
	QScopedPointer<CaptchaPrivate> d_ptr;
};

} // namespace Jreen

#endif // JREEN_CAPTCHA_H
