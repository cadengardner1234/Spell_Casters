#pragma once
#include <QString>
#include <QPointF>

class PlayerState{
public:
	PlayerState(QString name="", QPointF pos = QPointF(0,0););
	
	// Getters
	QString getName() const;
	QPointF getPosition() const;
	int getHealth() const;

	//Setters
	void setPosition(QPointF newPos);
	void applyDamage(int amount);
	void heal(int amount);
private: 	
	QString name;
	QPointF position;
	int health;
	
};
