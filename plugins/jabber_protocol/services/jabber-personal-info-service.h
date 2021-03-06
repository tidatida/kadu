/*
 * %kadu copyright begin%
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

#include "protocols/services/personal-info-service.h"

#include "buddies/buddy.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyStorage;
class JabberVCardService;

class QXmppVCardIq;

class JabberPersonalInfoService : public PersonalInfoService
{
	Q_OBJECT

public:
	explicit JabberPersonalInfoService(Account account, QObject *parent = nullptr);
	virtual ~JabberPersonalInfoService();

	void setVCardService(JabberVCardService *vCardService);

	virtual void fetchPersonalInfo(const QString &id);
	virtual void updatePersonalInfo(const QString &id, Buddy buddy);

private:
	QPointer<BuddyStorage> m_buddyStorage;
	QPointer<JabberVCardService> VCardService;
	Buddy CurrentBuddy;

private slots:
	INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);

	virtual void vCardDownloaded(bool ok, const QXmppVCardIq &vCard);

};
