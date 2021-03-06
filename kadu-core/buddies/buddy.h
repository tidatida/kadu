/*
 * %kadu copyright begin%
 * Copyright 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_H
#define BUDDY_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "buddies/buddy-gender.h"
#include "buddies/buddy-shared.h"
#include "storage/shared-base.h"
#include "exports.h"

class Account;
class Avatar;
class BuddyShared;
class Contact;
class ContactShared;
class Group;
class StoragePoint;
class ConfigurationApi;

class KADUAPI Buddy : public SharedBase<BuddyShared>
{
	KaduSharedBaseClass(Buddy)

	friend class BuddyDummyFactory;
	friend class ContactShared;
	// only allow ContactShared and BuddyDummyFactory to access these methods
	void addContact(Contact contact);
	void removeContact(Contact contact) const;

public:
	static Buddy null;

	Buddy();
	Buddy(BuddyShared *data);
	explicit Buddy(QObject *data);
	Buddy(const Buddy &copy);
	virtual ~Buddy();

	void mergeWith(Buddy buddy); // TODO: 0.10 refactor

	void importConfiguration(const QDomElement &parent);
	void importConfiguration(); // import configuration from customDataValues

	QString customData(const QString &key) const;
	void setCustomData(const QString &key, const QString &value);
	void removeCustomData(const QString &key);

	void sortContacts();
	void normalizePriorities();

// 	void setData(BuddyShared *data) { Data = data; }  // TODO: 0.10 tricky merge, this should work well ;)

	QVector<Contact> contacts(Account account) const;
	QList<Contact> contacts() const;
	bool hasContact(Account account) const;

	QString id(Account account) const;
	// properties

	bool showInAllGroup() const;
	bool isInGroup(Group group) const;
	void addToGroup(Group group) const;
	void removeFromGroup(Group group) const;

	bool isEmpty(bool checkOnlyForContacts = false) const;

	QString display() const;

	KaduSharedBase_PropertyCRW(Avatar, buddyAvatar, BuddyAvatar)
	KaduSharedBase_PropertyWrite(const QString &, display, Display)
	KaduSharedBase_PropertyCRW(QString, firstName, FirstName)
	KaduSharedBase_PropertyCRW(QString, lastName, LastName)
	KaduSharedBase_PropertyCRW(QString, familyName, FamilyName)
	KaduSharedBase_PropertyCRW(QString, city, City)
	KaduSharedBase_PropertyCRW(QString, familyCity, FamilyCity)
	KaduSharedBase_PropertyCRW(QString, nickName, NickName)
	KaduSharedBase_PropertyCRW(QString, homePhone, HomePhone)
	KaduSharedBase_PropertyCRW(QString, mobile, Mobile)
	KaduSharedBase_PropertyCRW(QString, email, Email)
	KaduSharedBase_PropertyCRW(QString, website, Website)
	KaduSharedBase_Property(unsigned short, birthYear, BirthYear)
	KaduSharedBase_Property(BuddyGender, gender, Gender)
	KaduSharedBase_PropertyCRW(QSet<Group>, groups, Groups)
	KaduSharedBase_Property(bool, preferHigherStatuses, PreferHigherStatuses)
	KaduSharedBase_PropertyBool(Anonymous)
	KaduSharedBase_PropertyBool(Temporary)
	KaduSharedBase_PropertyBool(Ignored)
	KaduSharedBase_PropertyBool(Blocked)
	KaduSharedBase_PropertyBool(OfflineTo)
	KaduSharedBase_PropertyRead(quint16, unreadMessagesCount, UnreadMessagesCount)

};

Q_DECLARE_METATYPE(Buddy)

#endif // BUDDY_H
