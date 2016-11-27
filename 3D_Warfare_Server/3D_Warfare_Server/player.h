#ifndef PLAYER_H
#define PLAYER_H

#include "struct_package.h"
#include "cannonball.h"

class Player;

class player_data
{
public:
	int hp;
	float x, y, z;
	int angle;
	Ball cannonball;

	player_data&  operator=(const Player& ref);
};

class Player
{
public:
	int hp;
	float x, y, z;
	int angle;
	Ball cannonball;

	Player(player_data ref) :
		hp(ref.hp),
		x(ref.x),
		y(ref.y),
		z(ref.z),
		angle(ref.angle),
		cannonball(ref.cannonball)
	{}
};

#endif