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

#include "error.h"
#include "jstrings.h"
#include "langmap.h"
#include <QStringList>
#include <QXmlStreamWriter>
#include <QCoreApplication>

namespace Jreen
{

class ErrorPrivate
{
public:
	Error::Type type;
	Error::Condition condition;
	LangMap text;
};

Error::Error(Error::Type type, Error::Condition condition, const Jreen::LangMap& string)
	: Payload(), d_ptr(new ErrorPrivate)
{
	Q_D(Error);
	d->type = type;
	d->condition = condition;
	d->text = string;
}

Error::~Error()
{
}

Error::Type Error::type() const
{
	return d_func()->type;
}

QString Error::text(const QString &lang) const
{
	return d_func()->text.value(lang);
}

Error::Condition Error::condition() const
{
	return d_func()->condition;
}

QString Error::conditionText() const
{
	QString errorText;
	switch (d_func()->condition) {
	case Jreen::Error::BadRequest:
		errorText = QCoreApplication::translate("Jreen::Error", "The sender has sent XML that is malformed or that cannot be processed.");
		break;
	case Jreen::Error::Conflict:
		errorText = QCoreApplication::translate("Jreen::Error", "Access cannot be granted because an existing resource or session exists with the same name or address.");
		break;
	case Jreen::Error::FeatureNotImplemented:
		errorText = QCoreApplication::translate("Jreen::Error", "The feature requested is not implemented by the recipient or server and therefore cannot be processed.");
		break;
	case Jreen::Error::Forbidden:
		errorText = QCoreApplication::translate("Jreen::Error", "The requesting entity does not possess the required permissions to perform the action.");
		break;
	case Jreen::Error::Gone:
		errorText = QCoreApplication::translate("Jreen::Error", "The recipient or server can no longer be contacted at this address.");
		break;
	case Jreen::Error::InternalServerError:
		errorText = QCoreApplication::translate("Jreen::Error", "The server could not process the stanza because of a misconfiguration or an otherwise-undefined internal server error.");
		break;
	case Jreen::Error::ItemNotFound:
		errorText = QCoreApplication::translate("Jreen::Error", "The addressed JID or item requested cannot be found.");
		break;
	case Jreen::Error::JidMalformed:
		errorText = QCoreApplication::translate("Jreen::Error", "The sending entity has provided or communicated an XMPP address or aspect thereof that does not adhere to the syntax defined in Addressing Scheme.");
		break;
	case Jreen::Error::NotAcceptable:
		errorText = QCoreApplication::translate("Jreen::Error", "The recipient or server understands the request but is refusing to process it because it does not meet criteria defined by the recipient or server.");
		break;
	case Jreen::Error::NotAllowed:
		errorText = QCoreApplication::translate("Jreen::Error", "The recipient or server does not allow any entity to perform the action.");
		break;
	case Jreen::Error::NotAuthorized:
		errorText = QCoreApplication::translate("Jreen::Error", "The sender must provide proper credentials before being allowed to perform the action, or has provided impreoper credentials.");
		break;
	case Jreen::Error::NotModified:
		errorText = QCoreApplication::translate("Jreen::Error", "The item requested has not changed since it was last requested.");
		break;
	case Jreen::Error::PaymentRequired:
		errorText = QCoreApplication::translate("Jreen::Error", "The requesting entity is not authorized to access the requested service because payment is required.");
		break;
	case Jreen::Error::RecipientUnavailable:
		errorText = QCoreApplication::translate("Jreen::Error", "The intended recipient is temporarily unavailable.");
		break;
	case Jreen::Error::Redirect:
		errorText = QCoreApplication::translate("Jreen::Error", "The recipient or server is redirecting requests for this information to another entity, usually temporarily.");
		break;
	case Jreen::Error::RegistrationRequired:
		errorText = QCoreApplication::translate("Jreen::Error", "The requesting entity is not authorized to access the requested service because registration is required.");
		break;
	case Jreen::Error::RemoteServerNotFound:
		errorText = QCoreApplication::translate("Jreen::Error", "A remote server or service specified as part or all of the JID of the intended recipient does not exist.");
		break;
	case Jreen::Error::RemoteServerTimeout:
		errorText = QCoreApplication::translate("Jreen::Error", "A remote server or service specified as part or all of the JID of the intended recipient could not be contacted within a reasonable amount of time.");
		break;
	case Jreen::Error::ResourceConstraint:
		errorText = QCoreApplication::translate("Jreen::Error", "The server or recipient lacks the system resources necessary to service the request.");
		break;
	case Jreen::Error::ServiceUnavailable:
		errorText = QCoreApplication::translate("Jreen::Error", "The server or recipient does not currently provide the requested service.");
		break;
	case Jreen::Error::SubscriptionRequired:
		errorText = QCoreApplication::translate("Jreen::Error", "The requesting entity is not authorized to access the requested service because a subscription is required.");
		break;
	case Jreen::Error::UndefinedCondition:
		errorText = QCoreApplication::translate("Jreen::Error", "The unknown error condition.");
		break;
	case Jreen::Error::UnexpectedRequest:
		errorText = QCoreApplication::translate("Jreen::Error", "The recipient or server understood the request but was not expecting it at this time.");
		break;
	case Jreen::Error::UnknownSender:
		errorText = QCoreApplication::translate("Jreen::Error", "The stanza 'from' address specified by a connected client is not valid for the stream.");
		break;
	case Jreen::Error::Undefined:
		errorText = QCoreApplication::translate("Jreen::Error", "No stanza error occured. You're just sleeping.");
		break;
	}
	return errorText;
}

}
