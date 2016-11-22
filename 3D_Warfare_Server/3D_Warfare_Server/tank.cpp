#ifndef TANK
#define TANK

#include "tank.h"
#include "tower.h"
#include "basetower.h"
#include "guardian.h"

void Tank::tankmove(list<Tank>& tankobject, Tower *towerobject, Guardian *guardianobject, Basetower *baseobject)
{
	if (this->hp > 0)
	{
		this->angle %= 360;
		this->time-=2;
		if (this->time < -200)
			this->exist = false;

		if (this->wave == 1)
		{
			if (this->time < 2450 && this->time && this->angle < 0 + this->Sangle)
				this->angle++;
			if (this->time < 450 && this->time && this->angle < 90 + this->Sangle)
				this->angle++;
			if (this->time == 0 && this->angle > 0 + this->Sangle)
				this->angle--;
		}
		if (this->wave == 3)
		{
			if (this->time < 2450 && this->time && this->angle > 0 + this->Sangle)
				this->angle--;
			if (this->time < 450 && this->time && this->angle > -90 + this->Sangle)
				this->angle--;
			if (this->time == 0 && this->angle < 0 + this->Sangle)
				this->angle++;
		}

		if (this->time > 0)
		{
			this->x -= 0.4*sin(this->angle * (3.14 / 180));
			this->z -= 0.4*cos(this->angle * (3.14 / 180));
		}
	}
	if (this->cannonball.exist == false && this->cannonball.delaytime <= 0 && this->hp > 0)
	{
		if (this->angle)
			this->cannonball.z = this->z - 2;
		else
			this->cannonball.z = this->z + 2;
		this->cannonball.x = this->x;
		this->cannonball.track = 0;
		this->cannonball.y = this->y + 3;
		this->cannonball.angle = this->angle;
		this->cannonball.exist = true;
		this->cannonball.delaytime = 60;
	}
	else
	{
		this->cannonball.Cannonball_timer(2);

		if (this->cannonball.exist && baseobject->hp>0 && this->cannonball.collisionball(baseobject->x, baseobject->y, baseobject->z, 10, 10, 10))
		{
			this->cannonball.exist = false;
			if (!guardianobject->exist)
				baseobject->hp--;
		}

		for (int i = 0; i < 6; i++)
		{
			if (this->cannonball.exist && towerobject[i].hp>0 && this->cannonball.collisionball(towerobject[i].x, towerobject[i].y, towerobject[i].z, 10, 10, 5))
			{
				this->cannonball.exist = false;
				towerobject[i].hp--;
			}
		}

		if (this->cannonball.exist && guardianobject->hp > 0 && this->cannonball.collisionball(guardianobject->x, guardianobject->y, guardianobject->z, 10, 15, 5))
		{
			this->cannonball.exist = false;
			guardianobject->hp--;
		}

		for (auto& d : tankobject)
		{
			if (this->cannonball.exist && d.hp>0 && this->cannonball.collisionball(d.x, d.y, d.z, 10, 10, 10))
			{
				this->cannonball.exist = false;
				d.hp--;
			}
		}
	}
}

void Tank::destroytank()
{
	if (this->hp <= 0 && this->exist)
	{
		this->y -= 0.1;
		if (this->y + 5 < 0)
		{
			this->delaytime--;
			if (this->delaytime == 0)
				this->exist = false;
		}
	}
}

#endif // !TANK