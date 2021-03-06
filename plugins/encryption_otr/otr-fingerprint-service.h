/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Account;
class Contact;
class OtrContextConverter;
class OtrPathService;
class OtrUserStateService;

class OtrFingerprintService : public QObject
{
	Q_OBJECT

public:
	static void wrapperOtrWriteFingerprints(void *data);

	enum Trust
	{
		TrustNotVerified,
		TrustVerified
	};

	Q_INVOKABLE OtrFingerprintService();
	virtual ~OtrFingerprintService();

	void readFingerprints() const;
	void writeFingerprints() const;

	void setContactFingerprintTrust(const Contact &contact, Trust trust) const;
	Trust contactFingerprintTrust(const Contact &contact) const;

	QString extractAccountFingerprint(const Account &account) const;
	QString extractContactFingerprint(const Contact &contact) const;

signals:
	void fingerprintsUpdated() const;

private slots:
	INJEQT_SET void setContextConverter(OtrContextConverter *contextConverter);
	INJEQT_SET void setPathService(OtrPathService *pathService);
	INJEQT_SET void setUserStateService(OtrUserStateService *userStateService);

private:
	QPointer<OtrContextConverter> ContextConverter;
	QPointer<OtrPathService> PathService;
	QPointer<OtrUserStateService> UserStateService;

	QString fingerprintsStoreFileName() const;

};
