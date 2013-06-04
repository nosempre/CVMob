#ifndef TRAJECTORYINSTANTITEM_HPP
#define TRAJECTORYINSTANTITEM_HPP

#include <QGraphicsRectItem>

class QGraphicsLineItem;

class TrajectoryInstantItem : public QGraphicsRectItem
{
public:
    TrajectoryInstantItem(QPointF pos, QPointF speed, QPointF accel, QGraphicsItem *parent,
                          QGraphicsLineItem *lineBefore = 0,
                          TrajectoryInstantItem *instantAfter = 0);
    TrajectoryInstantItem(QGraphicsItem *parent);
    ~TrajectoryInstantItem();

    bool isSpeedVisible() const;
    bool isAccelVisible() const;
    const QPointF pos() const;
    inline const QPointF speed() const { return _speed; }
    inline const QPointF accel() const { return _accel; }

    void setSpeedVisible(bool visible);
    void setAccelVisible(bool visible);
    void setPos(const QPointF &pos);
    void setSpeed(const QPointF &speed);
    void setAccel(const QPointF &accel);
    void setLineBefore(QGraphicsLineItem *lineBefore);
    void setInstantAfter(TrajectoryInstantItem *instantAfter);

private:
    QPointF _speed;
    QPointF _accel;
    QGraphicsLineItem *_speedLine;
    QGraphicsLineItem *_accelLine;

    QGraphicsLineItem *_lineBefore;
    TrajectoryInstantItem *_instantAfter;
};

#endif // TRAJECTORYINSTANTITEM_HPP
