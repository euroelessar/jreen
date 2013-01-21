/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JREEN_LOGGER_H
#define JREEN_LOGGER_H

#include <QtDebug>
#include "jreen.h"

namespace Jreen
{
class Logger;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#  define jreenDebug QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "jreen").debug
#  define jreenWarning QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "jreen").warning
#  define jreenCritical QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "jreen").critical
#  define jreenFatal QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "jreen").critical
#else
#  define jreenDebug Logger::debug
#  define jreenWarning Logger::warning
#  define jreenCritical Logger::critical
#  define jreenFatal Logger::fatal
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0) || QT_DEPRECATED_SINCE(5, 0)
class JREEN_EXPORT Debug
{
	struct Stream {
		Stream(QtMsgType t) : ref(1), debug(&stream), type(t) {}
		int ref;
		QString stream;
		QDebug debug;
		QtMsgType type;
	} *stream;

public:
	inline Debug(QtMsgType type) : stream(new Debug::Stream(type)) {}
	inline Debug(const Debug &o) : stream(o.stream) { ++stream->ref; }
	inline Debug &operator =(const Debug &o);
	inline ~Debug();

	template <typename T>
	Q_INLINE_TEMPLATE Debug &operator<<(T t);

private:
	friend class Logger;
};

class JREEN_EXPORT Logger
{
public:
	static Debug debug() { return Debug(QtDebugMsg); }
	static Debug warning() { return Debug(QtWarningMsg); }
	static Debug critical() { return Debug(QtCriticalMsg); }
	static Debug fatal() { return Debug(QtFatalMsg); }

	static void addHandler(QtMsgHandler handler);
	static void removeHandler(QtMsgHandler handler);

	static bool isNull();

private:
	friend class Debug;
	static void flushDebug(Debug::Stream *stream);

	Logger();
	~Logger();
};

inline Debug &Debug::operator =(const Debug &o)
{
	if (this != &o) {
		Debug copy(o);
		qSwap(stream, copy.stream);
	}
	return *this;
}

inline Debug::~Debug()
{
	if (!--stream->ref) {
		Logger::flushDebug(stream);
		delete stream;
	}
}

template <typename T>
Q_INLINE_TEMPLATE Debug &Debug::operator<<(T t)
{
	if (!Logger::isNull())
		stream->debug << t;
	return *this;
}
#endif

}

#endif // JREEN_LOGGER_H
