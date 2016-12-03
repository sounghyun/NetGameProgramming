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
	this->cannonball = ref.cannonball;

	return *this;
}

#endif