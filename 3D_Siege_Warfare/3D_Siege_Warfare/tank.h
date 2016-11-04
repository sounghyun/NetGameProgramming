void createtank(Tank *tank, float angle, int way)
{
	tank->hp = 5;
	tank->Sangle = angle;
	tank->angle = 90 * (way - 1) + angle;
	if (tank->Sangle)
		tank->z = -450;
	else
		tank->z = -50;
	tank->x = 100;
	tank->y = 0;
	tank->exist = true;
	tank->delaytime = 1500;
	if (way == 1)
		tank->time = 2000;
	else
		tank->time = 2730;
}

void tank(Tank *tank, bool body)
{
	glPushMatrix(); // ÅÊÅ©
	if (!body)
		glTranslated(0, tank->quake, 0);

	glTranslated(tank->x, tank->y, tank->z + 1);
	glRotated(tank->angle, 0, 1, 0);

	if (body)
	{
		glScaled(0.5, 0.5, 0.5);

		glColor3d(0, 0, 1);
		glTranslated(0, 2, 0);
		glScaled(2, 1, 2);
		glutSolidCube(5);


		glTranslated(0, 2, 0);
		glScaled(1, 2.5, 1);
		glutSolidCube(3);

		glScaled(0.5, 1, 0.5);

		glColor3d(1, 0, 1);
		glTranslated(0, 1, -5);
		glScaled(1, 0.5, 5);
		glutSolidCube(1);
	}
	else
	{
		glColor3d(1, 0, 1);
		glTranslated(0, -1, -2);
		glRotated(30, 1, 0, 0);
		glScaled(0.5, 0.25, 3);
		glutSolidCube(0.75);
	}

	glPopMatrix();

}

void tankmove(Tank * tank, int way, Tank *tankobject, Tower *towerobject, Guardian *guardianobject, Tower *baseobject)
{
	if (tank->hp > 0)
	{
		tank->time--;
		if (tank->time < - 300)
			tank->exist = false;

		if (way == 1)
		{
			if (tank->time < 2400 && tank->time && tank->angle < 0 + tank->Sangle)
				tank->angle++;
			if (tank->time < 430 && tank->time && tank->angle < 90 + tank->Sangle)
				tank->angle++;
			if (tank->time == 0 && tank->angle > 0 + tank->Sangle)
				tank->angle--;
		}
		if (way == 3)
		{
			if (tank->time < 2400 && tank->time && tank->angle > 0 + tank->Sangle)
				tank->angle--;
			if (tank->time < 430 && tank->time && tank->angle > -90 + tank->Sangle)
				tank->angle--;
			if (tank->time == 0 && tank->angle < 0 + tank->Sangle)
				tank->angle++;
		}

		if (tank->time > 0)
		{
			tank->x -= 0.2*sin(tank->angle * (3.14 / 180));
			tank->z -= 0.2*cos(tank->angle * (3.14 / 180));
		}
	}
	if (tank->cannonball.exist == false && tank->cannonball.delaytime == 0 && tank->hp > 0)
	{
		if (tank->angle)
			tank->cannonball.z = tank->z - 2;
		else
			tank->cannonball.z = tank->z + 2;
		tank->cannonball.x = tank->x;
		tank->cannonball.track = 0;
		tank->cannonball.y = tank->y + 3;
		tank->cannonball.angle = tank->angle;
		tank->cannonball.exist = true;
		tank->cannonball.delaytime = 60;
	}
	else
	{
		Cannonball_timer(&tank->cannonball, 2);

		if (tank->cannonball.exist && baseobject->hp>0 && collisionball(tank->cannonball, baseobject->x, baseobject->y, baseobject->z, 10, 10, 10))
		{
			tank->cannonball.exist = false;
			if (!guardianobject->exist)
				baseobject->hp--;
		}

		for (int i = 0; i < 6; i++)
		{
			if (tank->cannonball.exist && towerobject[i].hp>0 && collisionball(tank->cannonball, towerobject[i].x, towerobject[i].y, towerobject[i].z, 10, 10, 5))
			{
				tank->cannonball.exist = false;
				towerobject[i].hp--;
			}
		}

		if (tank->cannonball.exist && guardianobject->hp > 0 && collisionball(tank->cannonball, guardianobject->x, guardianobject->y, guardianobject->z, 10, 15, 5))
		{
			tank->cannonball.exist = false;
			guardianobject->hp--;
		}

		for (int i = 0; i < 9; i++)
		{
			if (tank->cannonball.exist && tankobject[i].hp>0 && collisionball(tank->cannonball, tankobject[i].x, tankobject[i].y, tankobject[i].z, 10, 10, 10))
			{
				tank->cannonball.exist = false;
				tankobject[i].hp--;
			}
		}
	}
}

void destroytank(Tank *object)
{
	if (object->hp <= 0 && object->exist)
	{
		object->y -= 0.1;
		if (object->y + 5 < 0)
		{
			object->delaytime--;
			if (object->delaytime == 0)
				object->exist = false;
		}
	}
}