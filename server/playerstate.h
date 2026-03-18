#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#pragma once
#include <QString>
#include <QPointF>

class PlayerState{
public:
    PlayerState(QString name="");

    // Getters
    QString getName() const;
    //QPointF getPosition() const;
    int getHealth() const;

    //Setters
    //void setPosition(QPointF newPos);
    void applyDamage(int amount);
    void heal(int amount);
    QString name;
    QString governmentName = "";
    int health;
    static const int HEAL_AMT = 15;
    static const int BIG_HEAL_AMT = 40;
    static const int REGEN_TICK_DELAY = 80; // Should approx be this number*25ms between regen ticks.  10s->10,000ms->400ticksTotal
    static const int REGEN_HEALTH_AMT = 8; // Adds this number to health
    static const int REGEN_DURATION = 5; // Adds it this number of times 
    int regenTickCounter; // How many ticks have occurred since last regen bump.
    int regenBoosts; // How many regen's have left
    void castRegen();
    void regenTimerCb();
    
private:
    //QPointF position;



};

#endif // PLAYERSTATE_H
