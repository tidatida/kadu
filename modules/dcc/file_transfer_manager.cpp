/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QKeyEvent>
#include <QtCore/QList>
#include <QtCore/QFile>

#include "chat_manager.h"
#include "config_file.h"
#include "dcc.h"
#include "dcc_socket.h"
#include "debug.h"
#include "file_transfer.h"
#include "file_transfer_notifications.h"
#include "file_transfer_window.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "protocol.h"

#include "../notify/notify.h"

#include "file_transfer_manager.h"

void disableNonDccUles(KaduAction *action)
{
	kdebugf();

	const UserListElements &ules = action->userListElements();

	if (!ules.count())
	{
		action->setEnabled(false);
		return;
	}

	bool dccEnabled = config_file.readBoolEntry("Network", "AllowDCC");
	bool dccKeyEnabled = true;

	if (!dccEnabled)
	{
		action->setEnabled(false);
		return;
	}

	unsigned int myUin = config_file.readUnsignedNumEntry("General", "UIN");
	foreach(const UserListElement &user, ules)
	{
		if (!user.usesProtocol("Gadu") || user.ID("Gadu").toUInt() == myUin)
		{
			action->setEnabled(false);
			return;
		}
	}

	action->setEnabled(true);
}

FileTransferManager::FileTransferManager(QObject *parent, const char *name) : QObject(parent, name),
	fileTransferWindow(0), toggleFileTransferWindowMenuId(0)
{
	kdebugf();

	connect(kadu, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(kaduKeyPressed(QKeyEvent *)));

	sendFileActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "sendFileAction",
		this, SLOT(sendFileActionActivated(QAction *, bool)),
		"SendFile", tr("Send file"), false, QString::null,
		disableNonDccUles
	); 
	sendFileActionDescription->setShortcut("kadu_sendfile");
	UserBox::insertActionDescription(1, sendFileActionDescription);

	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget*)));

	foreach(ChatWidget *it, chat_manager->chats())
		chatCreated(it);

	dcc_manager->addHandler(this);

	fileTransferWindowActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "sendFileWindowAction",
		this, SLOT(toggleFileTransferWindow(QAction *, bool)),
		"SendFileWindow", tr("Toggle transfers window")
	);
	kadu->insertMenuActionDescription(10, fileTransferWindowActionDescription);

	notification_manager->registerEvent("FileTransfer/IncomingFile",  QT_TRANSLATE_NOOP("@default", "An user wants to send you a file"), CallbackRequired);
	notification_manager->registerEvent("FileTransfer/Finished", QT_TRANSLATE_NOOP("@default", "File transfer was finished"), CallbackNotRequired);

	readFromConfig();

	kdebugf2();
}

FileTransferManager::~FileTransferManager()
{
	kdebugf();

	writeToConfig();

 	notification_manager->unregisterEvent("FileTransfer/IncomingFile");
	notification_manager->unregisterEvent("FileTransfer/Finished");

	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));

	UserBox::removeActionDescription(sendFileActionDescription);
	delete sendFileActionDescription;

	dcc_manager->removeHandler(this);

	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget *)));

	foreach(ChatWidget *it, chat_manager->chats())
		chatDestroying(it);

	destroyAll();

	kadu->removeMenuActionDescription(fileTransferWindowActionDescription);
	delete fileTransferWindowActionDescription;

	if (fileTransferWindow)
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		delete fileTransferWindow;
	}

	kdebugf2();
}

void FileTransferManager::readFromConfig()
{
	kdebugf();

	destroyAll();
	QDomElement fts_elem = xml_config_file->findElement(xml_config_file->rootElement(), "FileTransfers");
	if (fts_elem.isNull())
		return;

	QDomNodeList ft_list = fts_elem.elementsByTagName("FileTransfer");
	FileTransfer *ft;
	for (unsigned int i = 0; i < ft_list.count(); i++)
	{
		ft = FileTransfer::fromDomElement(ft_list.item(i).toElement(), this);
		connect(ft, SIGNAL(fileTransferFinished(FileTransfer *)), this, SLOT(fileTransferFinishedSlot(FileTransfer *)));
	}

	kdebugf2();
}

void FileTransferManager::writeToConfig()
{
	kdebugf();

	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement fts_elem = xml_config_file->accessElement(root_elem, "FileTransfers");
	xml_config_file->removeChildren(fts_elem);
	foreach(FileTransfer *i, Transfers)
		i->toDomElement(fts_elem);
	xml_config_file->sync();

	kdebugf2();
}

QStringList FileTransferManager::selectFilesToSend()
{
	return QFileDialog::getOpenFileNames(
		QString::null,
		config_file.readEntry("Network", "LastUploadDirectory"),
		0, "open file", tr("Select file location"));
}

void FileTransferManager::sendFile(UinType receiver, const QString &filename)
{
	kdebugf();

	FileTransfer *ft = search(FileTransfer::TypeSend, receiver, filename);

	if (!ft)
	{
		ft = new FileTransfer(this, DccUnknow, FileTransfer::TypeSend, receiver, filename);
		addTransfer(ft);
	}

	if (!fileTransferWindow)
		toggleFileTransferWindow(0, false);

	ft->start();

	kdebugf2();
}

void FileTransferManager::sendFile(UinType receiver)
{
	kdebugf();

	QStringList files = selectFilesToSend();
	if (!files.count())
	{
		kdebugf2();
		return;
	}

	foreach(const QString &file, files)
		sendFile(receiver, file);

	kdebugf2();
}

void FileTransferManager::sendFileActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *kaduMainWindow = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!kaduMainWindow)
		return;

	UserListElements users = kaduMainWindow->userListElements();
	if (users.count())
		sendFile(users);

	kdebugf2();
}

void FileTransferManager::sendFile(const UserListElements users)
{
	kdebugf();

	QStringList files = selectFilesToSend();
	if (!files.count())
	{
		kdebugf2();
		return;
	}

	unsigned int myUin = config_file.readUnsignedNumEntry("General", "UIN");

	foreach(const UserListElement &user, users)
		foreach(const QString &file, files)
			if (user.usesProtocol("Gadu") && user.ID("Gadu") != QString::number(myUin))
				sendFile(user.ID("Gadu").toUInt(), file);

	kdebugf2();
}

void FileTransferManager::kaduKeyPressed(QKeyEvent *e)
{
// TODO: 0.6.5
// 	if (HotKey::shortCut(e, "ShortCuts", "kadu_sendfile"))
// 		sendFile();
}

void FileTransferManager::chatKeyPressed(QKeyEvent *e, ChatWidget *chatWidget, bool &handled)
{
	if (HotKey::shortCut(e, "ShortCuts", "kadu_sendfile"))
	{
		sendFile(chatWidget->users()->toUserListElements());
		handled = true;
	}
}

void FileTransferManager::chatCreated(ChatWidget *chat)
{
	connect(chat, SIGNAL(fileDropped(const UserGroup *, const QString &)),
		this, SLOT(fileDropped(const UserGroup *, const QString &)));
	connect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));
}

void FileTransferManager::chatDestroying(ChatWidget *chat)
{
	disconnect(chat, SIGNAL(fileDropped(const UserGroup *, const QString &)),
		this, SLOT(fileDropped(const UserGroup *, const QString &)));
	disconnect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));
}

void FileTransferManager::fileDropped(const UserGroup *group, const QString &fileName)
{
	foreach(const UserListElement &user, *group)
		if (user.usesProtocol("Gadu"))
			sendFile(user.ID("Gadu").toUInt(), fileName);
}

void FileTransferManager::showFileTransferWindow()
{
	if (!fileTransferWindow)
		toggleFileTransferWindow(0, false);
}

void FileTransferManager::toggleFileTransferWindow(QAction *sender, bool toggled)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: fileTransferWindow:%p\n", fileTransferWindow);
	if (fileTransferWindow)
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			fileTransferWindow, SLOT(fileTransferDestroying(FileTransfer *)));
		disconnect(fileTransferWindow, SIGNAL(destroyed()), this, SLOT(fileTransferWindowDestroyed()));
		delete fileTransferWindow;
		fileTransferWindow = 0;
	}
	else
	{
		fileTransferWindow = new FileTransferWindow();
		connect(fileTransferWindow, SIGNAL(destroyed()), this, SLOT(fileTransferWindowDestroyed()));
		connect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		connect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			fileTransferWindow, SLOT(fileTransferDestroying(FileTransfer *)));
		fileTransferWindow->show();
	}
	kdebugf2();
}

void FileTransferManager::fileTransferFinishedSlot(FileTransfer *fileTransfer)
{
	QString message;

	if (config_file.readBoolEntry("Network", "RemoveCompletedTransfers"))
		fileTransfer->deleteLater();

	Notification *fileTransferFinishedNotification = new Notification("FileTransfer/Finished", "SendFile", UserListElements());
	fileTransferFinishedNotification->setTitle(tr("File transfer finished"));
	fileTransferFinishedNotification->setText(tr("File has been transferred sucessfully."));

	notification_manager->notify(fileTransferFinishedNotification);
}

void FileTransferManager::fileTransferWindowDestroyed()
{
	kdebugf();
	fileTransferWindow = 0;
}

bool FileTransferManager::socketEvent(DccSocket *socket, bool &lock)
{
	switch (socket->ggDccEvent()->type)
	{
		case GG_EVENT_DCC_NEED_FILE_ACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_FILE_ACK! uin:%d peer_uin:%d\n",
				socket->uin(), socket->peerUin());
			needFileAccept(socket);
			lock = true;
			return true;

		default:
			return false;
	}
}

void FileTransferManager::needFileAccept(DccSocket *socket)
{
	kdebugf();

	QString fileName;
	QString question;

	QString fileSize = QString("%1").arg(((float) socket->fileSize() / 1024), 0, 'f', 2);

	FileTransfer *ft = search(FileTransfer::TypeReceive, socket->peerUin(), socket->fileName(), FileTransfer::FileNameGadu);

	NewFileTransferNotification *newFileTransferNotification;

	if (ft)
	{
		newFileTransferNotification = new NewFileTransferNotification(ft, socket,
			userlist->byID("Gadu", QString::number(socket->peerUin())), FileTransfer::StartRestore);

		question = narg(tr("User %1 wants to send you a file %2\nof size %3kB.\n"
		                   "This is probably a next part of %4\n What should I do?"),
			userlist->byID("Gadu", QString::number(socket->peerUin())).altNick(),
			socket->fileName(),
			fileSize,
			ft->fileName()
		);
	}
	else
	{
		newFileTransferNotification = new NewFileTransferNotification(ft, socket,
			userlist->byID("Gadu", QString::number(socket->peerUin())), FileTransfer::StartNew);

		question = narg(tr("User %1 wants to send you a file %2\nof size %3kB. Accept transfer?"),
			userlist->byID("Gadu", QString::number(socket->peerUin())).altNick(),
			socket->fileName(),
			fileSize
		);
	}

	newFileTransferNotification->setText(question);
	newFileTransferNotification->setTitle("Incoming transfer");

	notification_manager->notify(newFileTransferNotification);

	kdebugf2();
}

void FileTransferManager::acceptFile(FileTransfer *ft, DccSocket *socket, QString fileName, bool resume)
{
	kdebugf();

	bool haveFileName = !fileName.isNull();

	QFileInfo fi;

	while (true)
	{
		if (!haveFileName || fileName.isEmpty())
			fileName = QFileDialog::getSaveFileName(config_file.readEntry("Network", "LastDownloadDirectory")
				+ socket->fileName(),
				QString::null, 0, "save file", tr("Select file location"));

		if (fileName.isEmpty())
		{
			kdebugmf(KDEBUG_INFO, "rejected\n");
			socket->reject();
			return;
		}

		config_file.writeEntry("Network", "LastDownloadDirectory", QFileInfo(fileName).dirPath() + '/');
		fi.setFile(fileName);

		if (!haveFileName && fi.exists())
		{
			QString question;
			question = tr("File %1 already exists.").arg(fileName);

			switch (QMessageBox::question(0, tr("Save file"), question, tr("Overwrite"), tr("Resume"),
			                                 tr("Select another file"), 0, 2))
			{
				case 0:
					resume = false;
					break;

				case 1:
					resume = true;
					break;

				case 2:
					continue;
			}
		}

		haveFileName = false;
		QIODevice::OpenMode flags = QIODevice::WriteOnly;
		if (resume)
			flags |= QIODevice::Append;
		else
			flags |= QIODevice::Truncate;

		QFile file(fileName);
		file.open(flags);

		if (!socket->setFile(file.handle()))
			MessageBox::msg(tr("Could not open file. Select another one."), true, "Warning");
		else
		{
			socket->setOffset(fi.size());

 			FileTransfer *ft = search(FileTransfer::TypeReceive, socket->peerUin(),
		 		fileName);

			if (!ft)
			{
				ft = new FileTransfer(this, DccUnknow, FileTransfer::TypeReceive, socket->peerUin(), fileName);
				addTransfer(ft);
			}

			socket->accept();
			socket->setHandler(ft);

			showFileTransferWindow();

			ft->start();

			break;
		}
	}

	kdebugf2();
}

void FileTransferManager::rejectFile(DccSocket *socket)
{
	kdebugf();
	socket->reject();
	kdebugf2();
}

void FileTransferManager::addTransfer(FileTransfer *transfer)
{
	connect(transfer, SIGNAL(destroyed(QObject *)), this, SLOT(transferDestroyed(QObject *)));

	Transfers.append(transfer);
}

void FileTransferManager::removeTransfer(FileTransfer *transfer)
{
	Transfers.remove(transfer);
}

void FileTransferManager::transferDestroyed(QObject *transfer)
{
	FileTransfer *ft = dynamic_cast<FileTransfer *>(transfer);
	if (ft)
		removeTransfer(ft);
}

const QList<FileTransfer *> FileTransferManager::transfers()
{
	return Transfers;
}

FileTransfer * FileTransferManager::search(FileTransfer::FileTransferType type, const UinType &contact, const QString &fileName,
	FileTransfer::FileNameType fileNameType)
{
	kdebugf();

	foreach(FileTransfer *i, Transfers)
		if (i->Type == type && i->Contact == contact)
			if (fileNameType == FileTransfer::FileNameFull)
			{
				if (i->FileName == fileName)
					return i;
			}
			else
			{
				if (i->GaduFileName == fileName)
					return i;
			}

	return 0;
}

FileTransfer * FileTransferManager::byUin(UinType uin)
{
	kdebugf();

	foreach(FileTransfer *i, Transfers)
		if (i->Contact == uin && i->unused())
			return i;

	return 0;
}

FileTransfer * FileTransferManager::byUinAndStatus(UinType uin, FileTransfer::FileTransferStatus status)
{
	kdebugf();

	foreach(FileTransfer *i, Transfers)
		if (i->Contact == uin && i->unused() == 0 && i->Status == status)
			return i;

	return 0;
}

void FileTransferManager::destroyAll()
{
	kdebugf();
	while (!Transfers.empty())
	{
		FileTransfer *ft = Transfers[0];
		Transfers.pop_front();
		delete ft;
	}
	kdebugf2();
}

void FileTransferManager::dcc7IncomingFileTransfer(DccSocket *socket)
{
	needFileAccept(socket);
}

FileTransferManager* file_transfer_manager = NULL;
