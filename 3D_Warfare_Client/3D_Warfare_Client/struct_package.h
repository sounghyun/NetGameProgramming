#ifndef STRUCK_PACKAGE_H
#define STRUCK_PACKAGE_H


#include <winsock2.h>
#include <GL/glut.h> // includes gl.h glu.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>
#include <list>

using namespace std;

struct Point
{
	GLfloat x, y, z;
	GLint state;
};

class Player
{
public:
	int hp;
	float x, y, z;
	int angle;

	Player(int angle) :
		x(100),
		y(5),
		z(-35),
		angle(angle)
	{}

	void ranbertank(bool body)
	{
		glPushMatrix(); // ÅÊÅ©

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
};

#endif