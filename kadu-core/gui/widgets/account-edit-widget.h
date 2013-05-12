/*
 * %kadu copyright begin%
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACCOUNT_EDIT_WIDGET_H
#define ACCOUNT_EDIT_WIDGET_H

#include <QtGui/QWidget>

#include "accounts/account.h"
#include "gui/widgets/account-configuration-widget.h"
#include "exports.h"

class AccountConfigurationWidget;
class AccountConfigurationWidgetFactory;
class AccountConfigurationWidgetFactoryRepository;

class KADUAPI AccountEditWidget : public AccountConfigurationWidget
{
	Q_OBJECT

	AccountConfigurationWidgetFactoryRepository *MyAccountConfigurationWidgetFactoryRepository;
	QMap<AccountConfigurationWidgetFactory *, AccountConfigurationWidget *> AccountConfigurationWidgets;

private slots:
	void factoryRegistered(AccountConfigurationWidgetFactory *factory);
	void factoryUnregistered(AccountConfigurationWidgetFactory *factory);

protected:
	AccountConfigurationWidgetFactoryRepository * accountConfigurationWidgetFactoryRepository() const;

	void applyAccountConfigurationWidgets();
	void cancelAccountConfigurationWidgets();

	ConfigurationValueState accountConfigurationWidgetsState();

public:
	explicit AccountEditWidget(AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository,
							   Account account, QWidget *parent = 0);
	virtual ~AccountEditWidget();

	QList<AccountConfigurationWidget *> accountConfigurationWidgets() const;

signals:
	void widgetAdded(AccountConfigurationWidget *widget);
	void widgetRemoved(AccountConfigurationWidget *widget);

};

#endif // ACCOUNT_EDIT_WIDGET_H
