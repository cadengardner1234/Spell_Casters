#ifndef SHIELD_H
#define SHIELD_H
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QRect>
#include <QtMath>
class shield : public QGraphicsItem
{
public:
    shield();
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString id;
    QColor color;
    //double dx, dy;
    double x, y;
    qreal w, h;
    int setRotation(double dx, double dy);

protected:
    void advance(int step);
private:

    
};

#endif // SHIELD_H
