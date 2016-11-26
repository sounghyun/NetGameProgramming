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

class Player;

class player_data
{
public:
	int hp;
	float x, y, z;
	int angle;

	player_data&  operator=(const Player& ref);
};

class Player
{
public:
	int hp;
	float x, y, z;
	int angle;

	Player() {}

	Player(player_data ref) :
	hp(ref.hp),
	x(ref.x),
	y(ref.y),
	z(ref.z),
	angle(ref.angle)
	{}
};


#endif