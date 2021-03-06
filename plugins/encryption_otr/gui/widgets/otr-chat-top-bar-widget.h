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

#ifndef OTR_CHAT_TOP_BAR_WIDGET_H
#define OTR_CHAT_TOP_BAR_WIDGET_H

#include <QtWidgets/QWidget>

#include "contacts/contact.h"

#include "otr-trust-level-service.h"

class IconsManager;

class QPushButton;

class OtrChatTopBarWidget : public QWidget
{
	Q_OBJECT

	QPointer<IconsManager> m_iconsManager;
	QPointer<OtrTrustLevelService> TrustLevelService;

	Contact MyContact;
	QPushButton *OtrStatusButton;
	QAction *StartAction;
	QAction *EndAction;
	QAction *VerifyAction;

	void createGui();
	OtrTrustLevelService::TrustLevel trustLevel() const;
	QString trustStatusString(OtrTrustLevelService::TrustLevel level) const;

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

	void startSession();
	void endSession();
	void verifyPeerIdentity();
	void trustLevelUpdated();

public:
	explicit OtrChatTopBarWidget(const Contact &contact, QWidget *parent = nullptr);
	virtual ~OtrChatTopBarWidget();

	void setTrustLevelService(OtrTrustLevelService *trustLevelService);

signals:
	void startSession(const Contact &contact);
	void endSession(const Contact &contact);
	void verifyPeerIdentity(const Contact &contact);

};

#endif // OTR_CHAT_TOP_BAR_WIDGET_H
