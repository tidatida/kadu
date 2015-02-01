/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "file-transfer/file-transfer.h"
#include "notify/notification/chat-notification.h"

class FileTransferManager;
class NotifyEvent;
enum class FileTransferStartType;

class NewFileTransferNotification : public ChatNotification
{
	Q_OBJECT

public:
	static void registerEvents();
	static void unregisterEvents();

	static void notifyIncomingFileTransfer(const FileTransfer &fileTransfer);

	NewFileTransferNotification(const QString &type, FileTransfer transfer, Chat chat, FileTransferStartType startType);

	virtual bool requireCallback() { return true; }

public slots:
	virtual void callbackAccept();
	virtual void callbackReject();

private:
	static NotifyEvent *m_fileTransferNotifyEvent;
	static NotifyEvent *m_fileTransferIncomingFileNotifyEvent;

	bool _continue;

	FileTransfer _transfer;
	QString _fileName;

private slots:
	void callbackAcceptAsNew();

};
