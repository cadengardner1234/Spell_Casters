#ifndef FIREBALL_H
#define FIREBALL_H
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QRect>
class Fireball : public QGraphicsItem
{
public:
    explicit Fireball();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString id;
    QColor color;
    double dx, dy;
    double x, y;
    qreal w, h;
protected:
    void advance(int step);
private:



};
#endif // FIREBALL_H
