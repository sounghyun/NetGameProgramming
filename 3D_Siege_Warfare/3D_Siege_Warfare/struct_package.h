#ifndef STRUCK_PACKAGE_H
#define STRUCK_PACKAGE_H

#include <GL/glut.h> // includes gl.h glu.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct Point
{
	GLfloat x, y, z;
	GLint state;
};

struct Ball
{
	GLfloat x, y, z;
	GLfloat sx, sy, sz;
	GLfloat track, angle;
	GLint delaytime;
	bool exist;
};

class Tank
{
public:
	int hp;
	float x, y, z;
	int angle, Sangle;
	int time;
	int delaytime;
	int quake;
	bool exist;
	Ball cannonball;

	Tank() {};

	void createtank(float angle, int way);

	void tank(bool body);
	template<class Tower, class Guardian>
	void tankmove(int way, Tank *tankobject, Tower *towerobject, Guardian *guardianobject, Tower *baseobject);

	void destroytank();
};

class Tower
{
public:
	int hp;
	float x, y, z;
	float angle;
	bool exist;
	Ball cannonball;

	Tower() {};

	void setup(int hp, float x, float z, float angle, bool exist );

	int GetHP() { return hp; };
	int GetX() { return x; };
	int GetY() { return y; };
	int GetZ() { return z; };
	int GetAngle() { return angle; };
	int GetExist() { return exist; };

	void tower();

	void towerattck(Tank *tankobject);

	void destroytower();
};

struct Guardian
{
	int hp;
	float x, y, z;
	int angle, Sangle;
	float Rangle, Langle;
	int time;
	bool RLck;
	bool exist;
};

#endif