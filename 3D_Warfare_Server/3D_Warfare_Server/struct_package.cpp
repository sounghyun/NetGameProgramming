#ifndef STRUCT_PACKAGE
#define STRUCT_PACKAGE

#include "struct_package.h"

player_data&  player_data::operator=(const Player& ref)
{
	this->hp = ref.hp;
	this->x = ref.x;
	this->y = ref.y;
	this->z = ref.z;
	this->angle = ref.angle;
	return *this;
}

#endif