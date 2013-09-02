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

#include "videoview.hpp"

#include <model/videomodel.hpp>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QImage>
#include <QLineF>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <view/graphicsitems/distanceitem.hpp>
#include <view/graphicsitems/trajectoryinstantitem.hpp>
#include <view/graphicsitems/trajectoryitem.hpp>
#include <view/playbar.hpp>
#include <view/videographicsview.hpp>
#include <view/videostatus.hpp>

#include <QLabel>

#include <QTimer>

#include <QDebug>
#include <cstdlib>
#include <ctime>

VideoView::VideoView(QWidget *parent) :
    QAbstractItemView(parent),
    _view(new VideoGraphicsView),
    _noVideoVideo(Video(new QGraphicsScene(_view), 0)),
    _playBar(new PlayBar(this)),
    _status(new VideoStatus(this))
{
    new QVBoxLayout(viewport());
    viewport()->layout()->addWidget(_status);
    _status->hide();
    viewport()->layout()->addWidget(_view);
    viewport()->layout()->setMargin(0);
    viewport()->layout()->setSpacing(0);
    _view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setRenderHint(QPainter::Antialiasing);
    viewport()->layout()->addWidget(_playBar);

    QImage bgImage(":/images/translucent-logo.png");
    _view->setScene(_noVideoVideo.scene);
    _noVideoVideo.bgRect = new QGraphicsRectItem(0);
    _noVideoVideo.bgRect->setRect(QRectF(QPointF(0, 0), bgImage.size()));
    _noVideoVideo.bgRect->setBrush(bgImage);
    _noVideoVideo.bgRect->setPen(Qt::NoPen);
    _noVideoVideo.scene->addItem(_noVideoVideo.bgRect);
    _noVideoVideo.scene->setSceneRect(QRectF(QPointF(0, 0), bgImage.size()));
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);
    QGraphicsItem *noVideoText = _noVideoVideo.scene->addText(tr("No video"));
    noVideoText->moveBy(100, 50);

    connect(_playBar, SIGNAL(newDistanceRequested()), SLOT(beginDistanceCreation()));

    connect(_playBar, &PlayBar::frameChanged, [=](int frame)
    {
        QModelIndex currentFrameIndex = model()
                ->index(_currentVideoRow, VideoModel::CurrentFrameColumn);

        if (frame == currentFrameIndex.data(VideoModel::VideoSceneRole).toInt()) {
            return;
        }

        model()->setData(currentFrameIndex, frame, VideoModel::VideoSceneEditRole);
    });

    connect(_playBar, &PlayBar::playingChanged, [=](bool playing)
    {
        if (!playing) {
            QImage bgImage = _videos[_currentVideoRow].bgRect->brush().textureImage().copy();
            _videos[_currentVideoRow].bgRect->setBrush(bgImage);
        }
    });

    connect(_playBar, &PlayBar::newTrajectoryRequested, [=]()
    {
        auto status = new Status::Persistent(_status, tr("Click a point to track"));

        connect(_view, SIGNAL(mouseReleased(QPointF)), SLOT(calculateTrajectory(QPointF)));
        connect(_view, SIGNAL(mouseReleased(QPointF)), status, SLOT(deleteLater()));
    });
}

VideoView::~VideoView()
{
    delete _view;
}

void VideoView::showMessage(const QString &message, int duration)
{
    new Status::Message(_status, message, duration);
}

QRect VideoView::visualRect(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QRect();
    }

    return QRect(index.model()->index(index.row(), 0).data().toPoint(),
                 index.model()->index(index.row(), 1).data().toPoint());
}

void VideoView::dataChanged(const QModelIndex &topLeft, const QModelIndex &, const QVector<int> &)
{
    const QModelIndex parent = topLeft.parent();

    if (!parent.isValid()) { // Level 0
        Video v = _videos.at(topLeft.row());

        if (topLeft.column() == VideoModel::FrameSizeColumn) {
            QRectF r = QRectF(QPointF(0, 0),
                              topLeft.data(VideoModel::VideoSceneRole)
                              .toSizeF());
            v.bgRect->setRect(r);
            v.scene->setSceneRect(r);
        } else if (topLeft.column() == VideoModel::CurrentFrameColumn) {
            int frame = topLeft.data(VideoModel::VideoSceneRole).toInt();

            v.bgRect->setBrush(model()
                               ->index(frame, 0, model()
                                                 ->index(topLeft.row(),
                                                         VideoModel::FramesColumn))
                               .data(VideoModel::VideoSceneRole)
                               .value<QImage>());

            for (TrajectoryItem *traj : v.trajectories) {
                traj->setCurrentFrame(frame);
            }
        }
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.column() == VideoModel::DistancesColumn) {
            DistanceItem *line = _videos.at(parent.row()).distances.at(topLeft.row());
            line->setLine(topLeft.data(VideoModel::VideoSceneRole).toLineF());
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        if (parent.parent().column() == VideoModel::TrajectoriesColumn &&
            topLeft.column() == VideoModel::PositionColumn) {
            QPointF pos = topLeft.data(VideoModel::VideoSceneRole).toPointF();
            TrajectoryItem *trajectory = _videos.at(parent.parent().row()).trajectories.at(parent.row());
            trajectory->instantAt(topLeft.row())->setPos(pos);
        }
    }
}

void VideoView::selectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    if (selected.isEmpty()) {
        return;
    }

    QModelIndex selectedIndex = selected.at(0).indexes().at(0);
    _currentVideoRow = selectedIndex.row();

    _view->setScene(_videos.at(_currentVideoRow).scene);
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);

    _playBar->setPlayData(model()->rowCount(
                              model()->index(
                                  _currentVideoRow,
                                  VideoModel::FramesColumn)),
                          model()->data(
                              model()->index(_currentVideoRow,
                                             VideoModel::FrameDurationColumn)).toInt());
}

void VideoView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(index)
    Q_UNUSED(hint)
}

QModelIndex VideoView::indexAt(const QPoint &point) const
{
    foreach (QGraphicsItem *item, _view->scene()->items(point, Qt::ContainsItemShape, Qt::AscendingOrder)) {
        for (int i = 0; i < model()->rowCount(); ++i) {
            QModelIndex index = model()->index(i, VideoModel::DistancesColumn);
            if (index.data().toPointF() == item->boundingRect().topLeft()) {
                return index;
            }
        }
    }

    return QModelIndex();
}

QModelIndex VideoView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(cursorAction)
    Q_UNUSED(modifiers)

    return QModelIndex();
}

int VideoView::horizontalOffset() const
{
    return 0;
}

int VideoView::verticalOffset() const
{
    return 0;
}

bool VideoView::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index)

    return true;
}

void VideoView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_UNUSED(rect)
    Q_UNUSED(command)
}

QRegion VideoView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_UNUSED(selection)

    return QRegion();
}

void VideoView::resizeEvent(QResizeEvent *event)
{;
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);

    QAbstractItemView::resizeEvent(event);
}

void VideoView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    if (!parent.isValid()) { // Level 0
        for (int i = start; i <= end; ++i) {
            _videos.insert(i, Video(new QGraphicsScene(_view), 0));
            Video &v = _videos[i];
            v.bgRect = new QGraphicsRectItem();
            v.scene->addItem(v.bgRect);
            v.bgRect->setPen(Qt::NoPen);
        }
    } else if (!parent.parent().isValid()) { // Level 1
        Video &v = _videos[parent.row()];
        for (int i = start; i <= end; ++i) {
            switch (parent.column()) {
            TrajectoryItem *traj;
            case VideoModel::DistancesColumn:
                v.distances << new DistanceItem(v.bgRect);
                break;
            case VideoModel::TrajectoriesColumn:
                traj = new TrajectoryItem(v.bgRect);
                traj->setDrawTrajectory(TrajectoryItem::DrawBefore);
                v.trajectories << traj;
                break;
            default:
                break;
            }
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        Video &v = _videos[parent.parent().row()];
        TrajectoryItem *trajectory = v.trajectories[parent.row()];

        for (int i = start; i <= end; ++i) {
            switch (parent.parent().column()) {
            case VideoModel::TrajectoriesColumn:
                trajectory->appendInstant();
                break;
            }
        }
    }
}

void VideoView::beginDistanceCreation()
{
    auto status = new Status::Persistent(_status, tr("Click and drag to measure a distance"));

    connect(_view, SIGNAL(mousePressed(QPointF)), SLOT(distanceFirstPoint(QPointF)));
    connect(_view, SIGNAL(mousePressed(QPointF)), status, SLOT(deleteLater()));
}

static QGraphicsLineItem *guideLine = 0;

void VideoView::distanceFirstPoint(const QPointF &p)
{
    disconnect(_view, SIGNAL(mousePressed(QPointF)), this, SLOT(distanceFirstPoint(QPointF)));

    Video &currentVideo = _videos[_currentVideoRow];

    guideLine = new QGraphicsLineItem(QLineF(p, p), currentVideo.bgRect);
    guideLine->setPen(QColor(0, 0, 255));

    auto status = new Status::Persistent(_status, tr("Release to finish"));

    connect(_view, SIGNAL(mouseDragged(QPointF)), SLOT(distanceUpdateSecondPoint(QPointF)));
    connect(_view, SIGNAL(mouseReleased(QPointF)), SLOT(distanceEndCreation(QPointF)));
    connect(_view, SIGNAL(mouseReleased(QPointF)), status, SLOT(deleteLater()));
}

void VideoView::distanceUpdateSecondPoint(const QPointF &p)
{
    guideLine->setLine(QLineF(guideLine->line().p1(), p));
}

void VideoView::distanceEndCreation(const QPointF &p)
{
    static_cast<VideoModel *>(model())->createDistance(guideLine->line(), _currentVideoRow);
    delete guideLine;
    guideLine = 0;

    new Status::Message(_status, tr("Done"), 3000);

    disconnect(_view, SIGNAL(mouseDragged(QPointF)), this, SLOT(distanceUpdateSecondPoint(QPointF)));
    disconnect(_view, SIGNAL(mouseReleased(QPointF)), this, SLOT(distanceEndCreation(QPointF)));
}

void VideoView::calculateTrajectory(const QPointF &p)
{
    disconnect(_view, SIGNAL(mouseReleased(QPointF)), this, SLOT(calculateTrajectory(QPointF)));

    int frame = model()->index(_currentVideoRow, VideoModel::CurrentFrameColumn)
            .data(VideoModel::VideoSceneRole).toInt();

    TrajectoryCalcJob *job = static_cast<VideoModel *>(model())->calculateTrajectory
            (p, frame, _currentVideoRow);
    new Status::Job(_status, tr("Calculating trajectory..."), job);

    connect(job, SIGNAL(finished()), job, SLOT(deleteLater()));

    job->start();
}
