#ifndef MEGAFIREBALL_H
#define MEGAFIREBALL_H
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QRect>
class MegaFireball : public QGraphicsItem
{
public:
    explicit MegaFireball();

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
#endif // MEGAFIREBALL_H
