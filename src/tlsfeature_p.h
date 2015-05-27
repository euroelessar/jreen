/****************************************************************************
**
** Jreen
**
** Copyright © 2013 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JREEN_TLSFEATURE_P_H
#define JREEN_TLSFEATURE_P_H

#include "streamfeature_p.h"
#include <QPointer>

class QSslError;
class QSslSocket;

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
	void onPeerVerifyError(const QSslError &error);
	void onDisconnected();
private:
	void init();

	QPointer<QSslSocket> m_socket;
	bool m_required;
	bool m_available;
};
}

#endif // JREEN_TLSFEATURE_P_H
