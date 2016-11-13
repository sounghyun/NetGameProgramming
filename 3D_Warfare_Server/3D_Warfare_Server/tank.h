#ifndef TANK_H
#define TANK_H

#include "struct_package.h"
#include "cannonball.h"

class Tower;
class Basetower;
class Guardian;

class Tank
{
public:
	int hp;
	float x, y, z;
	int w, h, r;
	int angle, Sangle;
	int time;
	int delaytime;
	int quake;
	bool exist;
	Ball cannonball;

	Tank() {};

	void createtank(float angle, int way);

	void ranbertank(bool body);

	virtual void tankmove(int way, Tank *tankobject, Tower *towerobject, Guardian *guardianobject, Basetower *baseobject);

	void destroytank();
};

#endif // !TANK_H