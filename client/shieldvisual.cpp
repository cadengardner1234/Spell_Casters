#include "shieldvisual.h"

shieldvisual::shieldvisual() 
{
    color = QColor("blue");
    x = 0.0;
    y = 0.0;
    w = 40.0;
    h = 40.0;
    scale = 2;
    shieldPixmap = shieldPixmap.scaled(scale*w, scale*h);
}

void shieldvisual::paint(QPainter *painter,const QStyleOptionGraphicsItem *option, QWidget *widget)
// paint() paints item in local coordinates
{
    painter->setBrush(Qt::transparent);
    if (HITBOXES_ON)
    {
        painter->setPen(color);
    }
    else
    {
        painter->setPen(Qt::transparent);
    }
    painter->drawRect(-w/2, -h/2, w, h);
    painter->drawPixmap(-w/2*scale, -h/2*scale, w*scale, h*scale,  shieldPixmap,  0, 0, w*scale, h*scale);
    //painter->drawPixmap()
    setRotation(rotation);

}
QRectF shieldvisual::boundingRect() const
// Determines bounds for repainting
{
    qreal adjust = 1.0;
    return QRectF(-w/2 - adjust, -h/2 - adjust, w + adjust, h + adjust);
}
void shieldvisual::advance(int step)
// advance() advances item by frame
{
    setRotation(rotation);
}

void shieldvisual::setRotationVal(double dx, double dy)
{
    rotation = 180*qAtan2(dy,dx)/3.14 + 90;
    qDebug() << "Setting rotation to: " << rotation;
}

void shieldvisual::setRotationVal(qreal rot)
{
    rotation = rot;
}


