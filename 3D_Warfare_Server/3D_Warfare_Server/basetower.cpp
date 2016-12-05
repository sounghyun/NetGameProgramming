#ifndef BASETOWER
#define BASETOWER

#include "basetower.h"
#include "tank.h"

void Basetower::setup(int hp, float x, float z, float angle, bool exist) {
	this->hp = hp;
	this->x = x, this->z = z;
	this->h = 10;
	this->w = 10;
	this->r = 10;
	this->angle = angle, this->exist = exist;
};

void Basetower::destroytower()
{
	if (this->hp <= 0 && this->exist)
	{
		this->y -= 0.1;
		if (this->y + 15 < 0)
			this->exist = false;
	}
}

#endif // !BASETOWER