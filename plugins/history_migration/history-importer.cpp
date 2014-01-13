/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtWidgets/QProgressDialog>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "misc/kadu-paths.h"
#include "plugins/history/history.h"
#include "debug.h"

#include "gui/windows/history-import-window.h"

#include "history-import-thread.h"
#include "history-migration-actions.h"

#include "history-importer.h"

HistoryImporter::HistoryImporter(const Account &account, const QString &path, QObject *parent) :
		QObject(parent), DestinationAccount(account), SourceDirectory(path), Thread(0), ProgressWindow(0)
{
	kdebugf();
}

HistoryImporter::~HistoryImporter()
{
	kdebugf();

	if (Thread)
	{
		disconnect(HistoryImport, 0, this, 0);
		HistoryImport->cancel(true);
		Thread->wait(2000);
		if (Thread->isRunning())
		{
			Thread->terminate();
			Thread->wait(2000);
		}
	}

	delete ProgressWindow;
	ProgressWindow = 0;
}

void HistoryImporter::run()
{
	kdebugf();

	if (Thread)
		return;

	if (!DestinationAccount || SourceDirectory.isEmpty() || !History::instance()->currentStorage())
	{
		deleteLater();
		return;
	}

	QList<UinsList> uinsLists = HistoryMigrationHelper::getUinsLists(SourceDirectory);
	int totalEntries = 0;

	foreach (const UinsList &uinsList, uinsLists)
		totalEntries += HistoryMigrationHelper::getHistoryEntriesCount(SourceDirectory, uinsList);

	if (0 == totalEntries)
	{
		deleteLater();
		return;
	}

	HistoryImport = new HistoryImportThread(DestinationAccount, SourceDirectory, uinsLists);
	HistoryImport->setFormattedStringFactory(Core::instance()->formattedStringFactory());
	HistoryImport->prepareChats();

	Thread = new QThread();
	HistoryImport->moveToThread(Thread);

	connect(Thread, SIGNAL(started()), HistoryImport, SLOT(run()));
	connect(HistoryImport, SIGNAL(finished()), this, SLOT(threadFinished()));

	ProgressWindow = new HistoryImportWindow();
	ProgressWindow->setChatsCount(uinsLists.size());
	connect(ProgressWindow, SIGNAL(rejected()), HistoryImport, SLOT(cancel()));

	QTimer *updateProgressBar = new QTimer(this);
	updateProgressBar->setSingleShot(false);
	updateProgressBar->setInterval(200);
	connect(updateProgressBar, SIGNAL(timeout()), this, SLOT(updateProgressWindow()));

	Thread->start();
	ProgressWindow->show();
	updateProgressBar->start();
}

void HistoryImporter::updateProgressWindow()
{
	if (ProgressWindow && HistoryImport)
	{
		ProgressWindow->setChatsProgress(HistoryImport->importedChats());
		ProgressWindow->setMessagesCount(HistoryImport->totalMessages());
		ProgressWindow->setMessagesProgress(HistoryImport->importedMessages());
	}
}

void HistoryImporter::threadFinished()
{
	if (HistoryImport && !HistoryImport->wasCanceled() && SourceDirectory == KaduPaths::instance()->profilePath() + QLatin1String("history/"))
	{
		config_file.writeEntry("History", "Imported_from_0.6.5", true);
		// this is no longer useful
		HistoryMigrationActions::unregisterActions();
	}

	deleteLater();
}

#include "moc_history-importer.cpp"
