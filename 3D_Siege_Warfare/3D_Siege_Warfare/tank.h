#ifndef TANK_H
#define TANK_H


void createtank(Tank *tank, float angle, int way);

void tank(Tank *tank, bool body);

void tankmove(Tank * tank, int way, Tank *tankobject, Tower *towerobject, Guardian *guardianobject, Tower *baseobject);

void destroytank(Tank *object);

#endif // !TANK_H