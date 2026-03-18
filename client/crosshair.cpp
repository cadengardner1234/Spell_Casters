#include "crosshair.h"

Crosshair::Crosshair(QGraphicsItem *parent)
{
    QString s = ":/sprites/crosshair.png";
    image = QPixmap(s);
    image = image.scaled(CROSSHAIR_WIDTH, CROSSHAIR_HEIGHT);
}

Crosshair::~Crosshair()
{

}

void Crosshair::goLeft()
{
    dx = -crosshair_dx;
}

void Crosshair::goRight()
{
    dx = crosshair_dx;
}

void Crosshair::goUp()
{
    dy = -crosshair_dy;
}

void Crosshair::goDown()
{
    dy = crosshair_dy;
}

void Crosshair::stopX()
{
    dx = 0.0;
}

void Crosshair::stopY()
{
    dy = 0.0;
}

qreal Crosshair::getDx()
{
    return dx;
}

qreal Crosshair::getDy()
{
    return dy;
}

void Crosshair::SetPlayer(int playerNum)
{
    if (playerNum <= 2)
    {
        quadrant = (playerNum % 2) + 1; // Swaps 1 and 2
    }
    else
    {
        quadrant = playerNum;
    }
}

QPointF Crosshair::getCorner()
{
    QPointF cornerPos;
    qreal x = SCENE_WIDTH/2;
    qreal y = SCENE_HEIGHT/2;

    /* Assuming Quadrants are:
     *          -y
     *           |
     *       2   |   1
     *           |
     * -x -------|------- +x
     *           |
     *       3   |   4
     *           |
     *          +y
    */ 

    if (quadrant==2 || quadrant == 3) //Left
    {
        x = -x;
    }
    if (quadrant<=2) //Bottom
    {
        y = -y;
    }

    cornerPos = QPointF(x,y);
    return cornerPos;
}


void Crosshair::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    painter->drawPixmap(-CROSSHAIR_WIDTH/2, -CROSSHAIR_HEIGHT/2, image);
    if (HITBOXES_ON)
    {
        painter->setPen(QPen(Qt::yellow));
        painter->drawRect(this->boundingRect());
        painter->setBrush(Qt::red);
        int dotRadius = 2;
        painter->drawEllipse(-dotRadius/2,-dotRadius,dotRadius,dotRadius);
        painter->setBrush(Qt::transparent);
    }
}

QRectF Crosshair::boundingRect() const
{
    return QRectF(-CROSSHAIR_WIDTH/2, -CROSSHAIR_HEIGHT/2, CROSSHAIR_WIDTH, CROSSHAIR_HEIGHT);
}

QPainterPath Crosshair::shape() const
{
    QPainterPath path;
    path.addRect(-CROSSHAIR_WIDTH/2, -CROSSHAIR_HEIGHT/2, CROSSHAIR_WIDTH, CROSSHAIR_HEIGHT);
    return path;
}

void Crosshair::advance(int phase)
{
    if (phase == 0)
    {
        return;
    }

    // Compute new coordinates
    QPointF pos = this->pos();
    pos.setX(pos.rx() + dx);
    pos.setY(pos.ry() + dy);

    qreal relX = pos.rx();
    qreal relY = pos.ry();

    // Force new coordinates to remain within scene bounds
    if (relX < -SCENE_WIDTH/2 + CROSSHAIR_WIDTH/2)
    {
        pos.setX(-SCENE_WIDTH/2 + CROSSHAIR_WIDTH/2);
    }
    else if (relX > SCENE_WIDTH/2 - CROSSHAIR_WIDTH/2)
    {
        pos.setX(SCENE_WIDTH/2 - CROSSHAIR_WIDTH/2);
    }

    if (relY < -SCENE_HEIGHT/2 + CROSSHAIR_HEIGHT/2)
    {
        pos.setY(-SCENE_HEIGHT/2 + CROSSHAIR_HEIGHT/2);
    }
    else if (relY > SCENE_HEIGHT/2 - CROSSHAIR_HEIGHT/2)
    {
        pos.setY(SCENE_HEIGHT/2 - CROSSHAIR_HEIGHT/2);
    }
    QPointF corner = getCorner();
    QPointF vector = pos - corner;
    qreal Dist = QLineF(corner, pos).length();
    if (Dist > BOUNDING_RADIUS)
    {
        qreal scale = BOUNDING_RADIUS/Dist;
        pos = corner + vector*scale;
    }

    // Move CROSSHAIR to new coordinates
    //qDebug() << "X: " << x << "    Y: " << y;
    x = pos.rx();
    y = pos.ry();
    this->setPos(pos);
    this->setZValue(5);

//    // Detect any collisions
//    QList<QGraphicsItem*> list = collidingItems();
//    if (!list.isEmpty())
//    {
//        qDebug() << "Crosshair detects collision";
//    }
}
