#ifndef CANNONBALL
#define CANNONBALL

#include "struct_package.h"
#include "cannonball.h"

bool Ball::collisionball(float x, float y, float z, float w, float h, float r)
{
	if (this->x >= x - w && this->x <= x + w && this->z >= z - r && this->z <= z + r && this->y >= y - h && this->y <= y + h)
		return true;
	return false;
}

void Ball::ranberCannonball()
{
	glPushMatrix();
	if (this->exist)
	{
		glTranslated(this->x, this->y, this->z);
		glRotated(this->angle, 0, 1, 0);

		glTranslated(0, 0, -6);

		glColor3d(1, 0, 0);

		glutSolidSphere(0.5, 30, 30);

	}
	glPopMatrix();
}

void Ball::Cannonball_timer(GLfloat trasum)
{
	if (this->exist)
	{
		this->x -= 1 * sin(this->angle * (3.14 / 180));
		this->z -= 1 * cos(this->angle * (3.14 / 180));
		this->y -= (0.7 * sin(this->track * (3.14 / 180))) - (1 / 2) * 9.8;
		if (this->track < 90)
			this->track += trasum;
		if (this->y < 0)
		{
			this->exist = false;
		}
	}
	if (this->delaytime > 0)
		this->delaytime--;
}

#endif // !CANNONBALL