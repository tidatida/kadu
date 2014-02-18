/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message/message.h"
#include "protocols/services/chat-state-service.h"

#include <QtCore/QVector>

class ChatImage;
class MessageRenderInfo;

class ChatMessagesRenderer : public QObject
{
	Q_OBJECT

public:
	explicit ChatMessagesRenderer(QObject *parent = nullptr);
	virtual ~ChatMessagesRenderer();

	virtual void initialize() = 0;
	virtual void clearMessages() = 0;
	virtual void appendChatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo) = 0;
	virtual void paintMessages(const QVector<Message> &messages) = 0;
	virtual void removeFirstMessage() = 0;
	virtual void refreshView(const QVector<Message> &allMessages, bool useTransparency = false) = 0;
	virtual void messageStatusChanged(Message, MessageStatus) = 0;
	virtual void contactActivityChanged(ChatStateService::State, const QString &, const QString &) = 0;
	virtual void chatImageAvailable(const ChatImage &chatImage, const QString &fileName) = 0;

};
