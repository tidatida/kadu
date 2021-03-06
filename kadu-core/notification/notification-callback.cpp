/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notification-callback.h"

#include "notification/notification.h"

NotificationCallback::NotificationCallback()
{
}

NotificationCallback::NotificationCallback(QString name, QString title, std::function<void(const Notification &)> callback) :
		m_name{std::move(name)},
		m_title{std::move(title)},
		m_callback{std::move(callback)}
{
}

QString NotificationCallback::name() const
{
	return m_name;
}

QString NotificationCallback::title() const
{
	return m_title;
}

std::function<void(const Notification &)> NotificationCallback::callback() const
{
	return m_callback;
}

void NotificationCallback::call(const Notification &notification) const
{
	if (m_callback)
		m_callback(notification);
}

bool operator == (const NotificationCallback &x, const NotificationCallback &y)
{
	if (x.name() != y.name())
		return false;
	if (x.title() != y.title())
		return false;
	return true;
}
