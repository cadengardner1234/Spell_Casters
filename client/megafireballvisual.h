#ifndef MEGAFIREBALLVISUAL_H
#define MEGAFIREBALLVISUAL_H
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QRect>
class MegaFireballVisual : public QGraphicsPixmapItem {
public:
    explicit MegaFireballVisual();

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
