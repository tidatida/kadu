/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROXY_COMBO_BOX_H
#define PROXY_COMBO_BOX_H

#include "network/proxy/network-proxy.h"
#include "exports.h"

#include "gui/widgets/actions-combo-box.h"

class NetworkProxyModel;

class KADUAPI ProxyComboBox : public ActionsComboBox
{
	Q_OBJECT

	NetworkProxyModel *Model;

	bool InActivatedSlot;
	QAction *EditProxyAction;
	QAction *DefaultProxyAction;

private slots:
	void editProxy();

protected:
	virtual void valueChanged(QVariant value, QVariant previousValue);
	virtual bool compare(QVariant value, QVariant previousValue) const;

public:
	explicit ProxyComboBox(QWidget *parent = 0);
	virtual ~ProxyComboBox();

	virtual void reset();

	void enableDefaultProxyAction();
	void selectDefaultProxy();
	bool isDefaultProxySelected();

	void setCurrentProxy(const NetworkProxy &networkProxy);
	NetworkProxy currentProxy();

signals:
	void proxyChanged(const NetworkProxy &networkProxy, const NetworkProxy &lastNetworkProxy = NetworkProxy::null);

};

#endif // PROXY_COMBO_BOX_H
