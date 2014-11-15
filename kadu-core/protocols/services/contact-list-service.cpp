/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "roster/roster.h"
#include "roster/roster-entry.h"
#include "roster/roster-entry-state.h"
#include "debug.h"

#include "contact-list-service.h"

ContactListService::ContactListService(Account account, QObject *parent) :
		AccountService(account, parent)
{
}

ContactListService::~ContactListService()
{
}

bool ContactListService::askForAddingContacts(const QMap<Buddy, Contact> &contactsToAdd, const QMap<Buddy, Contact> &contactsToRename)
{
	if (contactsToAdd.isEmpty() && contactsToRename.isEmpty())
		return true;

	QString questionString = tr("Kadu since version 0.10.0 automatically synchronizes Gadu-Gadu contact list with server. "
			"Now the first synchronization will be performed.<br/><br/>");

	if (!contactsToAdd.isEmpty())
	{
		QStringList contactsToAddStrings;
		for (QMap<Buddy, Contact>::const_iterator i = contactsToAdd.constBegin(); i != contactsToAdd.constEnd(); i++)
			contactsToAddStrings.append(i.key().display() + " (" + i.value().id() + ')');

		questionString += tr("The following contacts present on the server were not found on your local contact list:<br/>"
				"<b>%1</b>.<br/>If you do not agree to add those contacts to your local list, "
				"they will be removed from the server.<br/><br/>").arg(contactsToAddStrings.join("</b>, <b>"));
	}

	if (!contactsToRename.isEmpty())
	{
		QStringList contactsToRenameStrings;
		for (QMap<Buddy, Contact>::const_iterator i = contactsToRename.constBegin(); i != contactsToRename.constEnd(); i++)
			contactsToRenameStrings.append(i.value().display(true) + " (" + i.value().id() + ") -> " + i.key().display());

		if (contactsToAdd.isEmpty())
			questionString += tr("The following contacts from your local list are present on the server under different names:<br/>"
					"<b>%1</b>.<br/><br/>").arg(contactsToRenameStrings.join("</b>, <b>"));
		else
			questionString += tr("Moreover, the following contacts from your local list are present on the server under different names:<br/>"
					"<b>%1</b>.<br/><br/>").arg(contactsToRenameStrings.join("</b>, <b>"));
	}

	questionString += tr("Do you want to apply the above changes to your local contact list? "
			"Regardless of your choice, it will be sent to the server after making possible changes.");

	MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-question"), tr("Kadu"), questionString);
	dialog->addButton(QMessageBox::Yes, tr("Apply changes"));
	dialog->addButton(QMessageBox::No, tr("Leave contact list unchanged"));

	return dialog->ask();
}

QVector<Contact> ContactListService::performAdds(const QMap<Buddy, Contact> &contactsToAdd)
{
	QVector<Contact> resultContacts;

	for (QMap<Buddy, Contact>::const_iterator i = contactsToAdd.constBegin(); i != contactsToAdd.constEnd(); i++)
	{
		ContactManager::instance()->addItem(i.value());
		i.value().setOwnerBuddy(i.key());
		i.value().rosterEntry()->setState(RosterEntryState::Synchronized);
		resultContacts.append(i.value());

		Roster::instance()->addContact(i.value());
	}

	return resultContacts;
}

void ContactListService::performRenames(const QMap<Buddy, Contact> &contactsToRename)
{
	BuddyList buddiesToRemove;
	for (QMap<Buddy, Contact>::const_iterator i = contactsToRename.constBegin(); i != contactsToRename.constEnd(); i++)
	{
		// do not remove now as theoretically it could be used in next loop run
		buddiesToRemove.append(i.value().ownerBuddy());
		i.value().setOwnerBuddy(i.key());
		i.value().rosterEntry()->setState(RosterEntryState::Synchronized);
	}

	for (auto &&buddy : buddiesToRemove)
		BuddyManager::instance()->removeBuddyIfEmpty(buddy, true);
}

QVector<Contact> ContactListService::registerBuddies(const BuddyList &buddies)
{
	QVector<Contact> resultContacts;
	QMap<Buddy, Contact> contactsToAdd;
	QMap<Buddy, Contact> contactsToRename;
	QMap<Buddy, Buddy> personalInfoSourceBuddies;

	for (auto &&buddy : buddies)
	{
		if (buddy.display().isEmpty())
			buddy.setDisplay(buddy.uuid().toString());

		Buddy targetBuddy;
		for (QMap<Buddy, Buddy>::const_iterator i = personalInfoSourceBuddies.constBegin(); i != personalInfoSourceBuddies.constEnd(); i++)
			if (i.key().display() == buddy.display())
			{
				targetBuddy = i.key();
				break;
			}
		if (!targetBuddy)
			targetBuddy = BuddyManager::instance()->byDisplay(buddy.display(), ActionCreate);
		targetBuddy.setAnonymous(false);

		for (auto &&contact : buddy.contacts(account()))
		{
			Contact knownContact = ContactManager::instance()->byId(account(), contact.id(), ActionReturnNull);
			if (knownContact)
			{
				// do not import dirty removed contacts unless we will be asking the user
				// (We will be asking only if we are migrating from 0.9.x. Remember that
				// all migrated contacts, including those with anynomous buddies, are initially marked dirty.)
				if (!(knownContact.rosterEntry()->requiresSynchronization() && knownContact.isAnonymous() && !haveToAskForAddingContacts()))
				{
					if (knownContact.isAnonymous())
						contactsToAdd.insert(targetBuddy, knownContact);
					else
					{
						resultContacts.append(knownContact);

						if (knownContact.ownerBuddy() != targetBuddy)
							contactsToRename.insert(targetBuddy, knownContact);
						else
							knownContact.rosterEntry()->setState(RosterEntryState::Synchronized);
					}

					personalInfoSourceBuddies.insert(targetBuddy, buddy);
				}
			}
			else
			{
				contactsToAdd.insert(targetBuddy, contact);
				personalInfoSourceBuddies.insert(targetBuddy, buddy);
			}
		}
	}

	if (haveToAskForAddingContacts() && !askForAddingContacts(contactsToAdd, contactsToRename))
		return resultContacts;

	resultContacts += performAdds(contactsToAdd);
	performRenames(contactsToRename);

	for (QMap<Buddy, Buddy>::const_iterator i = personalInfoSourceBuddies.constBegin(); i != personalInfoSourceBuddies.constEnd(); i++)
	{
		if (i.key().contacts().isEmpty())
			continue;

		copySupportedBuddyInformation(i.key(), i.value());
		// sometimes when a new Contact is added from server on login, sorting fails on that Contact
		// TODO 0.10: find out why it happens and fix it _properly_ as it _might_ be a bug in model
		BuddyManager::instance()->addItem(i.key());
	}

	return resultContacts;
}

QList<Contact> ContactListService::setBuddiesList(const BuddyList &buddies)
{
	QList<Contact> unImportedContacts = ContactManager::instance()->contacts(account()).toList();

	for (auto &&myselfContact : Core::instance()->myself().contacts(account()))
		unImportedContacts.removeAll(myselfContact);

	// now buddies = SERVER_CONTACTS, unImportedContacts = ALL_EVER_HAD_LOCALLY_CONTACTS

	QVector<Contact> managedContacts = registerBuddies(buddies);
	for (auto &&contact : managedContacts)
		unImportedContacts.removeAll(contact);

	// now unImportedContacts = ALL_EVER_HAD_LOCALLY_CONTACTS - (SERVER_CONTACTS - LOCAL_DIRTY_REMOVED_CONTACTS)
	// (unless we are importing from 0.9.x)

	QList<Contact>::iterator i = unImportedContacts.begin();
	while (i != unImportedContacts.end())
	{
		if (i->rosterEntry()->requiresSynchronization() || i->isAnonymous())
		{
			// local dirty removed contacts are no longer dirty if they were absent on server
			if (i->rosterEntry()->requiresSynchronization() && i->isAnonymous())
				i->rosterEntry()->setState(RosterEntryState::Synchronized);

			i = unImportedContacts.erase(i);
		}
	}

	// now unImportedContacts = ALL_EVER_HAD_LOCALLY_CONTACTS - (SERVER_CONTACTS - LOCAL_DIRTY_REMOVED_CONTACTS) -
	//                          - LOCAL_REMOVED_CONTACTS - LOCAL_DIRTY_ADDED_CONTACTS =
	//                        = NOT_REMOVED_LOCAL_CONTACTS - SERVER_CONTACTS - LOCAL_DIRTY_ADDED_CONTACTS =
	//                        = NOT_PRESENT_ON_SERVER_BUT_PRESENT_LOCALLY_CONTACTS - LOCAL_DIRTY_ADDED_CONTACTS
	// (unless we are importing from 0.9.x)

	return unImportedContacts;
}

void ContactListService::copySupportedBuddyInformation(const Buddy &destination, const Buddy &source)
{
	destination.setFirstName(source.firstName());
	destination.setLastName(source.lastName());
	destination.setNickName(source.nickName());
	destination.setDisplay(source.display());
	destination.setMobile(source.mobile());
	destination.setGroups(source.groups());
	destination.setEmail(source.email());
	destination.setOfflineTo(source.isOfflineTo());
	destination.setHomePhone(source.homePhone());
}

#include "moc_contact-list-service.cpp"
