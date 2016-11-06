#ifndef TOWER_H
#define TOWER_H

#include "struct_package.h"
#include "cannonball.h"

class Tank;

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

	void setup(int hp, float x, float z, float angle, bool exist);

	int GetHP() { return hp; };
	int GetX() { return x; };
	int GetY() { return y; };
	int GetZ() { return z; };
	int GetAngle() { return angle; };
	int GetExist() { return exist; };

	void ranbertower();

	void towerattck(Tank *tankobject);

	void destroytower();
};

#endif // !TOWER_H