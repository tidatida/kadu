/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "avatars/avatar-manager.h"

#include "iris/xmpp_tasks.h"
#include "services/jabber-pep-service.h"
#include "base64.h"
#include "jabber-protocol.h"

#include "jabber-avatar-pep-fetcher.h"

#define XMLNS_AVATAR_DATA "urn:xmpp:avatar:data"
#define XMLNS_AVATAR_METADATA "urn:xmpp:avatar:metadata"
#define NS_AVATAR_DATA "http://www.xmpp.org/extensions/xep-0084.html#ns-data"

JabberAvatarPepFetcher::JabberAvatarPepFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}

JabberAvatarPepFetcher::~JabberAvatarPepFetcher()
{
	if (DiscoItems)
	{
		XMPP::JT_DiscoItems *discoItems = DiscoItems.data();
		DiscoItems.clear();
		disconnect(discoItems, 0, this, 0);
		delete discoItems;
	}
}

void JabberAvatarPepFetcher::done()
{
	emit avatarFetched(MyContact, true);
}

void JabberAvatarPepFetcher::failed()
{
	emit avatarFetched(MyContact, false);
}

void JabberAvatarPepFetcher::fetchAvatar()
{
	JabberProtocol *jabberProtocol = qobject_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!jabberProtocol || !jabberProtocol->isConnected() || !jabberProtocol->xmppClient() || !jabberProtocol->xmppClient()->rootTask() ||
		!jabberProtocol->pepService()->enabled())
	{
		failed();
		deleteLater();
		return;
	}

	DiscoItems = new XMPP::JT_DiscoItems(jabberProtocol->xmppClient()->rootTask());
	connect(DiscoItems.data(), SIGNAL(destroyed()), this, SLOT(deleteLater()));
	connect(DiscoItems.data(), SIGNAL(finished()), this, SLOT(discoItemsFinished()));
	DiscoItems.data()->get(MyContact.id());
	DiscoItems.data()->go();
}

void JabberAvatarPepFetcher::discoItemsFinished()
{
	XMPP::DiscoList result = DiscoItems.data()->items();

	bool hasAvatar = false;
	foreach (const XMPP::DiscoItem &item, result)
		if (item.node() == XMLNS_AVATAR_DATA || item.node() == NS_AVATAR_DATA)
		{
			hasAvatar = true;
			break;
		}

	if (!hasAvatar)
	{
		failed();
		deleteLater();
		return;
	}

	JabberProtocol *jabberProtocol = qobject_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (jabberProtocol)
	{
		connect(jabberProtocol->pepService(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarMetadataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
		jabberProtocol->pepService()->get(MyContact.id(), XMLNS_AVATAR_METADATA, "");
	}
}

void JabberAvatarPepFetcher::avatarMetadataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item)
{
	if (jid.bare() != MyContact.id() || node != XMLNS_AVATAR_METADATA)
		return; // not our data :(

	AvatarId = item.id();
	if (AvatarId == "current")
	{
		Avatar contactAvatar = AvatarManager::instance()->byContact(MyContact, ActionCreateAndAdd);
		contactAvatar.setLastUpdated(QDateTime::currentDateTime());
		contactAvatar.setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));
		contactAvatar.setPixmap(QPixmap());

		done();
		deleteLater();
		return;
	}

	JabberProtocol *jabberProtocol = qobject_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (jabberProtocol)
	{
		disconnect(jabberProtocol->pepService(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarMetadataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
		connect(jabberProtocol->pepService(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarDataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
		jabberProtocol->pepService()->get(MyContact.id(), XMLNS_AVATAR_DATA, item.id());
	}
}

void JabberAvatarPepFetcher::avatarDataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item)
{
	if (jid.bare() != MyContact.id() || node != XMLNS_AVATAR_DATA || item.id() != AvatarId)
		return; // not our data :(

	JabberProtocol *jabberProtocol = qobject_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (jabberProtocol)
		disconnect(jabberProtocol->pepService(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarDataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));

	QByteArray imageData = XMPP::Base64::decode(item.payload().text());

	Avatar contactAvatar = AvatarManager::instance()->byContact(MyContact, ActionCreateAndAdd);
	contactAvatar.setLastUpdated(QDateTime::currentDateTime());
	contactAvatar.setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));

	QPixmap pixmap;

	if (!imageData.isEmpty())
		pixmap.loadFromData(imageData);

	contactAvatar.setPixmap(pixmap);

	done();
	deleteLater();
}
