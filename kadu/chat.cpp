/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpixmap.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qlayout.h>
#include <qaccel.h>
#include <qtimer.h>
#include <qcursor.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qsplitter.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qvaluelist.h>

#include <stdlib.h>
#include <math.h>

//
#include "config_dialog.h"
#include "kadu.h"
#include "pixmaps.h"
#include "chat.h"
#include "search.h"
#include "history.h"
#include "misc.h"
#include "emoticons.h"
#include "debug.h"
#include "sound.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif
//

QValueList<UinsList> wasFirstMsgs;
const char *colors[16] = {"#FF0000", "#A00000", "#00FF00", "#00A000", "#0000FF", "#0000A0", "#FFFF00",
	"#A0A000", "#FF00FF", "#A000A0", "#00FFFF", "#00A0A0", "#FFFFFF", "#A0A0A0", "#808080", "#000000"};

KaduTextBrowser::KaduTextBrowser(QWidget *parent, const char *name)
	: QTextBrowser(parent, name) {

}

void KaduTextBrowser::setSource(const QString &name) {
}

CustomInput::CustomInput(QWidget *parent, const char *name) : QMultiLineEdit(parent, name) {
}

void CustomInput::keyPressEvent(QKeyEvent * e) {
	kdebug("CustomInput::keyPressEvent()\n");
	if (autosend_enabled && e->key() == Key_Return && !(e->state() & ShiftButton)) {
		kdebug("CustomInput::keyPressEvent(): emit enterPressed()\n");
		emit sendMessage();
		}
	else {
		if (e->key() == Key_Minus) {
			insert("-");
			return;
			}
		if (e->text() == "*") {
			insert("*");
			return;
			}
		if (e->state() & ControlButton) {
			if (e->key() == Key_B) {
				emit specialKeyPressed(CustomInput::KEY_BOLD);
				return;
				}
			else if (e->key() == Key_I) {
				emit specialKeyPressed(CustomInput::KEY_ITALIC);
				return;
				}
			else if (e->key() == Key_U)
				emit specialKeyPressed(CustomInput::KEY_UNDERLINE);
			else if (e->key() == Key_Minus) {
				}
			}
		QMultiLineEdit::keyPressEvent(e);
		}
}

void CustomInput::setAutosend(bool on) {
	autosend_enabled = on;
}

void CustomInput::paste() {
	pasteSubType("plain");
}

Chat::Chat(UinsList uins, QWidget *parent, const char *name)
 : QWidget(parent, name, Qt::WDestructiveClose), uins(uins) {
	int i;
	struct chats chat;
	QValueList<int> sizes;

	emoticon_selector = NULL;
	color_selector = NULL;

	title_timer = new QTimer(this);
	connect(title_timer,SIGNAL(timeout()),this,SLOT(changeTitle()));
  
	/* register us in the chats registry... */
	chat.uins = uins;
	chat.ptr = this;
	chats.append(chat);
	index = chats.count() - 1;

	QSplitter *split1, *split2;

	split1 = new QSplitter(Qt::Vertical, this);

	if (uins.count() > 1) {
		split2 = new QSplitter(Qt::Horizontal, split1);
		body = new KaduTextBrowser(split2);
		}
	else 
		body = new KaduTextBrowser(split1);
		
	if(config.emoticons_style==EMOTS_ANIMATED)
		body->setStyleSheet(new AnimStyleSheet(body,emoticons.themePath()));
	
	body->setMinimumSize(QSize(1,1));
	body->setFont(config.fonts.chat);
	QObject::connect(body, SIGNAL(linkClicked(const QString &)), this, SLOT(hyperlinkClicked(const QString &)));

	QPoint pos = QCursor::pos();
	
	if (uins.count() > 1) {
		setGeometry((pos.x() + 550) / 2, (pos.y() + 400) / 2, 550, 400);
		userbox = new UserBox(split2);
		userbox->setMinimumSize(QSize(1,1));
		userbox->setPaletteBackgroundColor(config.colors.userboxBg);
		userbox->setPaletteForegroundColor(config.colors.userboxFg);
		userbox->QListBox::setFont(config.fonts.userbox);

		for (i = 0; i < uins.count(); i++)
			userbox->addUser(userlist.byUin(uins[i]).altnick);
		userbox->refresh();
		sizes.append(3);
		sizes.append(1);
		split2->setSizes(sizes);
		}
	else {
		setGeometry((pos.x() + 400) / 2, (pos.y() + 400) / 2, 400, 400);
		userbox = NULL;
		}
		
	QVBox *downpart = new QVBox(split1);
	QHBox *edtbuttontray = new QHBox(downpart);

	QLabel *edt = new QLabel(i18n("Edit window:"),edtbuttontray);
	QToolTip::add(edt, i18n("This is where you type in the text to be sent"));

	buttontray = new QHBox(edtbuttontray);

	autosend = new QPushButton(buttontray);
	autosend->setPixmap(loadIcon("key_enter.png"));
	autosend->setToggleButton(true);
	QToolTip::add(autosend, i18n("Enter key sends message"));

	lockscroll = new QPushButton(buttontray);
	lockscroll->setPixmap(QPixmap((const char**)scroll_lock));
	lockscroll->setToggleButton(true);
	QToolTip::add(lockscroll, i18n("Blocks scrolling"));

#ifdef HAVE_OPENSSL
	encryption = new QPushButton(buttontray);
	connect(encryption, SIGNAL(clicked()), this, SLOT(regEncryptSend()));

	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(uins[0]));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);
	bool encryption_possible =
		(keyfile.permission(QFileInfo::ReadUser) && uins.count() == 1);

	setupEncryptButton(config.encryption && encryption_possible);
	
	encryption->setEnabled(encryption_possible);	
#endif
	
	QPushButton *clearchat= new QPushButton(buttontray);
	clearchat->setPixmap(loadIcon("eraser.png"));
	QToolTip::add(clearchat, i18n("Clear messages in chat window"));

	iconsel = new QPushButton(buttontray);
	iconsel->setPixmap(loadIcon("icons.png"));
	if(config.emoticons_style==EMOTS_NONE)
	{
		QToolTip::add(iconsel, i18n("Insert emoticon - enable in configuration"));
		iconsel->setEnabled(false);
	}
	else
		QToolTip::add(iconsel, i18n("Insert emoticon"));

	QPushButton *history = new QPushButton(buttontray);
	history->setPixmap(QPixmap((const char**)history_xpm));
	QToolTip::add(history, i18n("Show history"));

	QPushButton *whois = new QPushButton(buttontray);
	whois->setPixmap(loadIcon("viewmag.png"));
	QToolTip::add(whois, i18n("Lookup user info"));

	edtbuttontray->setStretchFactor(edt, 50);
	edtbuttontray->setStretchFactor(buttontray, 1);

	edit = new CustomInput(downpart);
	edit->setMinimumHeight(1);
	edit->setWordWrap(QMultiLineEdit::WidgetWidth);
	edit->setFont(config.fonts.chat);

	if (config.autosend)
		autosend->setOn(true);
	edit->setAutosend(config.autosend);

	QHBox *btnpart = new QHBox(downpart);

	QFont afont = QApplication::font();
	boldbtn = new QPushButton("B", btnpart);
	boldbtn->setToggleButton(true);
	afont.setBold(true);
	boldbtn->setFont(afont);
	italicbtn = new QPushButton("I", btnpart);
	italicbtn->setToggleButton(true);
	afont.setBold(false);
	afont.setItalic(true);
	italicbtn->setFont(afont);
	underlinebtn = new QPushButton("U", btnpart);
	underlinebtn->setToggleButton(true);
	afont.setItalic(false);
	afont.setUnderline(true);
	underlinebtn->setFont(afont);
	colorbtn = new QPushButton(btnpart);
//	colorbtn->setMinimumSize(boldbtn->width(), boldbtn->height());
	QPixmap p(16, 16);
	p.fill(colors[15]);
	colorbtn->setPixmap(p);

	QHBox *fillerbox = new QHBox(btnpart);

	cancelbtn = new QPushButton(QIconSet(loadIcon("stop.png")),i18n("&Cancel"),btnpart);
	cancelbtn->setFixedWidth(120);
	QToolTip::add(cancelbtn, i18n("Cancel waiting for delivery"));
	cancelbtn->hide();

	sendbtn = new QPushButton(QIconSet(loadIcon("forward.png")),i18n("&Send"),btnpart);
	sendbtn->setFixedWidth(120);
	connect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
	QAccel *acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_Return + CTRL), this, SLOT(sendMessage()));

	acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_PageUp + SHIFT), this, SLOT(pageUp()));
	acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_PageDown + SHIFT), this, SLOT(pageDown()));

	btnpart->setStretchFactor(boldbtn, 1);
	btnpart->setStretchFactor(italicbtn, 1);
	btnpart->setStretchFactor(underlinebtn, 1);
	btnpart->setStretchFactor(colorbtn, 1);
	btnpart->setStretchFactor(fillerbox, 50);
	btnpart->setStretchFactor(cancelbtn, 1);
	btnpart->setStretchFactor(sendbtn, 1);

	sizes.clear();
	sizes.append(3);
	sizes.append(2);
	split1->setSizes(sizes);

	QGridLayout *grid = new QGridLayout (this, 5, 4, 3, 3);
	grid->addMultiCellWidget(split1, 0, 4, 0, 3);
	grid->addRowSpacing(1, 5);
	grid->setRowStretch(0, 2);

	QMimeSourceFactory *bodyformat;
	bodyformat = new QMimeSourceFactory;

	body->setMimeSourceFactory(bodyformat);
	body->setTextFormat(Qt::RichText);
	edit->setMimeSourceFactory(bodyformat);
	edit->setTextFormat(Qt::RichText);

	connect(autosend, SIGNAL(toggled(bool)), edit, SLOT(setAutosend(bool)));
	connect(history, SIGNAL(clicked()), this, SLOT(HistoryBox()));
	connect(iconsel, SIGNAL(clicked()), this, SLOT(insertEmoticon()));
	connect(whois, SIGNAL(clicked()), this, SLOT(userWhois()));
	connect(clearchat, SIGNAL(clicked()), this, SLOT(clearChatWindow()));
	connect(cancelbtn, SIGNAL(clicked()), this, SLOT(cancelMessage()));
	connect(boldbtn, SIGNAL(toggled(bool)), this, SLOT(toggledBold(bool)));
	connect(italicbtn, SIGNAL(toggled(bool)), this, SLOT(toggledItalic(bool)));
	connect(underlinebtn, SIGNAL(toggled(bool)), this, SLOT(toggledUnderline(bool)));
	connect(colorbtn, SIGNAL(clicked()), this, SLOT(changeColor()));
	connect(edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(curPosChanged(int, int)));
	connect(edit, SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(edit, SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));

	totaloccurences = 0;

	edit->setFocus();
}

Chat::~Chat() {
	int i,j;

	i = 0;
	while (i < chats.count() && chats[i].ptr != this)
		i++;
	chats.remove(chats.at(i));

	i = 0;
	while (i < acks.size() && acks[i].ptr != this)
		i++;
	if (i < acks.size()) {
		for (j = i + 1; j < acks.size(); j++) {
			acks[j-1].ack = acks[j].ack;
			acks[j-1].seq = acks[j].seq;
			acks[j-1].ptr = acks[j].ptr;
			acks[j-1].type = acks[j].type;
			}
		acks.resize(acks.size() - 1);
		}
	if (userbox)
		delete userbox;
		
	kdebug("Chat::~Chat: chat destroyed: index %d\n", index);
}

void Chat::specialKeyPressed(int key) {
	kdebug("Chat::specialKeyPressed()\n");
	switch (key) {
		case CustomInput::KEY_BOLD:
			boldbtn->setOn(!boldbtn->isOn());
			edit->setBold(boldbtn->isOn());
			break;
		case CustomInput::KEY_ITALIC:
			italicbtn->setOn(!italicbtn->isOn());
			edit->setItalic(italicbtn->isOn());
			break;
		case CustomInput::KEY_UNDERLINE:
			underlinebtn->setOn(!underlinebtn->isOn());
			edit->setUnderline(underlinebtn->isOn());
			break;
		}
}

void Chat::toggledBold(bool on) {
	kdebug("Chat::toggledBold()\n");
	edit->setBold(on);
}

void Chat::toggledItalic(bool on) {
	kdebug("Chat::toggledItalic()\n");
	edit->setItalic(on);
}

void Chat::toggledUnderline(bool on) {
	kdebug("Chat::toggledUnderline()\n");
	edit->setUnderline(on);
}

void Chat::curPosChanged(int para, int pos) {
	int i;

	kdebug("Chat::curPosChanged()\n");
	if (edit->bold() != boldbtn->isOn())
		boldbtn->setOn(edit->bold());
	if (edit->italic() != italicbtn->isOn())
		italicbtn->setOn(edit->italic());
	if (edit->underline() != underlinebtn->isOn())
		underlinebtn->setOn(edit->underline());
	if (edit->color() != actcolor) {
		for (i = 0; i < 16; i++)
			if (edit->color() == QColor(colors[i]))
				break;
		QPixmap p(16, 16);
		p.fill(colors[i]);
		colorbtn->setPixmap(p);
		actcolor = colors[i];
		}
	
}

void Chat::setupEncryptButton(bool enabled) {
#ifdef HAVE_OPENSSL
	encrypt_enabled = enabled;
	QToolTip::remove(encryption);
	if (enabled) {
		QToolTip::add(encryption, i18n("Disable encryption for this conversation"));
		encryption->setPixmap(loadIcon("encrypted.png"));
		}
	else {
		QToolTip::add(encryption, i18n("Enable encryption for this conversation"));
		encryption->setPixmap(loadIcon("decrypted.png"));
		}
#endif		
}

void Chat::pageUp() {
	body->scrollBy(0, (body->height() * -2) / 3);
}

void Chat::pageDown() {
	body->scrollBy(0, (body->height() * 2) / 3);
}

void Chat::setEncryptionBtnEnabled(bool enabled) {
#ifdef HAVE_OPENSSL
	encryption->setEnabled(enabled && config.encryption);
#endif
}

void Chat::changeAppearance() {
	if (uins.count() > 1 && userbox) {
		userbox->setPaletteBackgroundColor(config.colors.userboxBg);
		userbox->setPaletteForegroundColor(config.colors.userboxFg);
		userbox->QListBox::setFont(config.fonts.userbox);
		}
	body->setFont(config.fonts.chat);
	edit->setFont(config.fonts.chat);
}

void Chat::setTitle() {
	QString name;
	QString title;
	int k;
	UserListElement user;

	if (uins.size() > 1) {
		if (config.conferenceprefix.isEmpty())
			title = i18n("Conference with ");
		else
			title = config.conferenceprefix;
		for (k = 0; k < uins.size(); k++) {
			if (k)
				title.append(", ");
			if (userlist.containsUin(uins[k]))
				user = userlist.byUin(uins[k]);
			else
				user.uin = uins[k];
			title.append(parse(config.conferencesyntax,user,false));
		}
		setIcon(*icons->loadIcon("online"));
	}
	else {
		if (userlist.containsUin(uins[0]))
			user = userlist.byUin(uins[0]);
		else
			user.uin = uins[0];
		if (config.chatsyntax.isEmpty())
			title = parse(i18n("Chat with ")+"%a (%s[: %d])",user,false);
		else
			title = parse(config.chatsyntax,user,false);
		setIcon(*icons->loadIcon(gg_icons[statusGGToStatusNr(user.status)]));
	}

	title.replace(QRegExp("\n"), " ");

	setCaption(title);
	title_buffer = title;
}

void Chat::changeTitle() {
	if(!isActiveWindow()){
		if (caption() == "  "){
			setCaption(title_buffer);
			title_timer->start(500,TRUE);
		}
		else{    
			setCaption("  ");
			title_timer->start(500,TRUE);
		}
	}
}

void Chat::windowActivationChange(bool oldActive) {
	if (isActiveWindow() && title_timer->isActive()){
		title_timer->stop();
		setCaption(title_buffer);
	}
}

void Chat::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Key_F9)
		clearChatWindow();
	else
		if (e->key() == Key_Escape)
			close();
	QWidget::keyPressEvent(e);
}

void Chat::regEncryptSend(void) {
#ifdef HAVE_OPENSSL
	encrypt_enabled = !encrypt_enabled;
	setupEncryptButton(encrypt_enabled);
#endif
}

/* convert special characters into emoticons, HTML into plain text and so forth */
QString Chat::convertCharacters(QString edit, bool me) {

	// escape'ujemy http:// i ftp:// zeby emotikony nie bruzdzily
	edit.replace(QRegExp("http://"),"___escaped_http___");
	edit.replace(QRegExp("ftp://"),"___escaped_ftp___");
	
	if(config.emoticons_style!=EMOTS_NONE)
	{
		body->mimeSourceFactory()->addFilePath(emoticons.themePath());
		if (me)
			emoticons.expandEmoticons(edit,config.colors.mychatBg);
		else
			emoticons.expandEmoticons(edit,config.colors.usrchatBg);
	};
	
	// przywracamy http:// i ftp://
	edit.replace(QRegExp("___escaped_http___"),"http://");
	edit.replace(QRegExp("___escaped_ftp___"),"ftp://");
	
	// zmieniamy windowsowe \r\n na unixowe \n
	edit.replace( QRegExp("\r\n"), "\n" );
//	edit.replace( QRegExp("<"), "&lt;" );
//	edit.replace( QRegExp(">"), "&gt;" );
	edit.replace( QRegExp("__escaped_lt__"), "<");
	edit.replace( QRegExp("__escaped_gt__"), ">");
	edit.replace( QRegExp("  "), " &nbsp;" );
	edit.replace( QRegExp("\n"), "<BR>" );

	// detekcja adresow url
	QRegExp url_regexp("(http://|www\\.|ftp://)[a-zA-Z0-9\\-\\._/~?=&#\\+%:]+");
	for (int s=0; s<edit.length(); s++)
	{
		int p=url_regexp.search(edit,s);
		if (p < 0)
			break;
		int l=url_regexp.matchedLength();
		QString link="<a href=\""+edit.mid(p,l)+"\">"+
			edit.mid(p,l)+"</a>";
		edit=edit.left(p)+link+edit.mid(p+l);
		s=p+link.length();
	};

	return edit;
}

/* unregister us */
void Chat::closeEvent(QCloseEvent *e) {
	kdebug("Chat::closeEvent()\n");	
	QWidget::closeEvent(e);
}

/* look up party's info */
void Chat::userWhois(void) {
	SearchDialog *sd;
	QString tmp;
	uin_t uin;
	
	if (!userbox)
		uin = uins[0];
	else
		if (userbox->currentItem() == -1)
			uin = uins[0];
		else
			uin = userlist.byAltNick(userbox->currentText()).uin;
	sd = new SearchDialog(0, "User info", uin);
	sd->init();
	sd->show();
	sd->firstSearch();
}

void Chat::hyperlinkClicked(const QString &link) {
	QProcess *browser;
	QString cmd;
	QStringList args;

	if (config.defaultwebbrowser)
		cmd = QString("konqueror %1").arg(link);
	else {
                if (config.webbrowser == "") {
			QMessageBox::warning(this, i18n("WWW error"),
				i18n("Web browser was not specified. Visit the configuration section"));
			kdebug("Chat::hyperlinkClicked(): Web browser NOT specified.\n");
			return;
			}
		cmd = QString(config.webbrowser).arg(link);
		}
	args = QStringList::split(" ", cmd);
	for (QStringList::iterator i = args.begin(); i != args.end(); i++)
		kdebug("Chat::hyperlinkClicked(): %s\n", (*i).latin1());
	browser = new QProcess(this);
	browser->setArguments(args);
	if (!browser->start())
		QMessageBox::critical(this, i18n("WWW error"),
			i18n("Could not spawn Web browser process. Check if the Web browser is functional"));
//	QObject::connect(smsProcess, SIGNAL(processExited()), this, SLOT(smsSigHandler()));
	delete browser;
}

void Chat::formatMessage(bool me, const QString &altnick, const QString &msg, const QString &time, QString &toadd) {
	QString editext = convertCharacters(msg,me);

	toadd.append("<TABLE width=\"100%\"><TR><TD bgcolor=\"");
	if (me)
	    	toadd.append(config.colors.mychatBg.name());
	else
	    	toadd.append(config.colors.usrchatBg.name());
	toadd.append("\"><FONT color=\"");
	if (me)
		toadd.append(config.colors.mychatText.name());
	else
		toadd.append(config.colors.usrchatText.name());
	toadd.append("\"><B>");
	toadd.append(altnick);
	toadd.append(" ");
	toadd.append(time + "</B><BR>" + editext + "</TD></TR></TABLE></FONT>");
}

void Chat::scrollMessages(QString &toadd) {
	if (config.chatprune)
		pruneWindow();

	body->viewport()->setUpdatesEnabled(false);
	if (!config.scrolldown)
		body->setText(toadd + body->text());
	else {
		body->setText(body->text() + toadd);
		if (!lockscroll->isOn())
			body->scrollToBottom();
		}
	body->viewport()->setUpdatesEnabled(true);
	body->viewport()->repaint();
}

void Chat::writeMessagesFromHistory(UinsList senders, time_t time) {
	QString toadd;
	QValueList<HistoryEntry> entries;
	QValueList<HistoryEntry> entriestmp;
	QDateTime date;
	int i, from, end, count;
	
	kdebug("Chat::writeMessageFromHistory()\n");

	QValueList<UinsList>::iterator uinsit = wasFirstMsgs.begin();
	while (uinsit != wasFirstMsgs.end() && !senders.equals(*uinsit))
		uinsit++;
//	if (uinsit != wasFirstMsgs.end())
//		return;
//	else
//		wasFirstMsgs.append(senders);

	date.setTime_t(time);
	count = history.getHistoryEntriesCount(senders);
	end = count - 1;
	
	while (end >= 0 && entries.count() < config.chathistorycitation) {
		from = (end < config.chathistorycitation) ? 0 : end - config.chathistorycitation + 1;
		entriestmp = history.getHistoryEntries(senders, from, end - from + 1, HISTORYMANAGER_ENTRY_CHATSEND
			| HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV);
		kdebug("Chat::writeMessageFromHistory(): temp entries = %d\n", entriestmp.count());
		if (time) {
			QValueList<HistoryEntry>::iterator it = entriestmp.begin();
			while (it != entriestmp.end()) {
				if ((*it).type == HISTORYMANAGER_ENTRY_CHATRCV
					|| (*it).type == HISTORYMANAGER_ENTRY_MSGRCV) {
						kdebug("Chat::writeMessageFromHistory(): %s %s\n",
							(const char *)date.toString("dd.MM.yyyy hh:mm:ss").local8Bit(),
							(const char *)(*it).sdate.toString("dd.MM.yyyy hh:mm:ss").local8Bit());
						if (date <= (*it).sdate)
							it = entriestmp.remove(it);
						else
							it++;
						}
				else
					it++;
				}
			}
		if (entriestmp.count())
			entries = entriestmp + entries;
		kdebug("Chat::writeMessageFromHistory(): entries = %d\n", entries.count());
		end = from - 1;
		}

	from = (entries.count() < config.chathistorycitation) ? 0 : entries.count() - config.chathistorycitation;
	for (i = from; i < entries.count(); i++)
		if (entries[i].date.secsTo(QDateTime::currentDateTime()) <= config.chathistorycitationtime * 3600)
			if (entries[i].type == HISTORYMANAGER_ENTRY_MSGSEND
				|| entries[i].type == HISTORYMANAGER_ENTRY_CHATSEND)
				formatMessage(true, config.nick, entries[i].message,
					entries[i].date.toString(":: dd.MM.yyyy (hh:mm:ss)"), toadd);
			else
				formatMessage(false, entries[i].nick, entries[i].message,
					entries[i].date.toString(":: dd.MM.yyyy (hh:mm:ss / S ")
					+ entries[i].sdate.toString("hh:mm:ss)"), toadd);
	if (toadd.length())
		scrollMessages(toadd);
}

/* invoked from outside when new message arrives, this is the window to the world */
void Chat::checkPresence(UinsList senders, QString &msg, time_t time, QString &toadd) {
	formatMessage(false, userlist.byUin(senders[0]).altnick, msg, timestamp(time), toadd);

	scrollMessages(toadd);
}

void Chat::alertNewMessage(void) {
	if (config.playsoundchatinvisible){
		if (!isActiveWindow())
			playSound(config.soundchat);
		}
	else
		if (config.playsoundchat)
			playSound(config.soundchat);

	if (config.blinkchattitle)
		if (!isActiveWindow() && !title_timer->isActive())
			changeTitle();
}

void Chat::writeMyMessage() {
	QString toadd;

	formatMessage(true, config.nick, myLastMessage, timestamp(), toadd);
	scrollMessages(toadd);
	if (!edit->isEnabled())
		cancelMessage();
	edit->clear();
	if (boldbtn->isOn())
		edit->setBold(true);
	if (italicbtn->isOn())
		edit->setItalic(true);
	if (underlinebtn->isOn())
		edit->setUnderline(true);
}

void Chat::addMyMessageToHistory() {
	int uin;

	uin = uins[0];
	if (config.logmessages)
		history.appendMessage(uins, uin, myLastMessage, true);
}

void Chat::clearChatWindow(void) {
	body->clear();
	totaloccurences = 0;
}

void Chat::cancelMessage(void) {
	edit->setReadOnly(false);
	edit->setEnabled(true);
	edit->setFocus();
	sendbtn->setEnabled(true);
	cancelbtn->hide();
}

/* sends the message typed */
void Chat::sendMessage(void) {
	int i, j, online;
	uin_t *users;
	QString mesg;

	if (getActualStatus() == GG_STATUS_NOT_AVAIL) {
		QMessageBox::critical(this, i18n("Send message error"),
			i18n("Application encountered network error."));
		return;
		}

	myLastMessage = edit->text();

	mesg = myLastMessage;
	mesg.replace(QRegExp("\n"), "\r\n");
	mesg = unformatGGMessage(mesg, myLastFormatsLength, myLastFormats);
	myLastMessage = mesg;
	if (myLastFormatsLength)
		myLastMessage = formatGGMessage(myLastMessage, myLastFormatsLength - sizeof(struct gg_msg_richtext),
			(void *)((char *)(myLastFormats) + sizeof(struct gg_msg_richtext)));
	else
		escapeSpecialCharacters(myLastMessage);
	kdebug("Chat::sendMessage():\n%s\n", myLastMessage.latin1());
	myLastMessage.replace(QRegExp("\r\n"), "\n");

	if (!mesg.length() || mesg.length() >= 2000)
		return;

	addMyMessageToHistory();
	// zmieniamy unixowe \n na windowsowe \r\n

	if (config.msgacks) {
		edit->setReadOnly(true);	
		edit->setEnabled(false);
		sendbtn->setEnabled(false);
		cancelbtn->show();
		}

	unsigned char *utmp = (unsigned char *)strdup(unicode2cp(mesg).data());

	users = new (uin_t)[uins.count()];
	for (j = 0, online = 0; j < uins.count(); j++) {
		UserListElement &ule = userlist.byUin(uins[j]);
		if (ule.status == GG_STATUS_AVAIL || ule.status == GG_STATUS_AVAIL_DESCR ||
			ule.status == GG_STATUS_BUSY || ule.status == GG_STATUS_BUSY_DESCR)
			online++;
		}
	online = uins.count();
	if (config.msgacks && online) {
		acks.resize(acks.size() + 1);
		i = acks.size() - 1;
		if (uins.count() > 1) {
			for (j = 0; j < uins.count(); j++)
				users[j] = uins[j];
			if (myLastFormatsLength)
				acks[i].seq = gg_send_message_confer_richtext(sess, GG_CLASS_CHAT,
					uins.count(), users, (unsigned char *)utmp,
					(unsigned char *)myLastFormats, myLastFormatsLength);
			else
				acks[i].seq = gg_send_message_confer(sess, GG_CLASS_CHAT,
					uins.count(), users, (unsigned char *)utmp);
			acks[i].ack = online;
			}
		else {
#ifdef HAVE_OPENSSL
			if (encrypt_enabled) {
				char* encrypted = sim_message_encrypt((unsigned char *)utmp, uins[0]);
				if (encrypted != NULL) {
					if (myLastFormatsLength)
						acks[i].seq = gg_send_message_richtext(sess, GG_CLASS_CHAT,
							uins[0], (unsigned char *)encrypted,
							(unsigned char *)myLastFormats, myLastFormatsLength);
					else
						acks[i].seq = gg_send_message(sess, GG_CLASS_CHAT,
							uins[0], (unsigned char *)encrypted);
					acks[i].ack = 1;
					free(encrypted);
				}
			} else {
#endif
				if (myLastFormatsLength)
					acks[i].seq = gg_send_message_richtext(sess, GG_CLASS_CHAT, uins[0],
						(unsigned char *)utmp,
						(unsigned char *)myLastFormats, myLastFormatsLength);
				else
					acks[i].seq = gg_send_message(sess, GG_CLASS_CHAT, uins[0],
						(unsigned char *)utmp);
				acks[i].ack = 1;
#ifdef HAVE_OPENSSL
			}
#endif
		}

		acks[i].type = 2;
		acks[i].ptr = this;
		}
	else {
		if (uins.count() > 1) {
			for (j = 0; j < uins.count(); j++)
				users[j] = uins[j];
			if (myLastFormatsLength)
				gg_send_message_confer_richtext(sess, GG_CLASS_CHAT,
					uins.count(), users, (unsigned char *)utmp,
					(unsigned char *)myLastFormats, myLastFormatsLength);
			else
				gg_send_message_confer(sess, GG_CLASS_CHAT,
					uins.count(), users, (unsigned char *)utmp);
			}
		else {
#ifdef HAVE_OPENSSL
			if (encrypt_enabled) {
				char* encrypted = sim_message_encrypt((unsigned char *)utmp, uins[0]);
				if (encrypted != NULL) {
					if (myLastFormatsLength)
						gg_send_message_richtext(sess, GG_CLASS_CHAT, uins[0],
							(unsigned char *)encrypted,
							(unsigned char *)myLastFormats, myLastFormatsLength);
					else
						gg_send_message(sess, GG_CLASS_CHAT, uins[0],
							(unsigned char *)encrypted);
					free(encrypted);
				}
			} else {
#endif
				if (myLastFormatsLength)
					gg_send_message_richtext(sess, GG_CLASS_CHAT, uins[0], (unsigned char *)utmp,
						(unsigned char *)myLastFormats, myLastFormatsLength);
				else
					gg_send_message(sess, GG_CLASS_CHAT, uins[0], (unsigned char *)utmp);
#ifdef HAVE_OPENSSL
			}
#endif
		}
		writeMyMessage();
	}
	if (myLastFormats)
		delete (char *)myLastFormats;
	delete users;
	free(utmp);

	if (sess->check & GG_CHECK_WRITE)
		kadusnw->setEnabled(true);		
}

/* prunes messages */
void Chat::pruneWindow(void) {
	int index,occurences;

	occurences = 0;
	if (config.scrolldown) {
		index = -1;
		while (occurences != config.chatprunelen && totaloccurences > config.chatprunelen - 1) {
			index = body->text().findRev(QString("<TABLE"), index - 8);
			occurences++;
			}
		totaloccurences++;

		body->setText(body->text().right(body->text().length() - index));	
		}
	else {
		index = 0;
		while (occurences != config.chatprunelen && totaloccurences > config.chatprunelen ) {
			if (occurences == 0)
				index = body->text().find(QString("<TABLE"), 0);
			else
				index = body->text().find(QString("<TABLE"), index + 8);

			occurences++;
			}
		totaloccurences++;

		if (totaloccurences > config.chatprunelen && index != -1 && index != 0)
			body->setText(body->text().left(index));
		}
}

/* opens messages history */
void Chat::HistoryBox(void) {
	History *hb;

	hb = new History(uins);
	hb->show();
}

void Chat::insertEmoticon(void)
{
	if (emoticon_selector == NULL)
	{
		emoticon_selector = new EmoticonSelector(NULL, "Emoticon selector", this);
		emoticon_selector->alignTo(iconsel);
		emoticon_selector->show();
	}
	else
	{
		emoticon_selector->close();
		emoticon_selector = NULL;
	}
}

void Chat::changeColor(void)
{
	if (color_selector == NULL) {
		color_selector = new ColorSelector(this);
		color_selector->alignTo(colorbtn);
		color_selector->show();
		connect(color_selector, SIGNAL(colorSelect(QColor)), this, SLOT(colorChanged(QColor)));
		connect(color_selector, SIGNAL(aboutToClose()), this, SLOT(aboutToClose()));
		}
	else
		color_selector = NULL;
}

void Chat::aboutToClose() {
	kdebug("Chat::aboutToClose()\n");
	color_selector = NULL;
}

void Chat::colorChanged(QColor color) {
	color_selector = NULL;
	QPixmap p(16, 16);
	p.fill(color);
	colorbtn->setPixmap(p);
	edit->setColor(color);
	actcolor = color;
}

/* adds an emoticon code to the edit window */
void Chat::addEmoticon(QString string) {
	int para, index;

	if (string.length()) {
		string.replace(QRegExp("&lt;"), "<");
		string.replace(QRegExp("&gt;"), ">");
		edit->insert(string);
		edit->setFocus();
		}
	emoticon_selector = NULL;
}

ColorSelectorButton::ColorSelectorButton(QWidget* parent, const QColor& qcolor) : QToolButton(parent)
{
	QPixmap p(15,15);
	p.fill(qcolor);
	color = qcolor;
	setPixmap(p);
//	setAutoRaise(true);
	setMouseTracking(true);
	QToolTip::add(this,color.name());
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void ColorSelectorButton::buttonClicked()
{
	emit clicked(color);
};

ColorSelector::ColorSelector(QWidget *parent, const char *name) : QWidget (parent, name,Qt::WType_Popup)
{
	setWFlags(Qt::WDestructiveClose);

	QValueList<QColor> qcolors;
	int i;

	for (i = 0; i < 16; i++)
		qcolors.append(colors[i]);

	int selector_count=qcolors.count();
	int selector_width=(int)sqrt((double)selector_count);
	int btn_width=0;
	QGridLayout *grid = new QGridLayout(this, 0, selector_width, 0, 0);

	for(int i=0; i<selector_count; i++)
	{
		ColorSelectorButton* btn = new ColorSelectorButton(this, qcolors[i]);
		btn_width=btn->sizeHint().width();
		grid->addWidget(btn, i/selector_width, i%selector_width);
		connect(btn,SIGNAL(clicked(const QColor&)),this,SLOT(iconClicked(const QColor&)));
	};
}

void ColorSelector::iconClicked(const QColor& color)
{
	emit colorSelect(color);
	close();
};

void ColorSelector::closeEvent(QCloseEvent *e) {
	kdebug("ColorSelector::closeEvent()\n");
	emit aboutToClose();
	QWidget::closeEvent(e);
}

void ColorSelector::alignTo(QWidget* w)
{
	// oblicz pozycj� widgetu do kt�rego r�wnamy
	QPoint w_pos = w->mapToGlobal(QPoint(0,0));
	// oblicz rozmiar selektora
	QSize e_size = sizeHint();
	// oblicz rozmiar pulpitu
	QSize s_size = QApplication::desktop()->size();
	// oblicz dystanse od widgetu do lewego brzegu i do prawego
	int l_dist = w_pos.x();
	int r_dist = s_size.width() - (w_pos.x() + w->width());
	// oblicz pozycj� w zale�no�ci od tego czy po lewej stronie
	// jest wi�cej miejsca czy po prawej
	int x;
	if (l_dist >= r_dist)
		x = w_pos.x() - e_size.width();
	else
		x = w_pos.x() + w->width();
	// oblicz pozycj� y - centrujemy w pionie
	int y = w_pos.y() + w->height()/2 - e_size.height()/2;
	// je�li wychodzi poza doln� kraw�d� to r�wnamy do niej
	if (y + e_size.height() > s_size.height())
		y = s_size.height() - e_size.height();
	// je�li wychodzi poza g�rn� kraw�d� to r�wnamy do niej
	if (y < 0)
		y = 0;
	// ustawiamy selektor na wyliczonej pozycji
	move(x, y);
};
