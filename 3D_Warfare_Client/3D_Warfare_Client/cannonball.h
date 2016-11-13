#ifndef CANNONBALL_H
#define CANNONBALL_H

#include "struct_package.h"

class Ball
{
public:
	GLfloat x, y, z;
	GLfloat sx, sy, sz;
	GLfloat track, angle;
	GLint delaytime;
	bool exist;

	int GetX() { return x; };
	int GetY() { return y; };
	int GetZ() { return z; };
	int GetAngle() { return angle; };
	int GetExist() { return exist; };

	bool collisionball(float x, float y, float z, float w, float h, float r);
	void ranberCannonball();
	void Cannonball_timer(GLfloat trasum);
};

#endif // !CANNONBALL_H




