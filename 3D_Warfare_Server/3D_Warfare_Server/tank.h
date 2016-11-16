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
	int wave;
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
								 wave(way+1),
								 delaytime(1500)
	{
		if (this->Sangle)
			this->z = -450;
		else
			this->z = -50;
		if (wave == 2)
			this->time = 2000;
		else
			this->time = 2730; };

	void tankmove(list<Tank>& tankobject, Tower *towerobject, Guardian *guardianobject, Basetower *baseobject);

	void destroytank();
};


struct Tank_data
{
	int hp;
	float x, y, z;
	int angle, Sangle;
	int wave;

	Tank_data& operator=(const Tank& ref){
		this->hp = ref.hp;
		this->x = ref.x;
		this->y = ref.y;
		this->z = ref.z;
		this->angle = ref.angle;
		this->Sangle = ref.Sangle;
		this->wave = ref.wave;

		return *this;
	}
};

#endif // !TANK_H