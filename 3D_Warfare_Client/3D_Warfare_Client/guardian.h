#ifndef GUARDIAN_H
#define GUARDIAN_H

#include "struct_package.h"

struct Guardian_Data
{
	int hp;
	float x, y, z;
	int angle, Sangle;
	float Rangle, Langle;
	bool RLck;
	bool exist;


};

class Guardian
{
public:
	int hp;
	float x, y, z;
	int w, h, r;
	int angle, Sangle;
	float Rangle, Langle;
	int time;
	bool RLck;
	bool exist;

	Guardian& operator=(const Guardian_Data& ref) {
		this->hp = ref.hp;
		this->x = ref.x;
		this->y = ref.y;
		this->z = ref.z;
		this->angle = ref.angle;
		this->Sangle = ref.Sangle;
		this->Rangle = ref.Rangle;
		this->Langle = ref.Langle;
		this->RLck = ref.RLck;
		this->exist = ref.exist;

		return *this;
	}

	void ranberguardian();
};



#endif // !GUARDIAN_H