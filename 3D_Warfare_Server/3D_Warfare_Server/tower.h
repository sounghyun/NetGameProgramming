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

	Tower(int hp, float x, float z, float angle, bool exist) {
		this->hp = hp;
		this->x = x, this->y = 0, this->z = z;
		this->h = 10;
		this->w = 10;
		this->r = 5;
		this->angle = angle, this->exist = exist;
	};

	int GetHP() { return hp; };
	int GetX() { return x; };
	int GetY() { return y; };
	int GetZ() { return z; };
	int GetAngle() { return angle; };
	int GetExist() { return exist; };

	void towerattck(list<Tank>& tankobject);

	void destroytower();
};

struct Tower_data
{
	int hp;
	float x, y, z;
	int angle;
	bool exist;
	Ball_data cannonball;

	Tower_data& operator=(const Tower& ref) {
		this->hp = ref.hp;
		this->x = ref.x;
		this->y = ref.y;
		this->z = ref.z;
		this->angle = ref.angle;
		this->exist = ref.exist;
		this->cannonball = ref.cannonball;

		return *this;
	}
};

#endif // !TOWER_H