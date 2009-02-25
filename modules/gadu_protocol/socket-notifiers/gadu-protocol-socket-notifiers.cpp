/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSocketNotifier>

#include <arpa/inet.h>
#include <libgadu.h>

#include "accounts/account.h"

#include "contacts/ignored-helper.h"

#include "config_file.h"
#include "debug.h"
#include "misc.h"

#include "gadu.h"

#include "gadu-protocol-socket-notifiers.h"

void GaduProtocolSocketNotifiers::watchFor(gg_session *sess)
{
	Sess = sess;
	GaduSocketNotifiers::watchFor(Sess ? Sess->fd : 0);
}

bool GaduProtocolSocketNotifiers::checkRead()
{
	return Sess->check & GG_CHECK_READ;
}

bool GaduProtocolSocketNotifiers::checkWrite()
{
	return Sess->check & GG_CHECK_WRITE;
}

void GaduProtocolSocketNotifiers::dumpConnectionState()
{
	switch (Sess->state)
	{
		case GG_STATE_RESOLVING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Resolving address\n");
			break;
		case GG_STATE_CONNECTING_HUB:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connecting to hub\n");
			break;
		case GG_STATE_READING_DATA:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Fetching data from hub\n");
			break;
		case GG_STATE_CONNECTING_GG:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connecting to server\n");
			break;
		case GG_STATE_READING_KEY:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Waiting for hash key\n");
			break;
		case GG_STATE_READING_REPLY:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Sending key\n");
			break;
		case GG_STATE_CONNECTED:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "connected\n");
			break;
		case GG_STATE_IDLE:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "idle!\n");
			break;
		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "state==error! error=%d\n", Sess->error);
			break;
		default:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown state! state=%d\n", Sess->state);
			break;
	}
}

void GaduProtocolSocketNotifiers::handleEventMsg(struct gg_event *e)
{
	Contact sender(CurrentAccount->getContactById(QString::number(e->event.msg.sender)));
	ContactList recipients;

	if (GG_CLASS_CTCP == e->event.msg.msgclass)
	{
		if (config_file.readBoolEntry("Network", "AllowDCC") &&
				!IgnoredHelper::isIgnored(sender) &&
				!sender.isAnonymous())
			emit dccConnectionRequestReceived(sender);

		return;
	}

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);
	for (int i = 0; i < e->event.msg.recipients_count; ++i)
	{
		Contact recipient = CurrentAccount->getContactById(QString::number(e->event.msg.recipients[i]));
		recipients.append(recipient);
	}

	QString message((char*)e->event.msg.message);
	QByteArray formats((const char*)e->event.msg.formats, e->event.msg.formats_length);

	emit messageReceived(sender, recipients, message, e->event.msg.time, formats);
}

void GaduProtocolSocketNotifiers::handleEventNotify(struct gg_event *e)
{
	struct gg_notify_reply *notify = (GG_EVENT_NOTIFY_DESCR == e->type)
		? e->event.notify_descr.notify
		: e->event.notify;

	while (notify->uin)
	{
		QString description = (GG_EVENT_NOTIFY_DESCR == e->type)
			? QString(e->event.notify_descr.descr)
			: QString::null;

		CurrentProtocol->socketContactStatusChanged(notify->uin, notify->status, description,
				QHostAddress((unsigned int)ntohl(notify->remote_ip)), notify->remote_port, 0, notify->version);
		notify++;
	}
}

void GaduProtocolSocketNotifiers::handleEventStatus(struct gg_event *e)
{
	if (GG_EVENT_STATUS60 == e->type)
		CurrentProtocol->socketContactStatusChanged(e->event.status60.uin, e->event.status60.status, QString(e->event.status60.descr),
				QHostAddress((unsigned int)ntohl(e->event.status60.remote_ip)), e->event.status60.remote_port,
				e->event.status60.image_size, e->event.status60.version);
	else
		CurrentProtocol->socketContactStatusChanged(e->event.status.uin, e->event.status.status, QString(e->event.status.descr),
				QHostAddress(), 0, 0, 0);
}

void GaduProtocolSocketNotifiers::handleEventAck(struct gg_event *e)
{
	CurrentProtocol->CurrentChatService->socketAckReceived(e->event.ack.recipient, e->event.ack.seq, e->event.ack.status);
}

void GaduProtocolSocketNotifiers::handleEventConnFailed(struct gg_event *e)
{
	GaduProtocol::GaduError err;

	switch (e->event.failure)
	{
		case GG_FAILURE_RESOLVING:   err = GaduProtocol::ConnectionServerNotFound; break;
		case GG_FAILURE_CONNECTING:  err = GaduProtocol::ConnectionCannotConnect; break;
		case GG_FAILURE_NEED_EMAIL:  err = GaduProtocol::ConnectionNeedEmail; break;
		case GG_FAILURE_INVALID:     err = GaduProtocol::ConnectionInvalidData; break;
		case GG_FAILURE_READING:     err = GaduProtocol::ConnectionCannotRead; break;
		case GG_FAILURE_WRITING:     err = GaduProtocol::ConnectionCannotWrite; break;
		case GG_FAILURE_PASSWORD:    err = GaduProtocol::ConnectionIncorrectPassword; break;
		case GG_FAILURE_TLS:         err = GaduProtocol::ConnectionTlsError; break;
		case GG_FAILURE_INTRUDER:    err = GaduProtocol::ConnectionIntruderError; break;
		case GG_FAILURE_UNAVAILABLE: err = GaduProtocol::ConnectionUnavailableError; break;

		default:
			kdebugm(KDEBUG_ERROR, "ERROR: unhandled/unknown connection error! %d\n", e->event.failure);
			err = GaduProtocol::ConnectionUnknow;
	}

	CurrentProtocol->socketConnFailed(err);

	// we don't have connection anymore
	watchFor(0);
}

void GaduProtocolSocketNotifiers::handleEventConnSuccess(struct gg_event *e)
{
	CurrentProtocol->socketConnSuccess();
}

void GaduProtocolSocketNotifiers::handleEventDisconnect(struct gg_event *e)
{
	CurrentProtocol->socketDisconnected();

	// close connection
	gg_logoff(Sess);
	// we don't have connection anymore
	watchFor(0);
}

void GaduProtocolSocketNotifiers::socketEvent()
{
	kdebugf();

	++socketEventCalls;
	if (socketEventCalls > 1)
		kdebugm(KDEBUG_WARNING, "************* GaduProtocolSocketNotifiers::socketEvent(): Recursive socketEvent calls detected!\n");

	gg_event* e;
	if (!(e = gg_watch_fd(Sess)))
	{
		emit error(GaduProtocol::ConnectionUnknow);
//		gg_free_event(e);//nulla nie zwalniamy, bo i po co?
		--socketEventCalls;
		return;
	}

	if (Sess->state == GG_STATE_CONNECTING_HUB || Sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");
		watchFor(Sess); // maybe fd has changed
	}

	dumpConnectionState();
	printf("main event: %d\n", e->type);

	switch (e->type)
	{
		case GG_EVENT_MSG:
			handleEventMsg(e);
			break;

		case GG_EVENT_NOTIFY:
		case GG_EVENT_NOTIFY_DESCR:
			handleEventNotify(e);
			break;

		case GG_EVENT_STATUS:
		case GG_EVENT_STATUS60:
			handleEventStatus(e);
			break;

		case GG_EVENT_ACK:
			handleEventAck(e);
			break;

		case GG_EVENT_CONN_FAILED:
			handleEventConnFailed(e);
			break;

		case GG_EVENT_CONN_SUCCESS:
			handleEventConnSuccess(e);
			break;

		case GG_EVENT_DISCONNECT:
			handleEventDisconnect(e);
			break;

		case GG_EVENT_IMAGE_REQUEST:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image request received\n");
			emit imageRequestReceived(
				e->event.image_request.sender,
				e->event.image_request.size,
				e->event.image_request.crc32);
			break;

		case GG_EVENT_IMAGE_REPLY:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image reply received\n");
			emit imageReceived(
				e->event.image_reply.sender,
				e->event.image_reply.size,
				e->event.image_reply.crc32,
				e->event.image_reply.filename,
				e->event.image_reply.image);
			break;

		case GG_EVENT_NOTIFY60:
			emit userlistReceived(e);
			break;

		case GG_EVENT_PUBDIR50_SEARCH_REPLY:
		case GG_EVENT_PUBDIR50_READ:
		case GG_EVENT_PUBDIR50_WRITE:
			emit pubdirReplyReceived(e->event.pubdir50);
			break;

		case GG_EVENT_USERLIST:
			emit userlistReplyReceived(e->event.userlist.type, e->event.userlist.reply);
			break;

		case GG_EVENT_NONE:
			kdebugm(KDEBUG_NETWORK, "GG_EVENT_NONE\n");
			break;

		case GG_EVENT_DCC7_NEW:
			emit dcc7New(e->event.dcc7_new);
			break;

		case GG_EVENT_DCC7_ACCEPT:
			emit dcc7Accepted(e->event.dcc7_accept.dcc7);
			break;

		case GG_EVENT_DCC7_REJECT:
			emit dcc7Rejected(e->event.dcc7_reject.dcc7);
			break;

		case GG_EVENT_XML_EVENT:
			kdebugm(KDEBUG_NETWORK, "GG_EVENT_XML_EVENT\n");
			break;
	}


/*
	if (!broken && g->sess->state != GG_STATE_IDLE && g->sess->state != GG_STATE_ERROR) {
		watch_t *w;
		if ((watch == g->sess->check) && g->sess->fd == fd) {
			if ((w = watch_find(&gg_plugin, fd, watch)))
				watch_timeout_set(w, g->sess->timeout);
			else debug("[gg] watches managment went to hell?\n");
			gg_event_free(e);
			return 0;
		}
		w = watch_add_session(s, g->sess->fd, g->sess->check, gg_session_handler);
		watch_timeout_set(w, g->sess->timeout);
	}*/

	gg_free_event(e);
	--socketEventCalls;
	kdebugf2();
}

int GaduProtocolSocketNotifiers::timeout()
{
	return Sess
		? Sess->timeout
		: 0;
}
