/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NOTIFY_CONFIGURATION_UI_HANDLER
#define NOTIFY_CONFIGURATION_UI_HANDLER

#include "configuration/gui/configuration-ui-handler.h"
#include "gui/windows/main-configuration-window.h"
#include "notification/notification-event.h"

#include "notification-manager.h"

class QCheckBox;
class QListWidget;
class QWidget;

class NotifierConfigurationWidget;
class NotifyGroupBox;
class NotifyTreeWidget;

struct NotifierConfigurationGuiItem
{
	NotifierConfigurationWidget *ConfigurationWidget;
	NotifyGroupBox *ConfigurationGroupBox;
	QMap<QString, bool> Events;

	NotifierConfigurationGuiItem() : ConfigurationWidget(0), ConfigurationGroupBox(0) {}
};

struct NotificationEventConfigurationItem
{
	NotificationEvent event;
	bool useCustomSettings;
};

class NotifyConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
	Q_OBJECT

	QMap<Notifier *, NotifierConfigurationGuiItem> NotifierGui;
	QMap<QString, NotificationEventConfigurationItem> NotificationEvents;

	QListWidget *allUsers;
	QListWidget *notifiedUsers;
	ConfigGroupBox *notificationsGroupBox;

	QCheckBox *useCustomSettingsCheckBox;
	NotifyTreeWidget *notifyTreeWidget;
	QWidget *notifierMainWidget;
	QVBoxLayout *notifierMainWidgetLayout;

	QString CurrentEvent;

	void addConfigurationWidget(Notifier *notifier);
	void removeConfigurationWidget(Notifier *notifier);

private slots:
	void notifierRegistered(Notifier *notifier);
	void notifierUnregistered(Notifier *notifier);

	void notificationEventAdded(NotificationEvent notifyEvent);
	void notificationEventRemoved(NotificationEvent notifyEvent);

	void moveToNotifyList();
	void moveToAllList();

	void eventSwitched();
	void notifierToggled(Notifier *notifier, bool toggled);

	void customSettingsCheckBoxToggled(bool toggled);

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

public:
	explicit NotifyConfigurationUiHandler(QObject *parent = 0);
	virtual ~NotifyConfigurationUiHandler();

	const QMap<Notifier *, NotifierConfigurationGuiItem> & notifierGui() { return NotifierGui; }
	const QMap<QString, NotificationEventConfigurationItem> & notifyEvents() { return NotificationEvents; }
};

#endif // NOTIFY_CONFIGURATION_UI_HANDLER