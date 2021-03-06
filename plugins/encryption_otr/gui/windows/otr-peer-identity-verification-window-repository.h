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

#include <QtCore/QObject>
#include <injeqt/injeqt.h>

class OtrPeerIdentityVerificationWindow;
class OtrPeerIdentityVerificationWindowFactory;

class OtrPeerIdentityVerificationWindowRepository : public QObject
{
	Q_OBJECT

	QPointer<OtrPeerIdentityVerificationWindowFactory> PeerIdentityVerificationWindowFactory;
	QMap<Contact, OtrPeerIdentityVerificationWindow *> Windows;

private slots:
	void windowDestroyed(const Contact &contact);

public:
	Q_INVOKABLE OtrPeerIdentityVerificationWindowRepository();
	virtual ~OtrPeerIdentityVerificationWindowRepository();

	OtrPeerIdentityVerificationWindow * windowForContact(const Contact &contact);

public slots:
	void showRespondQuestionAndAnswerVerificationWindow(const Contact &contact, const QString &question);
	void showRespondSharedSecretVerificationWindow(const Contact &contact);
	void showVerificationWindow(const Contact &contact);

private slots:
	INJEQT_SET void setPeerIdentityVerificationWindowFactory(OtrPeerIdentityVerificationWindowFactory *peerIdentityVerificationWindowFactory);

};
