/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef TLSFEATURE_H
#define TLSFEATURE_H

#include "streamfeature_p.h"
#include <QtCrypto>

namespace Jreen
{
class TLSFeature : public QObject, public StreamFeature
{
	Q_OBJECT
public:
	enum State {
		AtStart,
		AtFeature,
		AtChallenge,
		AtSuccess
	};
    TLSFeature();
	int priority() { return 10; }
//		void setStreamInfo(StreamInfo *info);
	void reset();
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	virtual void setStreamInfo(StreamInfo *info);
	bool isActivatable();
	bool activate();
private slots:
	void onHandshaken();
	void onClosed();
	void onError();
	void onDisconnected();
private:
	void init();
	
	struct ScopedPointerEventDeleter
	{
		static inline void cleanup(QObject *pointer) { if (pointer) pointer->deleteLater(); }
	};
	QScopedPointer<QCA::TLS, ScopedPointerEventDeleter> m_tls;
	bool m_hasTls;
	bool m_required;
	bool m_available;
};
}

#endif // TLSFEATURE_H
