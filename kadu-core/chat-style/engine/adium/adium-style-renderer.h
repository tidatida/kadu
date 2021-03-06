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

#pragma once

#include "chat-style/engine/chat-style-renderer.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AdiumStyle;
class ChatConfigurationHolder;
class IconsManager;
class MessageHtmlRendererService;
class SystemInfo;

class AdiumStyleRenderer : public ChatStyleRenderer
{
	Q_OBJECT

public:
	explicit AdiumStyleRenderer(ChatStyleRendererConfiguration configuration, std::shared_ptr<AdiumStyle> style, QObject *parent = nullptr);
	virtual ~AdiumStyleRenderer();

	virtual void clearMessages() override;
	virtual void appendChatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo) override;
	virtual void removeFirstMessage() override;
	virtual void displayMessageStatus(const QString &id, MessageStatus) override;
	virtual void displayChatState(ChatState, const QString &, const QString &) override;
	virtual void displayChatImage(const ChatImage &chatImage, const QString &fileName) override;

private:
	QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
	QPointer<IconsManager> m_iconsManager;
	QPointer<MessageHtmlRendererService> m_messageHtmlRendererService;
	QPointer<SystemInfo> m_systemInfo;

	std::shared_ptr<AdiumStyle> m_style;

	QString replaceKeywords(const QString &styleHref, const QString &style);
	QString replaceKeywords(const QString &styleHref, const QString &source, const Message &message, const QString &nickColor);
	QString preprocessStyleBaseHtml(bool useTransparency);

private slots:
	INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService);
	INJEQT_SET void setSystemInfo(SystemInfo *systemInfo);
	INJEQT_INIT void init();

	void pageLoaded();

};
