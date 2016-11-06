#ifndef TANK
#define TANK

#include "tank.h"
#include "tower.h"
#include "basetower.h"
#include "guardian.h"

void Tank::createtank(float angle, int way)
{
	this->hp = 5;
	this->Sangle = angle;
	this->angle = 90 * (way - 1) + angle;
	if (this->Sangle)
		this->z = -450;
	else
		this->z = -50;
	this->x = 100;
	this->y = 0;
	this->h = 10;
	this->w = 10;
	this->r = 10;
	this->exist = true;
	this->delaytime = 1500;
	if (way == 1)
		this->time = 2000;
	else
		this->time = 2730;
}

void Tank::ranbertank(bool body)
{
	glPushMatrix(); // ÅÊÅ©
	if (!body)
		glTranslated(0, this->quake, 0);

	glTranslated(this->x, this->y, this->z + 1);
	glRotated(this->angle, 0, 1, 0);

	if (body)
	{
		glScaled(0.5, 0.5, 0.5);

		glColor3d(0, 0, 1);
		glTranslated(0, 2, 0);
		glScaled(2, 1, 2);
		glutSolidCube(5);


		glTranslated(0, 2, 0);
		glScaled(1, 2.5, 1);
		glutSolidCube(3);

		glScaled(0.5, 1, 0.5);

		glColor3d(1, 0, 1);
		glTranslated(0, 1, -5);
		glScaled(1, 0.5, 5);
		glutSolidCube(1);
	}
	else
	{
		glColor3d(1, 0, 1);
		glTranslated(0, -1, -2);
		glRotated(30, 1, 0, 0);
		glScaled(0.5, 0.25, 3);
		glutSolidCube(0.75);
	}

	glPopMatrix();

}

void Tank::tankmove(int way, Tank *tankobject, Tower *towerobject, Guardian *guardianobject, Basetower *baseobject)
{
	if (this->hp > 0)
	{
		this->time--;
		if (this->time < -300)
			this->exist = false;

		if (way == 1)
		{
			if (this->time < 2400 && this->time && this->angle < 0 + this->Sangle)
				this->angle++;
			if (this->time < 430 && this->time && this->angle < 90 + this->Sangle)
				this->angle++;
			if (this->time == 0 && this->angle > 0 + this->Sangle)
				this->angle--;
		}
		if (way == 3)
		{
			if (this->time < 2400 && this->time && this->angle > 0 + this->Sangle)
				this->angle--;
			if (this->time < 430 && this->time && this->angle > -90 + this->Sangle)
				this->angle--;
			if (this->time == 0 && this->angle < 0 + this->Sangle)
				this->angle++;
		}

		if (this->time > 0)
		{
			this->x -= 0.2*sin(this->angle * (3.14 / 180));
			this->z -= 0.2*cos(this->angle * (3.14 / 180));
		}
	}
	if (this->cannonball.exist == false && this->cannonball.delaytime == 0 && this->hp > 0)
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

		for (int i = 0; i < 9; i++)
		{
			if (this->cannonball.exist && tankobject[i].hp>0 && this->cannonball.collisionball(tankobject[i].x, tankobject[i].y, tankobject[i].z, 10, 10, 10))
			{
				this->cannonball.exist = false;
				tankobject[i].hp--;
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