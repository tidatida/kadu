/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "model/kadu-abstract-model.h"
#include "exports.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QPointer>
#include <QtCore/QVector>
#include <injeqt/injeqt.h>

class ChatDataExtractor;
class ChatListMimeDataService;
class ContactDataExtractor;
class TalkableConverter;

class KADUAPI ChatListModel : public QAbstractItemModel, public KaduAbstractModel
{
	Q_OBJECT

public:
	explicit ChatListModel(QObject *parent = nullptr);
	virtual ~ChatListModel();

	void setChats(const QVector<Chat> &chats);
	void addChat(const Chat &chat);
	void removeChat(const Chat &chat);

	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QFlags<Qt::ItemFlag> flags(const QModelIndex &index) const;

	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	virtual Chat chatAt(const QModelIndex &index) const;
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

	// D&D
	virtual QStringList mimeTypes() const;
	virtual QMimeData * mimeData(const QModelIndexList &indexes) const;

private:
	QPointer<ChatDataExtractor> m_chatDataExtractor;
	QPointer<ChatListMimeDataService> m_chatListMimeDataService;
	QPointer<ContactDataExtractor> m_contactDataExtractor;
	QPointer<TalkableConverter> m_talkableConverter;

	QVector<Chat> List;

	void connectChat(const Chat &chat);
	void disconnectChat(const Chat &chat);

	Chat chatFromVariant(const QVariant &variant) const;

private slots:
	INJEQT_SET void setChatDataExtractor(ChatDataExtractor *chatDataExtractor);
	INJEQT_SET void setChatListMimeDataService(ChatListMimeDataService *chatListMimeDataService);
	INJEQT_SET void setContactDataExtractor(ContactDataExtractor *contactDataExtractor);
	INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);

	void contactAboutToBeAdded(const Contact &contact);
	void contactAdded(const Contact &contact);
	void contactAboutToBeRemoved(const Contact &contact);
	void contactRemoved(const Contact &contact);

	void chatUpdated();

};
