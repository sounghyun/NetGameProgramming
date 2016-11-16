#ifndef TANK
#define TANK

#include "tank.h"
#include "tower.h"
#include "basetower.h"
#include "guardian.h"

void Tank::ranbertank(bool body)
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