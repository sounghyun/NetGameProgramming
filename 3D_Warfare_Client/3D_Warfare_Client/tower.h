#ifndef TOWER_H
#define TOWER_H

#include "struct_package.h"
#include "cannonball.h"

class Tank;

struct Tower_data
{
	int hp;
	float x, y, z;
	int angle;
	bool exist;
	Ball_data cannonball;
};

class Tower
{
public:
	int hp;
	float x, y, z;
	int w, h, r;
	float angle;
	bool exist;
	Ball cannonball;

	Tower() {};

	Tower(Tower_data* ref) : 
		hp(ref->hp),
		angle(ref->angle),
		x(ref->x),
		y(ref->y),
		z(ref->z),
		h(10),
		w(10),
		r(10),
		exist(ref->exist)
	{
		this->cannonball = ref->cannonball;
	};

	int GetHP() { return hp; };
	int GetX() { return x; };
	int GetY() { return y; };
	int GetZ() { return z; };
	int GetAngle() { return angle; };
	int GetExist() { return exist; };

	void ranbertower();

	void destroytower();
};

#endif // !TOWER_H