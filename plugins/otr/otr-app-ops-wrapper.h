/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_APP_OPS_WRAPPER_H
#define OTR_APP_OPS_WRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

#include "chat/chat.h"

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class Contact;

class OtrContextConverter;
class OtrFingerprintService;
class OtrOpData;
class OtrOpDataFactory;
class OtrPeerIdentityVerificationService;
class OtrTrustLevelService;
class OtrUserStateService;

class OtrAppOpsWrapper : public QObject
{
	Q_OBJECT

	enum IsLoggedInStatus {
		NotSure = -1,
		NotLoggedIn = 0,
		LoggedIn = 1
	};

	friend void kadu_otr_create_privkey(void *, const char *, const char *);
	friend int kadu_otr_is_logged_in(void *, const char *, const char *, const char *);
	friend void kadu_otr_inject_message(void *, const char *, const char *, const char *, const char *);
	friend void kadu_otr_update_context_list(void *);
	friend void kadu_otr_write_fingerprints(void *);
	friend int kadu_otr_max_message_size(void *, ConnContext *);
	friend const char * kadu_otr_otr_error_message(void *, ConnContext *, OtrlErrorCode);
	friend const char * kadu_otr_resent_msg_prefix(void *, ConnContext *);
	friend void kadu_otr_handle_msg_event(void *, OtrlMessageEvent, ConnContext *, const char *, gcry_error_t);
	friend void kadu_otr_create_instag(void *, const char *, const char *);

	QWeakPointer<OtrContextConverter> ContextConverter;
	QWeakPointer<OtrFingerprintService> FingerprintService;
	QWeakPointer<OtrOpDataFactory> OpDataFactory;
	QWeakPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;
	QWeakPointer<OtrTrustLevelService> TrustLevelService;
	QWeakPointer<OtrUserStateService> UserStateService;
	OtrlMessageAppOps Ops;

	void createPrivateKey(OtrOpData *opData) const;
	IsLoggedInStatus isLoggedIn(OtrOpData *opData, const QString &contactId) const;
	void injectMessage(OtrOpData *opData, const QByteArray &messageContent) const;
	void updateContextList(OtrOpData *opData);
	void writeFingerprints();
	int maxMessageSize(OtrOpData *opData) const;
	QString errorMessage(OtrOpData *opData, OtrlErrorCode errorCode) const;
	QString resentMessagePrefix() const;
	void handleMsgEvent(OtrOpData *opData, OtrlMessageEvent event, const QString &message, gcry_error_t errorCode) const;
	QString messageString(OtrlMessageEvent event, const QString &message, gcry_error_t errorCode, const QString &peerDisplay) const;
	QString gpgErrorString(gcry_error_t errorCode) const;
	void createInstanceTag(OtrOpData *opData);
	QString instanceTagsFileName() const;

public:
	explicit OtrAppOpsWrapper();
	virtual ~OtrAppOpsWrapper();

	void setContextConverter(OtrContextConverter *contextConverter);
	void setFingerprintService(OtrFingerprintService *fingerprintService);
	void setOpDataFactory(OtrOpDataFactory *opDataFactory);
	void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);
	void setUserStateService(OtrUserStateService *userStateService);
	void setTrustLevelService(OtrTrustLevelService *trustLevelService);

	const OtrlMessageAppOps * ops() const;

};

#endif // OTR_APP_OPS_WRAPPER_H
