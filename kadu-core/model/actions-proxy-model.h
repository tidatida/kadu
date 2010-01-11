/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACTIONS_PROXY_MODEL_H
#define ACTIONS_PROXY_MODEL_H

#include <QtGui/QAbstractProxyModel>

class ActionsProxyModel : public QAbstractProxyModel
{
	Q_OBJECT

public:
	typedef QPair<QString, QString> ModelAction;
	typedef QList<ModelAction> ModelActionList;

private:
	ModelActionList BeforeActions;
	ModelActionList AfterActions;

private slots:
	void sourceDataChanged(const QModelIndex &, const QModelIndex &);
	void sourceHeaderDataChanged(Qt::Orientation, int, int);
	void sourceRowsAboutToBeInserted(const QModelIndex &, int, int);
	void sourceRowsInserted(const QModelIndex &, int, int);
	void sourceColumnsAboutToBeInserted(const QModelIndex &, int, int);
	void sourceColumnsInserted(const QModelIndex &, int, int);
	void sourceRowsAboutToBeRemoved(const QModelIndex &, int, int);
	void sourceRowsRemoved(const QModelIndex &, int, int);
	void sourceColumnsAboutToBeRemoved(const QModelIndex &, int, int);
	void sourceColumnsRemoved(const QModelIndex &, int, int);
	void sourceLayoutAboutToBeChanged();
	void sourceLayoutChanged();

public:
	ActionsProxyModel(ModelActionList beforeActions, ModelActionList afterActions, QObject *parent);
	virtual ~ActionsProxyModel();

	virtual void setSourceModel(QAbstractItemModel *newSourceModel);

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;

	virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

};

#endif // ACTIONS_PROXY_MODEL_H
