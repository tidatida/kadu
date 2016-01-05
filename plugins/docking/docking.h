/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "docking-exports.h"

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtWidgets/QSystemTrayIcon>
#include <injeqt/injeqt.h>

class AttentionService;
class DockingConfigurationProvider;
class DockingMenuActionRepository;
class StatusNotifierItem;

class DOCKINGAPI Docking final : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE Docking(QObject *parent = nullptr);
	virtual ~Docking();

	DockingMenuActionRepository * dockingMenuActionRepository() const;

	void showMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon, int msecs);

signals:
	void messageClicked();

private:
	static Docking *m_instance;

	owned_qptr<DockingConfigurationProvider> m_dockingConfigurationProvider;
	owned_qptr<DockingMenuActionRepository> m_dockingMenuActionRepository;
	owned_qptr<StatusNotifierItem> m_statusNotifierItem;

	void openUnreadMessages();

private slots:
	INJEQT_SET void setAttentionService(AttentionService *attentionService);

	void configurationUpdated();
	void needAttentionChanged(bool needAttention);
	void searchingForTrayPosition(QPoint &point);
	void activateRequested();

};
