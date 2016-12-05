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

void Player::destroy()
{
	if (this->hp <= 0 && this->exist)
	{
		this->y -= 0.1;
		if (this->y + 5 < 0)
		{
			this->exist = false;
		}
	}
}

#endif