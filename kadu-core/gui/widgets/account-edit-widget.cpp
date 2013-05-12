/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/composite-configuration-value-state-notifier.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"

#include "account-configuration-widget-factory-repository.h"

#include "account-edit-widget.h"
#include "account-configuration-widget-factory.h"

AccountEditWidget::AccountEditWidget(AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository, Account account, QWidget *parent) :
		AccountConfigurationWidget(account, parent), MyAccountConfigurationWidgetFactoryRepository(accountConfigurationWidgetFactoryRepository),
		StateNotifier(new SimpleConfigurationValueStateNotifier(this)),
		CompositeStateNotifier(new CompositeConfigurationValueStateNotifier(this))
{
	if (MyAccountConfigurationWidgetFactoryRepository)
	{
		connect(MyAccountConfigurationWidgetFactoryRepository, SIGNAL(factoryRegistered(AccountConfigurationWidgetFactory*)),
				this, SLOT(factoryRegistered(AccountConfigurationWidgetFactory*)));
		connect(MyAccountConfigurationWidgetFactoryRepository, SIGNAL(factoryUnregistered(AccountConfigurationWidgetFactory*)),
				this, SLOT(factoryUnregistered(AccountConfigurationWidgetFactory*)));

		foreach (AccountConfigurationWidgetFactory *factory, MyAccountConfigurationWidgetFactoryRepository->factories())
			factoryRegistered(factory);
	}

	compositeStateNotifier()->addConfigurationValueStateNotifier(StateNotifier);
}

AccountEditWidget::~AccountEditWidget()
{
}

AccountConfigurationWidgetFactoryRepository * AccountEditWidget::accountConfigurationWidgetFactoryRepository() const
{
	return MyAccountConfigurationWidgetFactoryRepository;
}

void AccountEditWidget::factoryRegistered(AccountConfigurationWidgetFactory *factory)
{
	AccountConfigurationWidget *widget = factory->createWidget(account(), this);
	if (widget)
	{
		AccountConfigurationWidgets.insert(factory, widget);
		emit widgetAdded(widget);
	}
}

void AccountEditWidget::factoryUnregistered(AccountConfigurationWidgetFactory *factory)
{
	if (AccountConfigurationWidgets.contains(factory))
	{
		AccountConfigurationWidget *widget = AccountConfigurationWidgets.value(factory);
		emit widgetRemoved(widget);
		widget->deleteLater();
	}
}

QList<AccountConfigurationWidget *> AccountEditWidget::accountConfigurationWidgets() const
{
	return AccountConfigurationWidgets.values();
}

void AccountEditWidget::applyAccountConfigurationWidgets()
{
	foreach (AccountConfigurationWidget *widget, AccountConfigurationWidgets)
		widget->apply();
}

void AccountEditWidget::cancelAccountConfigurationWidgets()
{
	foreach (AccountConfigurationWidget *widget, AccountConfigurationWidgets)
		widget->cancel();
}

ConfigurationValueState AccountEditWidget::accountConfigurationWidgetsState()
{
	bool anyChanged = false;

	foreach (AccountConfigurationWidget *widget, AccountConfigurationWidgets)
	{
		if (widget->stateNotifier())
		{
			switch (widget->stateNotifier()->state())
			{
				case StateChangedDataInvalid:
					return StateChangedDataInvalid;
				case StateNotChanged:
					break;
				case StateChangedDataValid:
					anyChanged = true;
					break;
			}
		}
	}

	if (anyChanged)
		return StateChangedDataValid;
	else
		return StateNotChanged;
}

SimpleConfigurationValueStateNotifier * AccountEditWidget::simpleStateNotifier() const
{
	return StateNotifier;
}

CompositeConfigurationValueStateNotifier * AccountEditWidget::compositeStateNotifier() const
{
	return CompositeStateNotifier;
}

const ConfigurationValueStateNotifier * AccountEditWidget::stateNotifier() const
{
	return CompositeStateNotifier;
}
