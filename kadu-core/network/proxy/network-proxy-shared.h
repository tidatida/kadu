/*
 * %kadu copyright begin%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "storage/shared.h"
#include "exports.h"

#include <QtNetwork/QHostAddress>
#include <injeqt/injeqt.h>

class NetworkProxyManager;

class KADUAPI NetworkProxyShared : public Shared
{
	Q_OBJECT

public:
	explicit NetworkProxyShared(const QUuid &uuid = QUuid());
	virtual ~NetworkProxyShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	KaduShared_Property(const QString &, type, Type)
	KaduShared_Property(const QString &, address, Address)
	KaduShared_Property(int, port, Port)
	KaduShared_Property(const QString &, user, User)
	KaduShared_Property(const QString &, password, Password)
	KaduShared_Property(const QString &, pollingUrl, PollingUrl)

	QString displayName();

signals:
	void updated();

protected:
	virtual void load();
	virtual void store();

private:
	QPointer<NetworkProxyManager> m_networkProxyManager;

	QString Type;
	QString Address;
	int Port;
	QString User;
	QString Password;
	QString PollingUrl;

private slots:
	INJEQT_SET void setNetworkProxyManager(NetworkProxyManager *networkProxyManager);

};
