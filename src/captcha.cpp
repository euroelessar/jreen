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

#include "captcha.h"

namespace Jreen {

class CaptchaPrivate
{
public:
	DataForm::Ptr form;
};

Captcha::Captcha(DataForm::Ptr form) : d_ptr(new CaptchaPrivate)
{
	d_func()->form = form;
}

Captcha::~Captcha()
{
}

DataForm::Ptr Captcha::form() const
{
	return d_func()->form;
}

void Captcha::setForm(const DataForm::Ptr &form)
{
	d_func()->form = form;
}

} // namespace Jreen
