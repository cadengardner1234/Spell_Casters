#ifndef SHIELDVISUAL_H
#define SHIELDVISUAL_H
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QRect>
#include <QtMath>
#include <QDebug>
#include "../defs.h"
class shieldvisual : public QGraphicsPixmapItem 
{
public:
    shieldvisual();
    QRectF boundingRect() const;
    //QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    int id;  // matches server id
    QColor color;
    double x, y;
    double w, h;
    double scale;
    void advance(int step);
    QPixmap shieldPixmap = QPixmap(":/sprites/Shield.png");
    qreal rotation = 0;
    void setRotationVal(double, double);
    void setRotationVal(qreal);
};

#endif // SHIELDVISUAL_H
