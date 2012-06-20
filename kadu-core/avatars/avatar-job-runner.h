/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef AVATAR_JOB_RUNNER_H
#define AVATAR_JOB_RUNNER_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

#include "contacts/contact.h"

#include "exports.h"

class QTimer;

class Account;
class AvatarService;

class KADUAPI AvatarJobRunner : public QObject
{
	Q_OBJECT

	Contact MyContact;

	QTimer *Timer;

	AvatarService * avatarService(const Account &account);
	AvatarService * avatarService(const Contact &contact);

private slots:
	void timeout();

public:
	explicit AvatarJobRunner(const Contact &contact, QObject *parent);
	virtual ~AvatarJobRunner();

	void runJob();

public slots:
	void avatarFetched(bool ok, QPixmap avatar);

signals:
	void jobFinished(bool ok);

};

#endif // AVATAR_JOB_RUNNER_H
