#ifndef GUARDIAN
#define GUARDIAN

#include "guardian.h"

void Guardian::createguardian(float angle)
{
	this->hp = 20;
	this->Sangle = angle;
	this->angle = 90 + angle;
	if (this->Sangle)
		this->z = -460;
	else
		this->z = -40;
	this->x = 100;
	this->y = 0;
	this->exist = true;
	this->time = 250;
}

void Guardian::guardianmove()
{
	if (this->hp > 0)
	{
		if (this->time % 500 == 0)
		{
			this->angle += 5;
			if ((this->angle - 90) % 180 == 0)
				this->time++;
		}
		else
		{
			this->x -= 0.2*sin(this->angle * (3.14 / 180));
			this->z -= 0.2*cos(this->angle * (3.14 / 180));
			this->time++;
		}

		if (this->RLck)
		{
			this->Rangle += 1;
			if (this->Langle > -45)
				this->Langle -= 1;
			if (this->Rangle > 45)
				this->RLck = false;
		}
		else
		{
			this->Langle += 1;
			if (this->Rangle > -45)
				this->Rangle -= 1;
			if (this->Langle > 45)
				this->RLck = true;
		}
	}
}

void Guardian::destroyguardian()
{
	if (this->hp <= 0 && this->exist)
	{
		this->y -= 0.1;
		if (this->y + 15 < 0)
			this->exist = false;
	}
}

#endif // !GUARDIAN