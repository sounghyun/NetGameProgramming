#ifndef BASETOWER_H
#define BASETOWER_H

#include "struct_package.h"

struct Basetower_Data
{
	int hp;
	float x, y, z;
	int angle;
	bool exist;
};

class Basetower
{
public:
	int hp;
	float x, y, z;
	int w, h, r;
	float angle;
	bool exist;

	Basetower() {};

	void setup(int hp, float x, float z, float angle, bool exist);

	Basetower& operator=(const Basetower_Data& ref) {
		this->hp = ref.hp;
		this->x = ref.x;
		this->y = ref.y;
		this->z = ref.z;
		this->angle = ref.angle;
		this->exist = ref.exist;

		return *this;
	}

	int GetHP() { return hp; };
	int GetX() { return x; };
	int GetY() { return y; };
	int GetZ() { return z; };
	int GetAngle() { return angle; };
	int GetExist() { return exist; };

	void ranberbasetower();
};

#endif // !BASETOWER_H