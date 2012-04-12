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

#ifndef JINGLEAUDIOCONTENTFACTORY_P_H
#define JINGLEAUDIOCONTENTFACTORY_P_H

#include "jingleaudiocontent_p.h"
#include "jingleaudiopayload.h"

namespace Jreen
{

class JingleAudioContentFactory : public JingleContentFactory<JingleAudioDescription>
{
public:
    JingleAudioContentFactory();
	
	virtual JingleContent *createObject(JingleSession *session);
	virtual QStringList features() const;
	virtual void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	virtual void handleEndElement(const QStringRef &name, const QStringRef &uri);
	virtual void handleCharacterData(const QStringRef &text);
	virtual void serialize(Payload *obj, QXmlStreamWriter *writer);
	virtual Payload::Ptr createPayload();
private:
	enum State { AtRoot, AtPayload } m_state;
	int m_depth;
	JingleAudioDescription::Ptr m_info;
	QScopedPointer<JingleAudioPayload> m_payload;
};

}

#endif // JINGLEAUDIOCONTENTFACTORY_P_H
