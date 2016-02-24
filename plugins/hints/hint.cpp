/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "hint.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "icons/icons-manager.h"
#include "misc/memory.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-configuration.h"
#include "notification/notification-service.h"
#include "notification/notification.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

Hint::Hint(QWidget *parent, const Notification &notification) :
		QFrame{parent},
		m_notification{notification}
{
}

Hint::~Hint()
{
}

void Hint::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void Hint::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void Hint::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void Hint::setNotificationConfiguration(NotificationConfiguration *notificationConfiguration)
{
	m_notificationConfiguration = notificationConfiguration;
}

void Hint::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void Hint::init()
{
	startSecs = secs = 10;

	createGui();
}

void Hint::createGui()
{
	auto pixmap = m_iconsManager->iconByPath(m_notification.icon).pixmap(m_configuration->deprecatedApi()->readNumEntry("Hints", "AllEvents_iconSize", 32));
	auto withPixmap = !pixmap.isNull();
	auto detailsText = details();
	auto withDetailsText = !detailsText.isEmpty();

	auto layout = make_owned<QGridLayout>(this);
	layout->setSpacing(0);

	if (withPixmap)
	{
		auto icon = make_owned<QLabel>(this);
		icon->setPixmap(pixmap);
		icon->setContentsMargins(0, 0, 6, 0);
		icon->setFixedSize(icon->sizeHint());
		layout->addWidget(icon, 0, 0, 2, 1, Qt::AlignTop);
	}

	auto label = make_owned<QLabel>(this);
	label->setTextInteractionFlags(Qt::NoTextInteraction);
	label->setText(QString{m_notification.text}.replace('\n', QStringLiteral("<br />")));

	layout->addWidget(label, 0, 1, withDetailsText ? 1 : 2, 1, Qt::AlignVCenter | Qt::AlignLeft);

	if (withDetailsText)
	{
		auto detailsLabel = make_owned<QLabel>(this);
		detailsLabel->setTextInteractionFlags(Qt::NoTextInteraction);
		detailsLabel->setText(detailsText.replace('\n', QStringLiteral("<br />")));

		layout->addWidget(detailsLabel, 1, 1, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
	}

	auto showButtons =
			!m_notification.callbacks.isEmpty() && m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowNotificationActions", 
				!m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowOnlyNecessaryButtons", false));
	if (showButtons)
	{
		auto callbacksBox = new QHBoxLayout{};
		callbacksBox->addStretch(10);
		layout->addLayout(callbacksBox, 2, 0, 1, 2);

		for (auto &&callbackName : m_notification.callbacks)
		{
			auto callback = m_notificationCallbackRepository->callback(callbackName);
			auto button = new QPushButton(callback.title(), this);
			button->setProperty("notify:callback", callbackName);
			connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

			callbacksBox->addWidget(button);
			callbacksBox->addStretch(1);
		}

		callbacksBox->addStretch(9);
	}
}

QString Hint::details() const
{
	if (!m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowContentMessage"))
		return {};

	auto const citeSign = 50;
	auto const syntax = QStringLiteral("<small>%1</small>");
	auto const message = QString{m_notification.details}.replace("<br/>", QStringLiteral(""));
	return (message.length() > citeSign
			? syntax.arg(message.left(citeSign) + "...")
			: syntax.arg(message)).replace('\n', QStringLiteral("<br />"));
}

void Hint::buttonClicked()
{
	auto const callbackName = sender()->property("notify:callback").toString();
	if (!callbackName.isEmpty())
	{
		auto callback = m_notificationCallbackRepository->callback(callbackName);
		callback.call(m_notification);
	}

	close();
}

void Hint::nextSecond(void)
{
	if (startSecs == 0)
		return;

	if (secs > 0)
		--secs;
}

bool Hint::isDeprecated()
{
	return startSecs != 0 && secs == 0;
}

void Hint::mouseReleaseEvent(QMouseEvent *event)
{
	switch (event->button())
	{
		case Qt::LeftButton:
			emit leftButtonClicked(this);
			break;

		case Qt::RightButton:
			emit rightButtonClicked(this);
			break;

		case Qt::MidButton:
			emit midButtonClicked(this);
			break;

		default:
			break;
	}
}

void Hint::enterEvent(QEvent *)
{
	setStyleSheet(QStringLiteral("* {background-color:%1;}").arg(palette().window().color().lighter().name()));
}

void Hint::leaveEvent(QEvent *)
{
	setStyleSheet(QStringLiteral(""));
}

void Hint::acceptNotification()
{
	m_notificationService->acceptNotification(m_notification);
}

void Hint::discardNotification()
{
	m_notificationService->discardNotification(m_notification);
}

#include "moc_hint.cpp"
