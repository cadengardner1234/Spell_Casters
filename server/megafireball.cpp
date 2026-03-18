#include "megafireball.h"

MegaFireball::MegaFireball()
{
    color = QColor("orange");
    dx = 0.00;
    dy = 0.00;
    x = 0.0;
    y = 0.0;
    w = 40.0;
    h = 40.0;
    }
    void MegaFireball::paint(QPainter *painter,const QStyleOptionGraphicsItem *option, QWidget *widget)
    // paint() paints item in local coordinates
    {
        painter->setBrush(color);
        painter->drawEllipse(-w/2, -h/2, w, h);
    }
    QRectF MegaFireball::boundingRect() const
    // Determines bounds for repainting
    {
        qreal adjust = 1.0;
        return QRectF(-w/2 - adjust, -h/2 - adjust, w + adjust, h + adjust);
    }
    void MegaFireball::advance(int step)
    // advance() advances item by frame
    {
       if (step == 0)
        return;
        x = this->pos().x();
        y = this->pos().y();
        x = x + dx;
        y = y + dy;
        setPos(x, y); // Set item position in parent coordinates
    }
    QPainterPath MegaFireball::shape() const
    // Returns shape for collision detection
    {
        QPainterPath path;
        path.addRect(-w/2, -h/2, w, h);
        return path;
    }
