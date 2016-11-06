#ifndef GUARDIAN_H
#define GUARDIAN_H

#include "struct_package.h"

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

	void createguardian(float angle);
	void ranberguardian();
	void guardianmove();
	void destroyguardian();
};

#endif // !GUARDIAN_H