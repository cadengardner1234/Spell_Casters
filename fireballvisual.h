#ifndef FIREBALLVISUAL_H
#define FIREBALLVISUAL_H
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QRect>
class FireballVisual : public QGraphicsPixmapItem {
public:
    explicit FireballVisual();

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
