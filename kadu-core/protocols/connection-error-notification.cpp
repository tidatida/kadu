/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "icons/icons-manager.h"
#include "identities/identity.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "parser/parser.h"
#include "debug.h"

#include "connection-error-notification.h"

NotifyEvent *ConnectionErrorNotification::ConnectionErrorNotifyEvent = 0;
QMap<Account, QStringList> ConnectionErrorNotification::ActiveErrors;

static QString getErrorMessage(const QObject * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = qobject_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorMessage();
	else
		return QString();
}

static QString getErrorServer(const QObject * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = qobject_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorServer();
	else
		return QString();
}

void ConnectionErrorNotification::registerEvent()
{
	if (ConnectionErrorNotifyEvent)
		return;

	ConnectionErrorNotifyEvent = new NotifyEvent("ConnectionError", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Connection error"));
	NotificationManager::instance()->registerNotifyEvent(ConnectionErrorNotifyEvent);
	Parser::registerObjectTag("error", getErrorMessage);
	Parser::registerObjectTag("errorServer", getErrorServer);
}

void ConnectionErrorNotification::unregisterEvent()
{
	if (!ConnectionErrorNotifyEvent)
		return;

	Parser::unregisterObjectTag("errorServer");
	Parser::unregisterObjectTag("error");

	NotificationManager::instance()->unregisterNotifyEvent(ConnectionErrorNotifyEvent);
	delete ConnectionErrorNotifyEvent;
	ConnectionErrorNotifyEvent = 0;
}

bool ConnectionErrorNotification::activeError(Account account, const QString &errorMessage)
{
	return ActiveErrors.value(account).contains(errorMessage);
}

ConnectionErrorNotification::ConnectionErrorNotification(Account account, const QString &errorServer, const QString &errorMessage) :
		AccountNotification(account, "ConnectionError", KaduIcon("dialog-error")),
		ErrorServer(errorServer), ErrorMessage(errorMessage)
{
	setTitle(tr("Connection error"));
	setText(tr("Connection error on account: %1 (%2)").arg(account.id()).arg(account.accountIdentity().name()));

	if (!ErrorMessage.isEmpty())
	{
		if (ErrorServer.isEmpty())
			setDetails(ErrorMessage);
		else
			setDetails(QString("%1 (%2)").arg(ErrorMessage).arg(ErrorServer));
	}

	ActiveErrors[account].append(ErrorMessage);
}

ConnectionErrorNotification::~ConnectionErrorNotification()
{
	QStringList &list = ActiveErrors[account()];
	list.removeOne(ErrorMessage);
	if (list.isEmpty())
		ActiveErrors.remove(account());
}

QString ConnectionErrorNotification::errorMessage() const
{
	kdebugf();

	return ErrorMessage;
}

QString ConnectionErrorNotification::errorServer() const
{
	kdebugf();

	return ErrorServer;
}
