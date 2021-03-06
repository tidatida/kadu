/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ACCOUNT_CREATE_WIDGET_H
#define ACCOUNT_CREATE_WIDGET_H

#include "accounts/account.h"
#include "gui/widgets/modal-configuration-widget.h"

#include "exports.h"

class SimpleConfigurationValueStateNotifier;

class KADUAPI AccountCreateWidget : public ModalConfigurationWidget
{
	Q_OBJECT

	SimpleConfigurationValueStateNotifier *StateNotifier;

protected:
	SimpleConfigurationValueStateNotifier * simpleStateNotifier() const;

public:
	explicit AccountCreateWidget(QWidget *parent = nullptr);
	virtual ~AccountCreateWidget();

	virtual const ConfigurationValueStateNotifier * stateNotifier() const;

signals:
	void accountCreated(Account account);

};

#endif // ACCOUNT_CREATE_WIDGET_H
