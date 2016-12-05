#ifndef PLAYER_H
#define PLAYER_H

#include "struct_package.h"
#include "cannonball.h"

class Player;

class player_data
{
public:
	int id;
	int hp;
	float x, y, z;
	int angle;
	bool exist;
	Ball cannonball;

	player_data&  operator=(const Player& ref);
};

class Player
{
public:
	int id;
	int hp;
	float x, y, z;
	int angle;
	bool exist;
	Ball cannonball;

	Player(player_data ref) :
		id(ref.id),
		hp(ref.hp),
		x(ref.x),
		y(ref.y),
		z(ref.z),
		angle(ref.angle),
		exist(ref.exist),
		cannonball(ref.cannonball)
	{}

	void destroy();
};

#endif