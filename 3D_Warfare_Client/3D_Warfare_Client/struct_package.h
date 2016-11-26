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
#include <vector>

using namespace std;

struct Point
{
	GLfloat x, y, z;
	GLint state;
};

struct player_data;

class Player
{
public:
	int hp;
	float x, y, z;
	int angle;

	Player(int angle) :
		x(100),
		y(0),
		z(-35),
		angle(angle)
	{}


	void ranbertank(bool body)
	{
		glPushMatrix(); // ÅÊÅ©



		if (body)
		{
			glTranslated(this->x, this->y, this->z + 1);
			glRotated(this->angle, 0, 1, 0);

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
			glTranslated(this->x, this->y, this->z + 1);
			glRotated(this->angle, 0, 1, 0);

			glColor3d(1, 0, 1);
			glTranslated(0, -1, -2);
			glRotated(30, 1, 0, 0);
			glScaled(0.5, 0.25, 3);
			glutSolidCube(0.75);
		}

		glPopMatrix();

	}

};

struct player_data
{
	int hp;
	float x, y, z;
	int angle;
	player_data& operator=(const Player& ref) {
		this->hp = ref.hp;
		this->x = ref.x;
		this->y = ref.y;
		this->z = ref.z;
		this->angle = ref.angle;
		return *this;
	}
};

#endif