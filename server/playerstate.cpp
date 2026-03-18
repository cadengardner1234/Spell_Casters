#include "playerstate.h"
#include <QDebug>

PlayerState::PlayerState(QString name)
    : name(std::move(name)),
      health(100),
      regenTickCounter(0),
      regenBoosts(0)
{
}


QString PlayerState::getName() const {
    return name;
}

//QPointF PlayerState::getPosition() const {
//    return position;
//}

int PlayerState::getHealth() const {
    return health;
}

//void PlayerState::setPosition(QPointF newPos) {
//	//must add boundary checker please
//    position = newPos;
//}

void PlayerState::applyDamage(int amount) {
    health -= amount;
    if (health < 0)
        health = 0;
}

void PlayerState::heal(int amount) {
    qDebug() << this->name << " healing by " << amount << " hp";
    health += amount;
    if (health > 100)
        health = 100;
}

void PlayerState::castRegen()
{
    regenBoosts += REGEN_DURATION;
    if (regenBoosts > 25)
    {
        regenBoosts = 25;
    }
}

void PlayerState::regenTimerCb()
{
    //qDebug() << "regenTimerCb called: regen-" << regenBoosts << " ticks-" <<regenTickCounter;
    if (regenBoosts > 0)
    {
        if (regenTickCounter >= REGEN_TICK_DELAY)
        {
            regenTickCounter = 0;
            regenBoosts--;
            heal(REGEN_HEALTH_AMT);

        }
        else
        {
            regenTickCounter++;
        }
    }
}
