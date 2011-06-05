/*

Copyright (C) 2011 Luciano Montanaro <mikelima@cirulla.net>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.

*/

#include "stationlistview.h"
#include "ui_stationlistview.h"
#include "stationlistmodel.h"
#include "stationlistproxymodel.h"
#include "keypressforwarder.h"
#include "settingsdialog.h"

#include <QActionGroup>
#include <QDebug>
#include <QKeyEvent>
#include <QSortFilterProxyModel>
#include <QStringListModel>

StationListView::StationListView(StationListModel *model, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StationListView),
    viewSelectionGroup(new QActionGroup(0)),
    stationListModel(model),
    filterModel(new StationListProxyModel(this)),
    keyPressForwarder(new KeyPressForwarder(this)),
    m_sortingMode(NoSorting)

{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
    setAttribute(Qt::WA_Maemo5AutoOrientation);
#endif
    ui->setupUi(this);
    viewSelectionGroup->addAction(ui->sortByNameAction);
    viewSelectionGroup->addAction(ui->sortNearFirstAction);
    viewSelectionGroup->addAction(ui->sortRecentFirstAction);
    filterModel->setSourceModel(stationListModel);
    filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->listView->setModel(filterModel);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->filterEdit->hide();

    keyPressForwarder->setTarget(ui->filterEdit);
    ui->listView->installEventFilter(keyPressForwarder);

    connect(ui->showAboutAction, SIGNAL(triggered()),
            this, SIGNAL(aboutTriggered()));
    connect(ui->listView,
            SIGNAL(activated(QModelIndex)), SLOT(showStation(QModelIndex)));
    connect(ui->filterEdit, SIGNAL(textChanged(const QString &)),
            SLOT(handleFilterChanges(const QString &)));
    connect(viewSelectionGroup, SIGNAL(triggered(QAction*)),
            SLOT(handleSortingChange(QAction*)));

    setSortingMode(AlphaSorting);
}


StationListView::~StationListView()
{
    delete ui;
}

void StationListView::showSettings(void)
{
    qDebug() << "Show Settings";
    SettingsDialog *settingsDialog = new SettingsDialog(this);
    if (settingsDialog->exec() == QDialog::Accepted) {
        // TODO Use new settings
    }

    delete settingsDialog;
}

void StationListView::showStation(const QModelIndex &index)
{
    qDebug() << "Show Station" << index.data();
    emit stationSelected(index.data().toString());
}

void StationListView::handleFilterChanges(const QString &filter)
{
    if (!filter.isEmpty())
        ui->filterEdit->show();
    else
        ui->filterEdit->hide();
    filterModel->setFilterFixedString(filter);
}

void StationListView::updatePosition(const QtMobility::QGeoPositionInfo &update)
{
    qDebug() << "Position update received" << update;
    filterModel->setUserPosition(update.coordinate());
    filterModel->invalidate();
    filterModel->sort(0);
}

void StationListView::handleSortingChange(QAction *action)
{
    SortingMode mode = NoSorting;
    if (action == ui->sortByNameAction) {
        mode = AlphaSorting;
        qDebug() << "sort by name";
    } else if (action == ui->sortNearFirstAction) {
        mode = DistanceSorting;
        qDebug() << "sort by distance";
    } else if (action == ui->sortRecentFirstAction) {
        mode = RecentUsageSorting;
        qDebug() << "sort by recent use";
    }
    setSortingMode(mode);
}

void StationListView::setSortingMode(StationListView::SortingMode mode)
{
    if (mode != m_sortingMode) {
        m_sortingMode = mode;
        switch (mode) {
        case AlphaSorting:
            filterModel->setSortRole(Qt::DisplayRole);
            ui->sortByNameAction->setChecked(true);
            break;
        case DistanceSorting:
            filterModel->setSortRole(StationListModel::PositionRole);
            ui->sortNearFirstAction->setChecked(true);
            break;
        case RecentUsageSorting:
            ui->sortRecentFirstAction->setChecked(true);
            break;
        case NoSorting:
        default:
            break;
        }
        filterModel->invalidate();
        filterModel->sort(0);
        emit sortingModeChanged(mode);
    }
}

StationListView::SortingMode StationListView::sortingMode()
{
    return m_sortingMode;
}
