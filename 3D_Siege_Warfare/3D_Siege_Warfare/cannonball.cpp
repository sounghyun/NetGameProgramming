#include <GL/glut.h> // includes gl.h glu.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "struct_package.h"


bool collisionball(Ball p1, float x, float y, float z, float w, float h, float r)
{
	if (p1.x >= x - w && p1.x <= x + w && p1.z >= z - r && p1.z <= z + r && p1.y >= y - h && p1.y <= y + h)
		return true;
	return false;
}

void Cannonball(Ball *cannonball)
{
	glPushMatrix();
	if (cannonball->exist)
	{
		glTranslated(cannonball->x, cannonball->y, cannonball->z);
		glRotated(cannonball->angle, 0, 1, 0);

		glTranslated(0, 0, -6);

		glColor3d(1, 0, 0);

		glutSolidSphere(0.5, 30, 30);

	}
	glPopMatrix();
}

void Cannonball_timer(Ball *cannonball, GLfloat trasum)
{
	if (cannonball->exist)
	{
		cannonball->x -= 1 * sin(cannonball->angle * (3.14 / 180));
		cannonball->z -= 1 * cos(cannonball->angle * (3.14 / 180));
		cannonball->y -= (0.7 * sin(cannonball->track * (3.14 / 180))) - (1 / 2) * 9.8;
		if (cannonball->track < 90)
			cannonball->track += trasum;
		if (cannonball->y < 0)
		{
			cannonball->exist = false;
		}
	}
	if (cannonball->delaytime > 0)
		cannonball->delaytime--;
}