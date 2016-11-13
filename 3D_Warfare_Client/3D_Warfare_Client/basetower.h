#ifndef BASETOWER_H
#define BASETOWER_H

#include "struct_package.h"
#include "cannonball.h"

class Tank;

class Basetower
{
public:
	int hp;
	float x, y, z;
	int w, h, r;
	float angle;
	bool exist;
	Ball cannonball;

	Basetower() {};

	void setup(int hp, float x, float z, float angle, bool exist);

	int GetHP() { return hp; };
	int GetX() { return x; };
	int GetY() { return y; };
	int GetZ() { return z; };
	int GetAngle() { return angle; };
	int GetExist() { return exist; };

	void ranberbasetower();

	void towerattck(Tank *tankobject);

	void destroytower();
};

#endif // !BASETOWER_H