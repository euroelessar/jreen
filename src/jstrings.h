/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JREEN_JSTRINGS_H
#define JREEN_JSTRINGS_H

#include "jreen.h"

namespace Jreen {
//enums
template<typename T, int N>
Q_INLINE_TEMPLATE int strToEnum(const T &str, const char *(&strings)[N])
{
	for(int i=0; i < N; i++) {
		if(QLatin1String(strings[i]) == str)
			return i;
	}
	return -1;
}

template<typename X,typename T, int N>
Q_INLINE_TEMPLATE X strToEnum(const T &str, const char *(&strings)[N])
{
	return static_cast<X>(strToEnum(str,strings));
}

template<int N>
Q_INLINE_TEMPLATE QLatin1String enumToStr(int i, const char *(&strings)[N])
{
	return QLatin1String((i < 0 || i >= N) ? 0 : strings[i]);
}

//flags
template<typename T, int N>
Q_INLINE_TEMPLATE int strToFlag(const T &str, const char *(&strings)[N])
{
	int flag = 0x1;
	for(int i=0;i<=N;i++) {
		if(QLatin1String(strings[i]) == str)
			return i;
		flag <<= 1;
	}
	return -1;
}

template<typename X, typename T, int N>
Q_INLINE_TEMPLATE X strToFlag(const T &str, const char *(&strings)[N])
{
	return static_cast<X>(strToFlag(str,strings));
}

template<int N>
Q_INLINE_TEMPLATE QString flagToStr(uint i, const char *(&strings)[N])
{
	uint n = 1;
	while ((1 << n) < i && n < N)
		++n;
	if (n >= N)
		return QString();
	return QLatin1String(strings[n]);
}


}

#endif // JREEN_JSTRINGS_H
