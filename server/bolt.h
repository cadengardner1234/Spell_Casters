#ifndef BOLT_H
#define BOLT_H
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QRect>
class Bolt : public QGraphicsItem
{
public:
    explicit Bolt();

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
#endif // BOLT_H
