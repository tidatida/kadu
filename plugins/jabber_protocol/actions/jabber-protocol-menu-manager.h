/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "protocols/protocol-menu-manager.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class JabberActions;

class JabberProtocolMenuManager : public QObject, public ProtocolMenuManager
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit JabberProtocolMenuManager(QObject *parent = nullptr);
	virtual ~JabberProtocolMenuManager();

	virtual const QList<ActionDescription *> & protocolActions() const;
	virtual const QString protocolName() const { return "jabber"; }

private:
	QPointer<JabberActions> m_jabberActions;

	mutable QList<ActionDescription *> m_rosterActions;

private slots:
	INJEQT_SET void setJabberActions(JabberActions *jabberActions);

};
