#ifndef PLAYER
#define PLAYER

#include "player.h"

player_data&  player_data::operator=(const Player& ref)
{
	this->id = ref.id;
	this->hp = ref.hp;
	this->x = ref.x;
	this->y = ref.y;
	this->z = ref.z;
	this->angle = ref.angle;
	this->exist = ref.exist;
	this->cannonball = ref.cannonball;

	return *this;
}

void Player::create(int number)
{
	this->hp = 2;
	this->y = 0;
	this->x = 100;
	this->id = number;

	if (this->id % 2 == 1)
	{
		this->z = -465;
		this->angle = 180;
	}
	else
	{
		this->z = -35;
		this->angle = 0;
	}
	this->exist = true;
}

void Player::ranbertank(bool body)
{
	glPushMatrix(); // ÅÊÅ©

	if (body)
	{
		glTranslated(this->x, this->y, this->z + 1);
		glRotated(this->angle, 0, 1, 0);

		glScaled(0.5, 0.5, 0.5);

		glColor3d(0, 0, 0);
		glTranslated(0, 2, 0);
		glScaled(2, 1, 2);
		glutSolidCube(5);

		if (this->id % 2)
			glColor3d(0, 0, 1);
		else
			glColor3d(1, 0, 0);
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
		glTranslated(this->x, this->y, this->z + 1);
		glRotated(this->angle, 0, 1, 0);

		glColor3d(1, 0, 1);
		glTranslated(0, 3.5, -4);
		glRotated(30, 1, 0, 0);
		glScaled(1, 0.25, 3);
		glutSolidCube(0.75);
	}

	glPopMatrix();

}

void Player::ballcreate()
{
	this->cannonball.z = this->z + 1;
	this->cannonball.x = this->x;
	this->cannonball.track = 0;
	this->cannonball.y = this->y + 4;
	this->cannonball.angle = this->angle;
	this->cannonball.exist = true;
	this->cannonball.delaytime = 60;
}

#endif