#ifndef PLAYER_H
#define PLAYER_H

#include "struct_package.h"
#include "cannonball.h"

class Player;

struct player_data
{
	int id;
	int hp;
	float x, y, z;
	int angle;
	Ball cannonball;

	player_data& operator=(const Player& ref);
};

class Player
{
public:
	int id;
	int hp;
	float x, y, z;
	int angle;
	Ball cannonball;

	Player(int angle) :
		x(100),
		y(0),
		z(-35),
		angle(angle)
	{}

	Player(player_data ref) :
		id(ref.id),
		hp(ref.hp),
		x(ref.x),
		y(ref.y),
		z(ref.z),
		angle(ref.angle),
		cannonball(ref.cannonball)
	{}

	void ranbertank(bool body);

	void ballcreate();

};

#endif