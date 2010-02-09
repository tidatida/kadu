/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "buddy-contact-model.h"

BuddyContactModel::BuddyContactModel(Buddy buddy, QObject *parent) :
		QAbstractListModel(parent), SourceBuddy(buddy)
{
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeAdded(Contact)),
			this, SLOT(contactAboutToBeAdded(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactRemoved(Contact)),
			this, SLOT(contactRemoved(Contact)));
}

BuddyContactModel::~BuddyContactModel()
{
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeAdded(Contact)),
			this, SLOT(contactAboutToBeAdded(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactRemoved(Contact)),
			this, SLOT(contactRemoved(Contact)));
}

int BuddyContactModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 2;
}

int BuddyContactModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return SourceBuddy.contacts().count();
}

QVariant BuddyContactModel::data(const QModelIndex &index, int role) const
{
	Contact data = contact(index);
	if (data.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			if (index.column() == 0) // long or shor name?
				return data.id();
			else
				return QString("%1 (%2)").arg(data.id(), data.contactAccount().name());

		case Qt::DecorationRole:
			return data.contactAccount().protocolHandler()
				? data.contactAccount().protocolHandler()->icon()
				: QIcon();

		case ContactRole:
			return QVariant::fromValue<Contact>(data);

		default:
			return QVariant();
	}
}

QVariant BuddyContactModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

Contact BuddyContactModel::contact(const QModelIndex &index) const
{
	if (!index.isValid())
		return Contact::null;

	if (index.row() < 0 || index.row() >= rowCount())
		return Contact::null;

	return SourceBuddy.contacts().at(index.row());
}

int BuddyContactModel::contactIndex(Contact data)
{
	return SourceBuddy.contacts().indexOf(data);
}

QModelIndex BuddyContactModel::contactModelIndex(Contact data)
{
	return createIndex(contactIndex(data), 0, 0);
}

void BuddyContactModel::contactAboutToBeAdded(Contact data)
{
	Q_UNUSED(data)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void BuddyContactModel::contactAdded(Contact data)
{
	Q_UNUSED(data)

	endInsertRows();
}

void BuddyContactModel::contactAboutToBeRemoved(Contact data)
{
	int index = contactIndex(data);
	beginRemoveRows(QModelIndex(), index, index);
}

void BuddyContactModel::contactRemoved(Contact data)
{
	Q_UNUSED(data)

	endRemoveRows();
}
