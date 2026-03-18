#ifndef BOLTVISUAL_H
#define BOLTVISUAL_H
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QRect>
class BoltVisual : public QGraphicsPixmapItem {
public:
    explicit BoltVisual();

    QRectF boundingRect() const;
    //QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    int id;  // matches server id
    QColor color;
    double x, y;
    double w, h;
    void advance(int step);

};
#endif
