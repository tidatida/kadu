/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIGURATION_VALUE_STATE_NOTIFIER_H
#define CONFIGURATION_VALUE_STATE_NOTIFIER_H

#include <QtCore/QObject>
#include "exports.h"

enum ConfigurationValueState
{
	StateNotChanged,
	StateChangedDataValid,
	StateChangedDataInvalid
};

class KADUAPI ConfigurationValueStateNotifier : public QObject
{
	Q_OBJECT

public:
	explicit ConfigurationValueStateNotifier(QObject *parent = nullptr);
	virtual ~ConfigurationValueStateNotifier();

	virtual ConfigurationValueState state() const = 0;

signals:
	void stateChanged(ConfigurationValueState state);

};

#endif // CONFIGURATION_VALUE_STATE_NOTIFIER_H
