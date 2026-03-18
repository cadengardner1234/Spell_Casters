#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <QGraphicsItem>
#include <QPixmap>
#include <QPainter>
#include <QtDebug>
#include <QGraphicsEllipseItem>
#include "../defs.h"

class Crosshair : public QGraphicsItem
{
public:
    Crosshair(QGraphicsItem *parent = nullptr);
    ~Crosshair() override;
    void SetPlayer(int);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void advance(int phase) override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    // Provide externally accessible steering interface
    void goLeft();
    void goRight();
    void goUp();
    void goDown();
    void stopX();
    void stopY();

    qreal getDx();
    qreal getDy();

    const qreal BOUNDING_RADIUS = 350;
    QPointF getCorner();

private:
     QPixmap image;    // sprite
     int quadrant = 0;
     //QPointF getCorner();

     qreal x = 0.0;                 // Current player position (x,y)
     qreal y = 0.0;

     qreal crosshair_dx = 4.5;         // Fixed increment of travel (player_dx, player_dy);
     qreal crosshair_dy = crosshair_dx;

     qreal dx = 0.0;                // Desired direction of travel
     qreal dy = 0.0;

};

#endif // CROSSHAIR_H
