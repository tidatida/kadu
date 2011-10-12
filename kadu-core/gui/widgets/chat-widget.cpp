/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtGui/QIcon>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QShortcut>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "buddies/buddy.h"
#include "buddies/buddy-set.h"
#include "chat/chat-geometry-data.h"
#include "chat/chat-manager.h"
#include "chat/message/message-render-info.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/chat-configuration-holder.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "contacts/model/contact-data-extractor.h"
#include "contacts/model/contact-list-model.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/chat-edit-box-size-manager.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/color-selector.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "parser/parser.h"
#include "protocols/protocol.h"

#include "activate.h"
#include "chat-edit-box.h"
#include "custom-input.h"
#include "debug.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"

#include "chat-widget.h"

ChatWidget::ChatWidget(const Chat &chat, QWidget *parent) :
		QWidget(parent), CurrentChat(chat),
		BuddiesWidget(0), InputBox(0), HorizontalSplitter(0),
		IsComposing(false), CurrentContactActivity(ChatStateService::StateNone),
		SplittersInitialized(false), NewMessagesCount(0)
{
	kdebugf();

	setAcceptDrops(true);
	ChatWidgetManager::instance()->registerChatWidget(this);

	createGui();
	configurationUpdated();

	ComposingTimer.setInterval(2 * 1000);
	connect(&ComposingTimer, SIGNAL(timeout()), this, SLOT(checkComposing()));

	connect(edit(), SIGNAL(textChanged()), this, SLOT(updateComposing()));

	foreach (const Contact &contact, CurrentChat.contacts())
	{
		connect(contact, SIGNAL(updated()), this, SLOT(refreshTitle()));
		connect(contact.ownerBuddy(), SIGNAL(updated()), this, SLOT(refreshTitle()));
	}

	// icon for conference never changes
	if (CurrentChat.contacts().count() == 1)
	{
		foreach (const Contact &contact, CurrentChat.contacts())
		{
			// actually we only need to send iconChanged() on CurrentStatus update
			// but we don't have a signal for that in ContactShared
			// TODO 0.10.0: consider adding currentStatusChanged() signal to ContactShared
			connect(contact, SIGNAL(updated()), this, SIGNAL(iconChanged()));
			connect(contact.ownerBuddy(), SIGNAL(buddySubscriptionChanged()), this, SIGNAL(iconChanged()));
		}

		if (currentProtocol() && currentProtocol()->chatStateService())
			connect(currentProtocol()->chatStateService(), SIGNAL(contactActivityChanged(ChatStateService::ContactActivity, const Contact &)),
					this, SLOT(contactActivityChanged(ChatStateService::ContactActivity, const Contact &)));
	}

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SIGNAL(iconChanged()));

	kdebugf2();
}

ChatWidget::~ChatWidget()
{
	kdebugf();
	ComposingTimer.stop();

	ChatWidgetManager::instance()->unregisterChatWidget(this);

	if (currentProtocol() && currentProtocol()->chatStateService())
		currentProtocol()->chatStateService()->chatWidgetClosed(chat());

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed\n");
}

void ChatWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);

	VerticalSplitter = new QSplitter(Qt::Vertical, this);

#ifdef Q_OS_MAC
	/* Dorr: workaround for mac tabs issue */
	VerticalSplitter->setAutoFillBackground(true);
#endif

	mainLayout->addWidget(VerticalSplitter);

	HorizontalSplitter = new QSplitter(Qt::Horizontal, this);
	HorizontalSplitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	HorizontalSplitter->setMinimumHeight(10);

	MessagesView = new ChatMessagesView(CurrentChat);

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_PageUp + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), MessagesView, SLOT(pageUp()));

	shortcut = new QShortcut(QKeySequence(Qt::Key_PageDown + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), MessagesView, SLOT(pageDown()));
	HorizontalSplitter->addWidget(MessagesView);

	if (CurrentChat.contacts().count() > 1)
		createContactsList();

	InputBox = new ChatEditBox(CurrentChat, this);
	InputBox->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
	InputBox->setMinimumHeight(10);

	VerticalSplitter->addWidget(HorizontalSplitter);
	VerticalSplitter->setStretchFactor(0, 1);
	VerticalSplitter->addWidget(InputBox);
	VerticalSplitter->setStretchFactor(1, 0);

	connect(VerticalSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(verticalSplitterMoved(int, int)));
	connect(InputBox->inputBox(), SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(InputBox->inputBox(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
			this, SLOT(keyPressedSlot(QKeyEvent *, CustomInput *, bool &)));
}

void ChatWidget::createContactsList()
{
	QWidget *contactsListContainer = new QWidget(HorizontalSplitter);

	QVBoxLayout *layout = new QVBoxLayout(contactsListContainer);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	BuddiesWidget = new BuddiesListWidget(BuddiesListWidget::FilterAtTop, this);
	BuddiesWidget->setShowAnonymous(true);
	BuddiesWidget->view()->setItemsExpandable(false);
	BuddiesWidget->setMinimumSize(QSize(30, 30));
	BuddiesWidget->view()->setModel(new ContactListModel(CurrentChat.contacts().toContactVector(), this));
	BuddiesWidget->view()->setRootIsDecorated(false);
	BuddiesWidget->view()->setShowAccountName(false);
	BuddiesWidget->view()->setContextMenuEnabled(true);

	connect(BuddiesWidget->view(), SIGNAL(chatActivated(Chat)),
			Core::instance()->kaduWindow(), SLOT(openChatWindow(Chat)));

	QPushButton *leaveConference = new QPushButton(tr("Leave\nconference"), contactsListContainer);
	leaveConference->setStyleSheet("text-align: center;");
	leaveConference->setMinimumWidth(BuddiesWidget->minimumWidth());
	connect(leaveConference, SIGNAL(clicked()), this, SLOT(leaveConference()));

	layout->addWidget(BuddiesWidget);
	layout->addWidget(leaveConference);

	QList<int> sizes;
	sizes.append(3);
	sizes.append(1);
	HorizontalSplitter->setSizes(sizes);
}

void ChatWidget::configurationUpdated()
{
	InputBox->inputBox()->setFont(ChatConfigurationHolder::instance()->chatFont());
	QString style;
	QColor color = qApp->palette().text().color();
	if (ChatConfigurationHolder::instance()->chatTextCustomColors())
	{
		style = QString("background-color:%1;").arg(ChatConfigurationHolder::instance()->chatTextBgColor().name());
		color = ChatConfigurationHolder::instance()->chatTextFontColor();
	}
	InputBox->inputBox()->viewport()->setStyleSheet(style);
	QPalette palette = InputBox->inputBox()->palette();
	palette.setBrush(QPalette::Text, color);
	InputBox->inputBox()->setPalette(palette);

	refreshTitle();
}

bool ChatWidget::keyPressEventHandled(QKeyEvent *e)
{
	if (e->matches(QKeySequence::Copy) && !MessagesView->selectedText().isEmpty())
	{
		MessagesView->triggerPageAction(QWebPage::Copy);
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
	{
		clearChatWindow();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
	{
		emit closed();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
	{
		Actions::instance()->createAction("lookupUserInfoAction", InputBox)->activate(QAction::Trigger);
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "kadu_openchatwith"))
	{
		Actions::instance()->createAction("openChatWithAction", InputBox)->activate(QAction::Trigger);
		return true;
	}

	return false;
}

void ChatWidget::keyPressEvent(QKeyEvent *e)
{
	kdebugf();
	if (keyPressEventHandled(e))
		e->accept();
	else
		QWidget::keyPressEvent(e);
	kdebugf2();
}

void ChatWidget::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);

	if (ChatEditBoxSizeManager::instance()->initialized())
		commonHeightChanged(ChatEditBoxSizeManager::instance()->commonHeight());
}

void ChatWidget::refreshTitle()
{
	kdebugf();
	QString title;

	int contactsCount = chat().contacts().count();
	kdebugmf(KDEBUG_FUNCTION_START, "chat().contacts().size() = %d\n", contactsCount);
	if (contactsCount > 1)
	{
		title = ChatConfigurationHolder::instance()->conferencePrefix();
		if (title.isEmpty())
			title = tr("Conference with ");

		QString conferenceContents = ChatConfigurationHolder::instance()->conferenceContents();
		QStringList contactslist;
		foreach (Contact contact, chat().contacts())
			contactslist.append(Parser::parse(conferenceContents.isEmpty() ? "%a" : conferenceContents, BuddyOrContact(contact), false));

		title.append(contactslist.join(", "));
	}
	else if (contactsCount == 1)
	{
		Contact contact = chat().contacts().toContact();

		if (ChatConfigurationHolder::instance()->chatContents().isEmpty())
		{
			if (contact.ownerBuddy().isAnonymous())
				title = Parser::parse(tr("Chat with ") + "%a", BuddyOrContact(contact), false);
			else
				title = Parser::parse(tr("Chat with ") + "%a (%s[: %d])", BuddyOrContact(contact), false);
		}
		else
			title = Parser::parse(ChatConfigurationHolder::instance()->chatContents(), BuddyOrContact(contact), false);

		if (ChatConfigurationHolder::instance()->contactStateWindowTitle())
		{
			QString message;
			if (CurrentContactActivity == ChatStateService::StateComposing)
			{
				if (ChatConfigurationHolder::instance()->contactStateWindowTitleComposingSyntax().isEmpty())
					message = tr("(Composing...)");
				else
					message = ChatConfigurationHolder::instance()->contactStateWindowTitleComposingSyntax();
			}
			else if (CurrentContactActivity == ChatStateService::StateInactive)
				message = tr("(Inactive)");
			title = ChatConfigurationHolder::instance()->contactStateWindowTitlePosition() == 0 ? message + " " + title : title + " "  + message;
		}
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	setTitle(title);

	kdebugf2();
}

void ChatWidget::setTitle(const QString &title)
{
	if (title != Title)
	{
		Title = title;
		emit titleChanged(this, title);
	}
}

QIcon ChatWidget::icon()
{
	int contactsCount = chat().contacts().count();
	if (contactsCount == 1)
	{
		Contact contact = chat().contacts().toContact();
		if (contact)
			return ContactDataExtractor::data(contact, Qt::DecorationRole, false).value<QIcon>();
	}
	else if (contactsCount > 1)
		return ChatTypeManager::instance()->chatType("Conference")->icon().icon();

	return KaduIcon("internet-group-chat").icon();
}

void ChatWidget::appendMessages(const QList<MessageRenderInfo *> &messages, bool pending)
{
	MessagesView->appendMessages(messages);

	if (pending)
		LastMessageTime = QDateTime::currentDateTime();
}

void ChatWidget::appendMessage(MessageRenderInfo *message, bool pending)
{
	MessagesView->appendMessage(message);

	if (pending)
		LastMessageTime = QDateTime::currentDateTime();
}

void ChatWidget::appendSystemMessage(const QString &rawContent, const QString &backgroundColor, const QString &fontColor)
{
	Message message = Message::create();
	message.setMessageChat(CurrentChat);
	message.setType(MessageTypeSystem);
	message.setContent(rawContent);
	message.setSendDate(QDateTime::currentDateTime());
	MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);
	messageRenderInfo->setBackgroundColor(backgroundColor)
		.setFontColor(fontColor)
		.setNickColor(fontColor);

	MessagesView->appendMessage(messageRenderInfo);
}

/* invoked from outside when new message arrives, this is the window to the world */
void ChatWidget::newMessage(MessageRenderInfo *messageRenderInfo)
{
	MessagesView->appendMessage(messageRenderInfo);

	LastMessageTime = QDateTime::currentDateTime();
	NewMessagesCount++;

 	emit messageReceived(CurrentChat);
}

void ChatWidget::resetEditBox()
{
	InputBox->inputBox()->clear();

	Action *action;
	action = ChatWidgetManager::instance()->actions()->bold()->action(InputBox);
	if (action)
		InputBox->inputBox()->setFontWeight(action->isChecked() ? QFont::Bold : QFont::Normal);

	action = ChatWidgetManager::instance()->actions()->italic()->action(InputBox);
	if (action)
		InputBox->inputBox()->setFontItalic(action->isChecked());

	action = ChatWidgetManager::instance()->actions()->underline()->action(InputBox);
	if (action)
		InputBox->inputBox()->setFontUnderline(action->isChecked());
}

void ChatWidget::clearChatWindow()
{
	kdebugf();
	if (!config_file.readBoolEntry("Chat", "ConfirmChatClear") || MessageDialog::ask(KaduIcon("dialog-question"), tr("Kadu"), tr("Chat window will be cleared. Continue?")))
	{
		MessagesView->clearMessages();
		activateWindow();
	}
	kdebugf2();
}

/* sends the message typed */
void ChatWidget::sendMessage()
{
	kdebugf();
	if (InputBox->inputBox()->toPlainText().isEmpty())
	{
		kdebugf2();
		return;
	}

	emit messageSendRequested(this);

	if (!currentProtocol())
		return;

	if (!currentProtocol()->isConnected())
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("Cannot send message while being offline.") + tr("Account:") + chat().chatAccount().id(),
				QMessageBox::Ok, this);
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}

	FormattedMessage message = FormattedMessage::parse(InputBox->inputBox()->document());
	ChatService *chatService = currentProtocol()->chatService();
	if (!chatService || !chatService->sendMessage(CurrentChat, message))
		return;

	resetEditBox();

	// We sent the message and reseted the edit box, so composing of that message is done.
	// Note that if ComposingTimer is not active, it means that we already reported
	// composing had stopped.
	if (ComposingTimer.isActive())
		composingStopped();

	emit messageSentAndConfirmed(CurrentChat, message.toHtml());

	emit messageSent(this);
	kdebugf2();
}

void ChatWidget::colorSelectorAboutToClose()
{
	kdebugf();
	kdebugf2();
}

CustomInput * ChatWidget::edit() const
{
	return InputBox ? InputBox->inputBox() : 0;
}

BuddiesListView * ChatWidget::contactsListWidget() const
{
	return BuddiesWidget ? BuddiesWidget->view() : 0;
}

unsigned int ChatWidget::countMessages() const
{
	return MessagesView ? MessagesView->countMessages() : 0;
}

bool ChatWidget::decodeLocalFiles(QDropEvent *event, QStringList &files)
{
	if (!event->mimeData()->hasUrls() || event->source() == MessagesView)
		return false;

	QList<QUrl> urls = event->mimeData()->urls();

	foreach (const QUrl &url, urls)
	{
		QString file = url.toLocalFile();
		if (!file.isEmpty())
		{
			//is needed to check if file refer to local file?
			QFileInfo fileInfo(file);
			if (fileInfo.exists())
				files.append(file);
		}
	}
	return !files.isEmpty();

}

void ChatWidget::dragEnterEvent(QDragEnterEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
		e->acceptProposedAction();

}

void ChatWidget::dragMoveEvent(QDragMoveEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
		e->acceptProposedAction();
}

void ChatWidget::dropEvent(QDropEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
	{
		e->acceptProposedAction();

		QStringList::const_iterator i = files.constBegin();
		QStringList::const_iterator end = files.constEnd();

		for (; i != end; ++i)
			emit fileDropped(CurrentChat, *i);
	}
}

Protocol *ChatWidget::currentProtocol() const
{
	return CurrentChat.chatAccount().protocolHandler();
}

void ChatWidget::makeActive()
{
	kdebugf();
	QWidget *win = this->window();
	_activateWindow(win);
	kdebugf2();
}

void ChatWidget::markAllMessagesRead()
{
	NewMessagesCount = 0;
}

void ChatWidget::verticalSplitterMoved(int pos, int index)
{
	Q_UNUSED(pos)
	Q_UNUSED(index)

	if (SplittersInitialized)
		ChatEditBoxSizeManager::instance()->setCommonHeight(VerticalSplitter->sizes().at(1));
}

void ChatWidget::kaduRestoreGeometry()
{
	if (!HorizontalSplitter)
		return;

	if (!chat())
		return;

	ChatGeometryData *cgd = chat().data()->moduleStorableData<ChatGeometryData>("chat-geometry", ChatWidgetManager::instance(), false);
	if (!cgd)
		return;

	QList<int> horizSizes = cgd->widgetHorizontalSizes();
	if (!horizSizes.isEmpty())
		HorizontalSplitter->setSizes(horizSizes);
}

void ChatWidget::kaduStoreGeometry()
{
	if (!HorizontalSplitter)
		return;

	if (!chat())
		return;

	ChatGeometryData *cgd = chat().data()->moduleStorableData<ChatGeometryData>("chat-geometry", ChatWidgetManager::instance(), true);
	if (!cgd)
		return;

	cgd->setWidgetHorizontalSizes(HorizontalSplitter->sizes());
	cgd->ensureStored();
}

void ChatWidget::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	if (!SplittersInitialized)
		QMetaObject::invokeMethod(this, "setUpVerticalSizes", Qt::QueuedConnection);
}

void ChatWidget::setUpVerticalSizes()
{
	// now we can accept this signal
	connect(ChatEditBoxSizeManager::instance(), SIGNAL(commonHeightChanged(int)), this, SLOT(commonHeightChanged(int)));

	// already set up by other window, so we use this window setting
	if (ChatEditBoxSizeManager::instance()->initialized())
	{
		commonHeightChanged(ChatEditBoxSizeManager::instance()->commonHeight());
		SplittersInitialized = true;
		return;
	}

	ChatGeometryData *cgd = chat().data()->moduleStorableData<ChatGeometryData>("chat-geometry", ChatWidgetManager::instance(), false);
	// no window has set up common height yet, so we use this data
	QList<int> vertSizes;
	if (cgd)
		vertSizes = cgd->widgetVerticalSizes();

	// if we dont have default values, we just make some up!
	if (vertSizes.count() != 2 || vertSizes.at(0) == 0 || vertSizes.at(1) == 0)
	{
		int h = height() / 3;
		vertSizes.clear();
		vertSizes.append(h * 2);
		vertSizes.append(h);
	}
	VerticalSplitter->setSizes(vertSizes);
	SplittersInitialized = true;
	ChatEditBoxSizeManager::instance()->setCommonHeight(vertSizes.at(1));
}

void ChatWidget::commonHeightChanged(int commonHeight)
{
	QList<int> sizes = VerticalSplitter->sizes();

	int sum = 0;
	if (2 == sizes.count())
	{
		if (sizes.at(1) == commonHeight)
			return;
		sum = sizes.at(0) + sizes.at(1);
	}
	else
		sum = height();

	if (sum < commonHeight)
		commonHeight = sum / 3;

	sizes.clear();
	sizes.append(sum - commonHeight);
	sizes.append(commonHeight);
	VerticalSplitter->setSizes(sizes);
}

void ChatWidget::composingStopped()
{
	ComposingTimer.stop();
	IsComposing = false;

	if (currentProtocol() && currentProtocol()->chatStateService())
		currentProtocol()->chatStateService()->composingStopped(chat());
}

void ChatWidget::checkComposing()
{
	if (!IsComposing)
		composingStopped();
	else
		// Reset IsComposing to false, so if updateComposing() method doesn't set it to true
		// before ComposingTimer hits this method again, we will call composingStopped().
		IsComposing = false;
}

void ChatWidget::updateComposing()
{
	if (!currentProtocol() || !currentProtocol()->chatStateService())
		return;

	if (!ComposingTimer.isActive())
	{
		// If the text was deleted either by sending a message or explicitly by the user,
		// let's not report it as composing.
		if (edit()->toPlainText().isEmpty())
			return;

		currentProtocol()->chatStateService()->composingStarted(chat());

		ComposingTimer.start();
	}
	IsComposing = true;
}

void ChatWidget::contactActivityChanged(ChatStateService::ContactActivity state, const Contact &contact)
{
	if (CurrentContactActivity == state)
		return;

	if (!CurrentChat.contacts().contains(contact))
		return;

	CurrentContactActivity = state;

	if (ChatConfigurationHolder::instance()->contactStateChats())
		MessagesView->contactActivityChanged(state, contact);

	if (ChatConfigurationHolder::instance()->contactStateWindowTitle())
		refreshTitle();

	if (CurrentContactActivity == ChatStateService::StateGone)
	{
		QString msg = "[ " + tr("%1 ended the conversation").arg(contact.ownerBuddy().display()) + " ]";
		Message message = Message::create();
		message.setMessageChat(CurrentChat);
		message.setType(MessageTypeSystem);
		message.setMessageSender(contact);
		message.setContent(msg);
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		MessagesView->appendMessage(message);
	}
}

void ChatWidget::leaveConference()
{
	if (!MessageDialog::ask(KaduIcon("dialog-warning"), tr("Kadu"), tr("All messages received in this conference will be ignored\nfrom now on. Are you sure you want to leave this conference?"), this))
		return;

	if (CurrentChat)
		CurrentChat.setIgnoreAllMessages(true);

	emit closed();
}

void ChatWidget::keyPressedSlot(QKeyEvent *e, CustomInput *input, bool &handled)
{
	Q_UNUSED(input)

	if (handled)
		return;

	handled = keyPressEventHandled(e);
}
