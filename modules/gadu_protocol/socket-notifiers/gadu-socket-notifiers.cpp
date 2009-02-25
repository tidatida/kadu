/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>

#include "debug.h"

#include "gadu-socket-notifiers.h"

GaduSocketNotifiers::GaduSocketNotifiers(QObject *parent)
	: QObject(parent), Socket(0), Started(false), ReadNotifier(0), WriteNotifier(0), Lock(false)
{
	kdebugf();
	kdebugf2();
}

GaduSocketNotifiers::~GaduSocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void GaduSocketNotifiers::createSocketNotifiers()
{
	kdebugf();

	deleteSocketNotifiers();

	if (0 >= Socket)
		return;

	// read_socket_notifier
	ReadNotifier = new QSocketNotifier(Socket, QSocketNotifier::Read, this);
	connect(ReadNotifier, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	//write_socket_notifier
	WriteNotifier = new QSocketNotifier(Socket, QSocketNotifier::Write, this);
	connect(WriteNotifier, SIGNAL(activated(int)), this, SLOT(dataSent()));

	TimeoutTimer = new QTimer();
	connect(TimeoutTimer, SIGNAL(timeout()), this, SLOT(socketTimeout()));

	Started = true;

	kdebugf2();
}

void GaduSocketNotifiers::deleteSocketNotifiers()
{
	kdebugf();

	if (!Started)
		return;

	Started = false;

	ReadNotifier->setEnabled(false);
	ReadNotifier->deleteLater();
	ReadNotifier = 0;

	WriteNotifier->setEnabled(false);
	WriteNotifier->deleteLater();
	WriteNotifier = 0;

	TimeoutTimer->stop();
	TimeoutTimer->deleteLater();
	TimeoutTimer = 0;

	kdebugf2();
}

void GaduSocketNotifiers::disable()
{
	if (!Started)
		return;

	ReadNotifier->setEnabled(false);
	WriteNotifier->setEnabled(false);
	TimeoutTimer->stop();
}

void GaduSocketNotifiers::enable()
{
	if (!Started)
		return;

	if (!Lock && checkRead())
		ReadNotifier->setEnabled(true);
	if (!Lock && checkWrite())
		WriteNotifier->setEnabled(true);
	if (0 != timeout())
		TimeoutTimer->start(timeout()); // TODO: 0.6.6 connect to something
}

void GaduSocketNotifiers::watchFor(int socket)
{
	if (Socket == socket)
		return;

	Socket = socket;
	createSocketNotifiers();
}

void GaduSocketNotifiers::lock()
{
	Lock = true;
}

void GaduSocketNotifiers::unlock()
{
	Lock = false;
	enable();
}

void GaduSocketNotifiers::socketTimeout()
{
	// TODO: disconnected
}

void GaduSocketNotifiers::dataReceived()
{
	kdebugf();

	disable();
	socketEvent();
	enable();

	kdebugf2();
}

void GaduSocketNotifiers::dataSent()
{
	kdebugf();

	disable();
	socketEvent();
	enable();

	kdebugf2();
}
