/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "anglesproxymodel.hpp"

AnglesProxyModel::AnglesProxyModel(QObject* parent) :
    InstantsProxyModel(parent)
{
    setColumn(VideoModel::AnglesColumn);
}

QVariant AnglesProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
    if (!proxyIndex.isValid() || role != Qt::DisplayRole || !_parentIndex.isValid()) {
        return QVariant();
    }
    
    if (!proxyIndex.parent().isValid()) {
        return proxyIndex.column() == 0? tr("Angle %1").arg(proxyIndex.row()) : QVariant();
    }
    
    return mapToSource(proxyIndex).data();
}

QVariant AnglesProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QVariant();
    }
    
    switch (section) {
    case VideoModel::AFrameColumn:
        return tr("Frame");
    case VideoModel::AngleColumn:
        return tr("Angle (radians)");
    case VideoModel::ASpeedColumn:
        return tr("Angular speed");
    case VideoModel::AAccelerationColumn:
        return tr("Angular acceleration");
    }
    
    return QVariant();
}

int AnglesProxyModel::columnCount(const QModelIndex& parent) const
{
    return 4;
}
