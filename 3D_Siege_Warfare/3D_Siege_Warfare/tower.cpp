#ifndef TOWER
#define TOWER

#include "tower.h"
#include "tank.h"

void Tower::setup(int hp, float x, float z, float angle, bool exist) {
	this->hp = hp;
	this->x = x, this->z = z;
	this->h = 10;
	this->w = 10;
	this->r = 5;
	this->angle = angle, this->exist = exist;
};

void Tower::ranbertower()
{
	glPushMatrix();
	if (this->exist)
	{
		glTranslated(this->x, this->y, this->z);
		glPushMatrix();
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 5);
			glVertex3d(-10, 0, 5);
			glVertex3d(10, 0, 5);
			glVertex3d(10, 22.5, 5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 5);
			glVertex3d(-10, 0, 5);
			glVertex3d(-10, 0, -5);
			glVertex3d(-10, 22.5, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 5);
			glVertex3d(10, 22.5, 5);
			glVertex3d(10, 22.5, -5);
			glVertex3d(-10, 22.5, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		glPopMatrix();

		glRotated(this->angle, 0, 1, 0);
		// 상단 1/4
		glPushMatrix();
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-5, 22.5, 5);
			glVertex3d(5, 22.5, 5);
			glVertex3d(5, 22.5, -5);
			glVertex3d(-5, 22.5, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-5, 30, 5);
			glVertex3d(-5, 22.5, 5);
			glVertex3d(-5, 22.5, -5);
			glVertex3d(-5, 30, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-5, 30, 5);
			glVertex3d(5, 30, 5);
			glVertex3d(5, 22.5, -5);
			glVertex3d(-5, 22.5, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		glPopMatrix();

		// 포신
		glPushMatrix();
		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 28, 5);
		glVertex3d(2, 28, 5);
		glVertex3d(2, 28, 15);
		glVertex3d(-2, 28, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 28, 5);
		glVertex3d(-2, 24, 5);
		glVertex3d(-2, 24, 15);
		glVertex3d(-2, 28, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 24, 5);
		glVertex3d(2, 24, 5);
		glVertex3d(2, 24, 15);
		glVertex3d(-2, 24, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(2, 28, 5);
		glVertex3d(2, 24, 5);
		glVertex3d(2, 24, 15);
		glVertex3d(2, 28, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 28, 15);
		glVertex3d(2, 28, 15);
		glVertex3d(2, 24, 15);
		glVertex3d(-2, 24, 15);
		glEnd();
		glPopMatrix();
	}
	glPopMatrix();
}

void Tower::towerattck(Tank *tankobject)
{
	if (this->cannonball.exist == false && this->cannonball.delaytime == 0 && this->hp > 0)
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

		for (int i = 0; i < 9; i++)
		{
			if (this->cannonball.exist && tankobject[i].hp>0 && this->cannonball.collisionball(tankobject[i].x, tankobject[i].y, tankobject[i].z, 10, 10, 10))
			{
				this->cannonball.exist = false;
				tankobject[i].hp -= 2;
			}
			tankobject[i].destroytank();
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