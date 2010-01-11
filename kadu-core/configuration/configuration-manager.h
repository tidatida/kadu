/*
 * %kadu copyright begin%
 * Copyright 2009 Juzef (juzefwt@tlen.pl)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef CONFIGURATION_MANAGER
#define CONFIGURATION_MANAGER

#include <QtCore/QList>
#include <QtCore/QUuid>

#include "exports.h"

class StorableObject;

class KADUAPI ConfigurationManager
{
	static ConfigurationManager *Instance;

	QUuid Uuid;
	QList<StorableObject *> RegisteredStorableObjects;

	ConfigurationManager();

	void importConfiguration();
	void copyOldContactsToImport();
	void importOldContact(); // TODO: remove after 0.6.6 release
	void importContactsIntoBuddies(); // TODO: remove after 0.6.6 release
	void importContactAccountDatasIntoContacts(); // TODO: remove after 0.6.6 release

public:
	static ConfigurationManager * instance();

	void load();
	void store();
	void flush();

	QUuid uuid() { return Uuid; }

	void registerStorableObject(StorableObject *object);
	void unregisterStorableObject(StorableObject *object);

};

#endif // CONFIGURATION_MANAGER
