#ifndef GUARDIAN_H
#define GUARDIAN_H

#include "struct_package.h"

struct Guardian_Data
{
	int hp;
	float x, y, z;
	int angle;
	float Rangle, Langle;
	bool RLck;


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
		this->Rangle = ref.Rangle;
		this->Langle = ref.Langle;
		this->RLck = ref.RLck;

		return *this;
	}

	void createguardian(float angle);
	void ranberguardian();
	void guardianmove();
	void destroyguardian();
};



#endif // !GUARDIAN_H