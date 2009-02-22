/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_WIDGET
#define FILE_TRANSFER_WIDGET

#include <QtGui/QFrame>

class QLabel;
class QProgressBar;
class QPushButton;

class FileTransfer;

class FileTransferWidget : public QFrame
{
	Q_OBJECT

	FileTransfer *CurrentTransfer;

	QLabel *DescriptionLabel;
	QLabel *StatusLabel;
	QProgressBar *ProgressBar;
	QPushButton *PauseButton;
	QPushButton *ContinueButton;

	void createGui();

private slots:
	void fileTransferDestroyed(QObject *);

	void remove();
	void pauseTransfer();
	void continueTransfer();

public:
	FileTransferWidget(FileTransfer *fileTransfer = 0, QWidget *parent = 0);
	virtual ~FileTransferWidget();

	FileTransfer * fileTransfer() { return CurrentTransfer; }

public slots:
	void fileTransferStatusChanged();

};

#endif // FILE_TRANSFER_WIDGET
