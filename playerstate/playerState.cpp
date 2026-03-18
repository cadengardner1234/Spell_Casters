#include "playerState.h"

PlayerState::PlayerState(QString name, QPointF pos)
	: name(std::move(name)),
	  position(pos),
	  health(100)
{
}


QString PlayerState::getName() const {
    return name;
}

QPointF PlayerState::getPosition() const {
    return position;
}

int PlayerState::getHealth() const {
    return health;
}

void PlayerState::setPosition(QPointF newPos) {
	//must add boundary checker please
    position = newPos;
}

void PlayerState::applyDamage(int amount) {
    health -= amount;
    if (health < 0)
        health = 0;
}

void PlayerState::heal(int amount) {
    health += amount;
    if (health > 100)
        health = 100;
}
