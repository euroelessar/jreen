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

#ifndef ERROR_H
#define ERROR_H

#include "stanzaextension.h"
#include "langmap.h"

namespace Jreen
{

class ErrorPrivate;
class JREEN_EXPORT Error : public Payload
{
	J_PAYLOAD(Jreen::Error)
	Q_DECLARE_PRIVATE(Error)
public:
	enum Type
	{
		Auth,
		Cancel,
		Continue,
		Modify,
		Wait,
		UndefinedType
	};
	enum Condition
	{
		BadRequest,          /**< The sender has sent XML that is malformed or that cannot be
		* processed (e.g., an IQ stanza that includes an unrecognized value
		* of the 'type' attribute); the associated error type SHOULD be
		* "modify". */
		Conflict,            /**< Access cannot be granted because an existing resource or session
		* exists with the same name or address; the associated error type
		* SHOULD be "cancel". */
		FeatureNotImplemented,/**< The feature requested is not implemented by the recipient or
		* server and therefore cannot be processed; the associated error
		* type SHOULD be "cancel". */
		Forbidden,           /**< The requesting entity does not possess the required permissions to
		* perform the action; the associated error type SHOULD be "auth". */
		Gone,                /**< The recipient or server can no longer be contacted at this address
		* (the error stanza MAY contain a new address in the XML character data
		* of the &lt;gone/&gt; element); the associated error type SHOULD be
		* "modify". */
		InternalServerError, /**< The server could not process the stanza because of a
		* misconfiguration or an otherwise-undefined internal server error; the
		* associated error type SHOULD be "wait". */
		ItemNotFound,        /**< The addressed JID or item requested cannot be found; the associated
		* error type SHOULD be "cancel". */
		JidMalformed,        /**< The sending entity has provided or communicated an XMPP address
		* (e.g., a value of the 'to' attribute) or aspect thereof (e.g., a
		* resource identifier) that does not adhere to the syntax defined in
		* Addressing Scheme (Section 3); the associated error type SHOULD be
		* "modify". */
		NotAcceptable,       /**< The recipient or server understands the request but is refusing to
		* process it because it does not meet criteria defined by the recipient
		* or server (e.g., a local policy regarding acceptable words in
		* messages); the associated error type SHOULD be "modify". */
		NotAllowed,          /**< The recipient or server does not allow any entity to perform the
		* action; the associated error type SHOULD be "cancel". */
		NotAuthorized,       /**< The sender must provide proper credentials before being allowed to
		* perform the action, or has provided impreoper credentials; the
		* associated error type should be "auth". */
		NotModified,         /**< The item requested has not changed since it was last requested;
		* the associated error type SHOULD be "continue". */
		PaymentRequired,     /**< The requesting entity is not authorized to access the requested
		* service because payment is required; the associated error type SHOULD
		* be "auth". */
		RecipientUnavailable,/**< The intended recipient is temporarily unavailable; the associated
		* error type SHOULD be "wait" (note: an application MUST NOT return
		* this error if doing so would provide information about the intended
		* recipient's network availability to an entity that is not authorized
		* to know such information). */
		Redirect,            /**< The recipient or server is redirecting requests for this
		* information to another entity, usually temporarily (the error
		* stanza SHOULD contain the alternate address, which MUST be a valid
		* JID, in the XML character data of the &lt;redirect/&gt; element);
		* the associated error type SHOULD be "modify". */
		RegistrationRequired,/**< The requesting entity is not authorized to access the requested
		* service because registration is required; the associated error type
		* SHOULD be "auth". */
		RemoteServerNotFound,/**< A remote server or service specified as part or all of the JID of
		* the intended recipient does not exist; the associated error type
		* SHOULD be "cancel". */
		RemoteServerTimeout, /**< A remote server or service specified as part or all of the JID of
		* the intended recipient (or required to fulfill a request) could not
		* be contacted within a reasonable amount of time; the associated error
		* type SHOULD be "wait". */
		ResourceConstraint,  /**< The server or recipient lacks the system resources necessary to
		* service the request; the associated error type SHOULD be "wait". */
		ServiceUnavailable,  /**< The server or recipient does not currently provide the requested
		* service; the associated error type SHOULD be "cancel". */
		SubscriptionRequired,/**< The requesting entity is not authorized to access the requested
		* service because a subscription is required; the associated error type
		* SHOULD be "auth". */
		UndefinedCondition,  /**< The error condition is not one of those defined by the other
		* conditions in this list; any error type may be associated with this
		* condition, and it SHOULD be used only in conjunction with an
		* application-specific condition. */
		UnexpectedRequest,   /**< The recipient or server understood the request but was not
		* expecting it at this time (e.g., the request was out of order);
		* the associated error type SHOULD be "wait". */
		UnknownSender,       /**< The stanza 'from' address specified by a connected client is not
		* valid for the stream (e.g., the stanza does not include a 'from'
		* address when multiple resources are bound to the stream); the
		* associated error type SHOULD be "modify".*/
		Undefined            /**< No stanza error occured. */
	};
	//	enum Condition
	//	{
	//		BadFormat,
	//		BadNamespacePrefix,
	//		Conflict,
	//		ConnectionTimeout,
	//		HostGone,
	//		HostUnknown,
	//		ImproperAddressing,
	//		InternalServerError,
	//		InvalidFrom,
	//		InvalidId,
	//		InvalidNamespace,
	//		InvalidXml,
	//		NotAuthorized,
	//		PolicyViolation,
	//		RemoteConnectionFailed,
	//		ResourceConstraint,
	//		RestrictedXml,
	//		SeeOtherHost,
	//		SystemShutdown,
	//		UndefinedCondition,
	//		UnsupportedEncoding,
	//		UnsupportedStanzaType,
	//		UnsupportedVersion,
	//		XmlNotWellFormed,
	//		Undefined
	//	};
	Error(Type type, Condition condition, const LangMap &langMap = LangMap());
	~Error();

	Type type() const;
	QString text(const QString &lang = QString()) const;
	Condition condition() const;
	QString conditionText() const;
private:
	QScopedPointer<ErrorPrivate> d_ptr;
};

}

#endif // ERROR_H
