/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contacts/contact.h"
#include "exports.h"

#include <QtCore/QObject>

class RosterService;

/**
 * @addtogroup Roster
 * @{
 */

/**
 * @class Roster
 * @short Generic interface to all RosterService objects
 *
 * This class allows adding, removing and updating contacts on remote roster. Adding contacts is done by addContact() method,
 * removing - by removeContact() method. This class searchs for suitable RosterService and uses it to perform its job.
 *
 * Use this class to manipulate rosters. After adding a contact to any roster, its data is updated automatically, until
 * it is removed.
 */
class KADUAPI Roster : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit Roster(QObject *parent = nullptr);
	virtual ~Roster();

	/**
	 * @return RosterService instance suitable to given @p contact
	 */
	static RosterService * rosterService(const Contact &contact);

public slots:
	/**
	 * @short Add new @p contact to remote roster.
	 *
	 * Roster state of newly added contact is set to HasLocalChanges (if is not already Detached)
	 * so it will be uploaded to server as soon as possible.
	 */
	void addContact(const Contact &contact) const;

	/**
	 * @short Remove @p contact from remote roster.
	 */
	void removeContact(const Contact &contact) const;

};

/**
 * @}
 */
