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

	Tank(float angle, int way) : hp(5), 
								 Sangle(angle),
								 angle(90 * (way - 1) + angle),
								 x(100),
								 y(0),
								 h(10),
								 w(10),
								 r(10),
								 exist(true),
								 delaytime(1500)
	{
		if (this->Sangle)
			this->z = -450;
		else
			this->z = -50;
		if (way == 1)
			this->time = 2000;
		else
			this->time = 2730; };

	void ranbertank(bool body);

	void tankmove(int way, Tank *tankobject, Tower *towerobject, Guardian *guardianobject, Basetower *baseobject);

	void destroytank();
};

#endif // !TANK_H