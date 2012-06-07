#include "videoview.h"

#include <model/cvmobvideomodel.hpp>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QImage>
#include <QtGui/QResizeEvent>
#include <QtGui/QVBoxLayout>

#include <QtGui/QLabel>

#include <QDebug>

VideoView::VideoView(QWidget *parent) :
    QAbstractItemView(parent),
    _view(new QGraphicsView),
    _noVideoScene(new QGraphicsScene(_view))
{
    new QHBoxLayout(viewport());
    viewport()->layout()->addWidget(_view);
    viewport()->layout()->setMargin(0);
    viewport()->layout()->setSpacing(0);
    _view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setRenderHint(QPainter::Antialiasing);

    QImage bgImage(":/images/translucent-logo.png");
    _noVideoScene->setBackgroundBrush(bgImage);
    _noVideoScene->setSceneRect(QRectF(QPointF(0, 0), bgImage.size()));
    QGraphicsItem *noVideoText = _noVideoScene->addText(tr("No video"));
    noVideoText->moveBy(100, 50);

    _view->setScene(_noVideoScene);
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);
}

VideoView::~VideoView()
{
    delete _view;
}

QRect VideoView::visualRect(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QRect();
    }

    return QRect(index.model()->index(index.row(), 0).data().toPoint(),
                 index.model()->index(index.row(), 1).data().toPoint());
}

void VideoView::dataChanged(const QModelIndex &topLeft, const QModelIndex &)
{
    const QModelIndex parent = topLeft.parent();

    if (!parent.isValid()) { // Level 0
        if (topLeft.column() == 4) { // Frame size
            _scenes.at(topLeft.row())->setSceneRect(QRectF(QPointF(0, 0),
                                                           topLeft.data(CvmobVideoModel::VideoSceneRole)
                                                           .toSizeF()));
        }
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.column() == 0) { // Distances
            QGraphicsLineItem *line = static_cast<QGraphicsLineItem *>(_scenes
                                                                       .at(parent.row())
                                                                       ->items()
                                                                       .at(topLeft.row()));
            line->setLine(topLeft.data(CvmobVideoModel::VideoSceneRole).toLineF());
        }
    }
}

void VideoView::selectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    _view->setScene(_scenes.at(selected.at(0).indexes().at(0).row()));
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);
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
            QModelIndex index = model()->index(i, 0);
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
            _scenes.insert(i, new QGraphicsScene(_view));
        }
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.column() == 0) { // Distances
            for (int i = start; i <= end; ++i) {
                _scenes[parent.row()]->addLine(0, 0, 0, 0);
            }
        }
    }
}
