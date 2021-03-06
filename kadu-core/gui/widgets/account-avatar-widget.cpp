/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "account-avatar-widget.h"

#include "avatars/avatar-manager.h"
#include "avatars/avatar.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "protocols/services/avatar-service.h"
#include "protocols/services/avatar-uploader.h"

#include <QtGui/QMovie>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

AccountAvatarWidget::AccountAvatarWidget(Account account, QWidget *parent) :
		QWidget(parent), MyAccount(account), Service(0), WaitMovie(0)
{
}

AccountAvatarWidget::~AccountAvatarWidget()
{
}

void AccountAvatarWidget::setAvatarManager(AvatarManager *avatarManager)
{
	m_avatarManager = avatarManager;
}

void AccountAvatarWidget::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void AccountAvatarWidget::setProtocolsManager(ProtocolsManager *protocolsManager)
{
	m_protocolsManager = protocolsManager;
}

void AccountAvatarWidget::init()
{
	WaitMovie = new QMovie(m_iconsManager->iconPath(KaduIcon("kadu_icons/please-wait", "16x16")), QByteArray(), this);

	createGui();

	connect(m_protocolsManager, SIGNAL(protocolFactoryRegistered(ProtocolFactory*)),
	        this, SLOT(protocolRegistered(ProtocolFactory*)));
	connect(m_protocolsManager, SIGNAL(protocolFactoryUnregistered(ProtocolFactory*)),
	        this, SLOT(protocolUnregistered(ProtocolFactory*)));

	for (auto factory : m_protocolsManager->protocolFactories())
		protocolRegistered(factory);
}

void AccountAvatarWidget::serviceDestroyed()
{
	Service = 0;
	setEnabled(false);
}

void AccountAvatarWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	AvatarLabel = new QLabel();
	AvatarLabel->setAlignment(Qt::AlignCenter);
	AvatarLabel->setFixedWidth(128);

	layout->addWidget(AvatarLabel);

	ChangePhotoButton = new QPushButton(this);
	connect(ChangePhotoButton, SIGNAL(clicked(bool)), this, SLOT(changeButtonClicked()));
	setupMode();

	layout->addWidget(ChangePhotoButton, 0, Qt::AlignHCenter);

	auto avatar = m_avatarManager->byContact(MyAccount.accountContact(), ActionCreateAndAdd);
	connect(avatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));
	avatarUpdated();
}

void AccountAvatarWidget::setupMode()
{
	auto avatar = m_avatarManager->byContact(MyAccount.accountContact(), ActionCreateAndAdd);
	if (MyAccount.protocolHandler()->protocolFactory()->canRemoveAvatar() && !avatar.isEmpty())
		Mode = ModeRemove;
	else
		Mode = ModeChange;

	if (ModeRemove == Mode)
		ChangePhotoButton->setText(tr("Remove Photo..."));
	else
		ChangePhotoButton->setText(tr("Change Photo..."));
}

void AccountAvatarWidget::protocolRegistered(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)

	if (Service)
		disconnect(Service, 0, this, 0);

	Protocol *protocol = MyAccount.protocolHandler();
	if (!protocol)
		return;

	Service = protocol->avatarService();
	setEnabled(0 != Service);

	if (Service)
		connect(Service, SIGNAL(destroyed()), this, SLOT(serviceDestroyed()));
}

void AccountAvatarWidget::protocolUnregistered(ProtocolFactory *protocolFactory)
{
	protocolRegistered(protocolFactory);
}

void AccountAvatarWidget::avatarUpdated()
{
	WaitMovie->stop();
	AvatarLabel->setMovie(0);
	QPixmap avatar = MyAccount.accountContact().contactAvatar().pixmap();

	if (avatar.width() > 128 || avatar.height() > 128)
		avatar = avatar.scaled(QSize(128, 128), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	AvatarLabel->setPixmap(avatar);

	setupMode();
}

void AccountAvatarWidget::changeButtonClicked()
{
	if (ModeRemove == Mode)
		removeAvatar();
	else
		changeAvatar();
}

void AccountAvatarWidget::uploadAvatar(QImage avatar)
{
	AvatarLabel->setScaledContents(false);
	AvatarLabel->setMovie(WaitMovie);
	WaitMovie->start();

	AvatarUploader *uploader = Service->createAvatarUploader();
	if (!uploader)
	{
		avatarUploaded(false, QImage());
		return;
	}

	connect(uploader, SIGNAL(avatarUploaded(bool,QImage)), this, SLOT(avatarUploaded(bool,QImage)));
	uploader->uploadAvatar(MyAccount.id(), MyAccount.password(), avatar);

	ChangePhotoButton->setEnabled(false);
}

void AccountAvatarWidget::changeAvatar()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select avatar file"), QString(), tr("Images (*.jpeg *.jpg *.png);;All Files (*)"));
	if (fileName.isEmpty())
		return;

	QImage avatar(fileName);
	if (avatar.isNull())
		return;

	uploadAvatar(avatar);
}

void AccountAvatarWidget::removeAvatar()
{
	uploadAvatar(QImage());
}

void AccountAvatarWidget::avatarUploaded(bool ok, QImage image)
{
	if (ok)
		m_avatarManager->byContact(MyAccount.accountContact(), ActionCreateAndAdd).setPixmap(QPixmap::fromImage(image));

	avatarUpdated();
	ChangePhotoButton->setEnabled(true);
}

#include "moc_account-avatar-widget.cpp"
