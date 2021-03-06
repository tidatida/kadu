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

#include "message-module.h"

#include "message/message-filter-service.h"
#include "message/message-html-renderer-service.h"
#include "message/message-manager-impl.h"
#include "message/message-notification-service.h"
#include "message/message-render-info-factory.h"
#include "message/message-storage.h"
#include "message/unread-message-repository.h"
#include "services/message-transformer-service.h"

MessageModule::MessageModule()
{
	add_type<MessageFilterService>();
	add_type<MessageHtmlRendererService>();
	add_type<MessageManagerImpl>();
	add_type<MessageNotificationService>();
	add_type<MessageRenderInfoFactory>();
	add_type<MessageStorage>();
	add_type<MessageTransformerService>();
	add_type<UnreadMessageRepository>();
}
