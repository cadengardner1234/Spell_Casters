#include "megafireballvisual.h"

MegaFireballVisual::MegaFireballVisual()
{
    color = QColor("orange");
    x = 0.0;
    y = 0.0;
    w = 40.0;
    h = 40.0;
    }
    void MegaFireballVisual::paint(QPainter *painter,const QStyleOptionGraphicsItem *option, QWidget *widget)
    // paint() paints item in local coordinates
    {
        painter->setBrush(color);
        painter->drawEllipse(-w/2, -h/2, w, h);
    }
    QRectF MegaFireballVisual::boundingRect() const
    // Determines bounds for repainting
    {
        qreal adjust = 1.0;
        return QRectF(-w/2 - adjust, -h/2 - adjust, w + adjust, h + adjust);
    }
    void MegaFireballVisual::advance(int step)
    // advance() advances item by frame
    {

    }
