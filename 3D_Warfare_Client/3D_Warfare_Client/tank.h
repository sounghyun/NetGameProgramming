#ifndef TANK_H
#define TANK_H

#include "struct_package.h"
#include "cannonball.h"

class Tower;
class Basetower;
class Guardian;

struct Tank_data
{
	int hp;
	float x, y, z;
	int angle, Sangle;
	int wave;
};

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

	Tank(Tank_data* ref) : hp(5),
		Sangle(ref->Sangle),
		angle(ref->angle),
		x(ref->x),
		y(ref->y),
		z(ref->z),
		h(10),
		w(10),
		r(10),
		exist(true),
		wave(ref->wave),
		delaytime(1500)
	{
		if (wave == 2)
			this->time = 2000;
		else
			this->time = 2730;
	};
	void ranbertank(bool body);

	void destroytank();
};

#endif // !TANK_H