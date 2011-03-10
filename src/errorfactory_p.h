/****************************************************************************
 *  errorfactory_p.h
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

#ifndef ERRORFACTORY_P_H
#define ERRORFACTORY_P_H
#include "error.h"
#include "stanzaextension.h"

namespace Jreen {

class ErrorFactory : public StanzaExtensionFactory<Error>
{
public:
    ErrorFactory();
    virtual ~ErrorFactory();
    virtual bool canParse(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes);
    virtual StanzaExtension::Ptr createExtension();
    virtual QStringList features() const;
    virtual void handleStartElement(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes);    
    virtual void handleCharacterData(const QStringRef& text);
    virtual void handleEndElement(const QStringRef& name, const QStringRef& uri);
    virtual void serialize(StanzaExtension* obj, QXmlStreamWriter* writer);
private:
	enum State {AtCondition,AtText};
	State m_state;
	int m_depth;
	Error::Type m_type;
	Error::Condition m_condition;
};

} // namespace Jreen

#endif // ERRORFACTORY_P_H
