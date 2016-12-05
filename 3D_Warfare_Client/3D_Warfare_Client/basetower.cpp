#ifndef BASETOWER
#define BASETOWER

#include "basetower.h"
#include "tank.h"

void Basetower::setup(int hp, float x, float z, float angle, bool exist) {
	this->hp = hp;
	this->x = x, this->z = z;
	this->h = 10;
	this->w = 10;
	this->r = 10;
	this->angle = angle, this->exist = exist;
};

void Basetower::ranberbasetower()
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
		// ��� 1/4
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

		// ����
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

#endif // !BASETOWER