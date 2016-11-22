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

struct Ball_data
{
	GLfloat x, y, z;
	GLfloat angle;
	bool exist;

	Ball_data& operator=(const Ball& ref) {
		this->x = ref.x;
		this->y = ref.y;
		this->z = ref.z;
		this->angle = ref.angle;
		this->exist = ref.exist;

		return *this;
	}
};
#endif // !CANNONBALL_H




