#ifndef TOWER
#define TOWER

#include "tower.h"
#include "tank.h"

void Tower::towerattck(list<Tank>& tankobject)
{
	if (this->cannonball.exist == false && this->cannonball.delaytime <= 0 && this->hp > 0)
	{
		if (this->angle)
			this->cannonball.z = this->z - 10;
		else
			this->cannonball.z = this->z + 10;
		this->cannonball.x = this->x;
		this->cannonball.track = 0;
		this->cannonball.y = this->y + 25;
		this->cannonball.angle = this->angle + 180;
		this->cannonball.exist = true;
		this->cannonball.delaytime = 80;
	}
	else
	{
		this->cannonball.Cannonball_timer(4);

		for (auto d : tankobject)
		{
			if (this->cannonball.exist && d.hp>0 && this->cannonball.collisionball(d.x, d.y, d.z, 10, 10, 10))
			{
				this->cannonball.exist = false;
				d.hp -= 2;
			}
			d.destroytank();
		}
	}
}

void Tower::destroytower()
{
	if (this->hp <= 0 && this->exist)
	{
		this->y -= 0.1;
		if (this->y + 15 < 0)
			this->exist = false;
	}
}
#endif // !TOWER