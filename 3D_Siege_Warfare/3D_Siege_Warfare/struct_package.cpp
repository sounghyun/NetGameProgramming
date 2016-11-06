#ifndef STRUCK_PACKAGE

#define STRUCK_PACKAGE

#include "struct_package.h"
#include "cannonball.h"

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

void Tank::tank(bool body)
{
	glPushMatrix(); // 탱크
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

template<class Object>
void Tank::tankmove(int way, Object *object)
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
		Cannonball_timer(&this->cannonball, 2);

		for (int i = 0; i < 9; i++)
		{
			if (this->cannonball.exist && object.hp>0 && collisionball(this->cannonball, object.x, object.y, object.z, object.h, object.w, object.r))
			{
				this->cannonball.exist = false;
				object.hp--;
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

void Tower::setup(int hp, float x, float z, float angle, bool exist) {
	this->hp = hp;
	this->x = x, this->z = z;
	this->h = 10;
	this->w = 10;
	this->r = 5;
	this->angle = angle, this->exist = exist;
};

void Tower::tower()
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
		Cannonball_timer(&this->cannonball, 4);

		for (int i = 0; i < 9; i++)
		{
			if (this->cannonball.exist && tankobject[i].hp>0 && collisionball(this->cannonball, tankobject[i].x, tankobject[i].y, tankobject[i].z, 10, 10, 10))
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

void Basetower::setup(int hp, float x, float z, float angle, bool exist) {
	this->hp = hp;
	this->x = x, this->z = z;
	this->h = 10;
	this->w = 10;
	this->r = 10;
	this->angle = angle, this->exist = exist;
};

void Basetower::basetower()
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

void Basetower::towerattck(Tank *tankobject)
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
		Cannonball_timer(&this->cannonball, 4);

		for (int i = 0; i < 9; i++)
		{
			if (this->cannonball.exist && tankobject[i].hp>0 && collisionball(this->cannonball, tankobject[i].x, tankobject[i].y, tankobject[i].z, 10, 10, 10))
			{
				this->cannonball.exist = false;
				tankobject[i].hp -= 2;
			}
			tankobject[i].destroytank();
		}
	}
}

void Basetower::destroytower()
{
	if (this->hp <= 0 && this->exist)
	{
		this->y -= 0.1;
		if (this->y + 15 < 0)
			this->exist = false;
	}
}

#endif