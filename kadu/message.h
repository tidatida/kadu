/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <qdialog.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include "libgadu.h"
#include "misc.h"
#include "../config.h"

class Message : public QDialog {
	Q_OBJECT
		QPushButton *sendbtn;
		int index;
		QString nicksnd;
		QMultiLineEdit *body;	
		QLabel *nicknamelab;
		QDialog *sendmessage;
		QRadioButton *b_chat;
#ifdef HAVE_OPENSSL
                QCheckBox *b_encryptmsg;
#endif

	public:
    		Message(const QString &, bool tchat=false, QWidget *parent=0, const char *name=0);
		~Message();
		void init(void);

	public slots:
		void gotAck(void);
		void commitSend(void);
		void HistoryBox(void);		
/*
	protected slots:
		void accept();
		void reject();*/
};

class rMessage : public QDialog {
	Q_OBJECT
	private:
		QString sender;
		QMultiLineEdit *body;	
		QLabel *lejbel;
		QDialog *recvmsg;
		bool tchat;
	
	public:
		rMessage(const QString &, int msgclass, UinsList uins, QString &msg,
			QWidget *parent=0, const char *name=0);
		void init(void);

	public slots:
		void replyMessage(void);
		void openChat(void);
		void cleanUp(void);
};

#endif
