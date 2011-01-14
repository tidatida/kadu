/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include <QtCore/QTimer>
#include <QtGui/QProgressDialog>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "modules/history/history.h"
#include "debug.h"

#include "gui/windows/history-import-window.h"

#include "history-import-thread.h"

#include "history-importer.h"

HistoryImporter::HistoryImporter(const Account &account, const QString &path, QObject *parent) :
		QObject(parent), DesctinationAccount(account), SourceDirectory(path), Thread(0)
{
	kdebugf();
}

HistoryImporter::~HistoryImporter()
{
	kdebugf();
}

void HistoryImporter::run()
{
	kdebugf();

	if (Thread)
		return;

	if (!DesctinationAccount || SourceDirectory.isEmpty() || !History::instance()->currentStorage())
		return;

	QList<UinsList> uinsLists = HistoryMigrationHelper::getUinsLists(SourceDirectory);
	int totalEntries = 0;

	foreach (const UinsList &uinsList, uinsLists)
		totalEntries += HistoryMigrationHelper::getHistoryEntriesCount(SourceDirectory, uinsList);

	if (0 == totalEntries)
		return;

	ProgressWindow = new HistoryImportWindow();
	ProgressWindow->setChatsCount(uinsLists.size());

	connect(ProgressWindow, SIGNAL(rejected()), this, SLOT(canceled()));

	Thread = new HistoryImportThread(DesctinationAccount, SourceDirectory, uinsLists, totalEntries);
	connect(Thread, SIGNAL(finished()), this, SLOT(threadFinished()));

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
	if (ProgressWindow)
	{
		ProgressWindow->setChatsProgress(Thread->importedChats());
		ProgressWindow->setMessagesCount(Thread->totalMessages());
		ProgressWindow->setMessagesProgress(Thread->importedMessages());
	}
}

void HistoryImporter::threadFinished()
{
	config_file.writeEntry("History", "Imported_from_0.6.5", true);

	delete ProgressWindow;
	ProgressWindow = 0;

	deleteLater();
}

void HistoryImporter::canceled()
{
	if (Thread)
		Thread->cancel();

	deleteLater();
}
