void tower(Tower *object)
{
	glPushMatrix();
	if (object->exist)
	{
		glTranslated(object->x, object->y, object->z);
		glPushMatrix();
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 5);
			glVertex3d(-10, 0, 5);
			glVertex3d(10, 0, 5);
			glVertex3d(10, 22.5, 5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 5);
			glVertex3d(-10, 0, 5);
			glVertex3d(-10, 0, -5);
			glVertex3d(-10, 22.5, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 5);
			glVertex3d(10, 22.5, 5);
			glVertex3d(10, 22.5, -5);
			glVertex3d(-10, 22.5, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		glPopMatrix();

		glRotated(object->angle, 0, 1, 0);
		// 상단 1/4
		glPushMatrix();
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-5, 22.5, 5);
			glVertex3d(5, 22.5, 5);
			glVertex3d(5, 22.5, -5);
			glVertex3d(-5, 22.5, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-5, 30, 5);
			glVertex3d(-5, 22.5, 5);
			glVertex3d(-5, 22.5, -5);
			glVertex3d(-5, 30, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-5, 30, 5);
			glVertex3d(5, 30, 5);
			glVertex3d(5, 22.5, -5);
			glVertex3d(-5, 22.5, -5);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		glPopMatrix();

		// 포신
		glPushMatrix();
		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 28, 5);
		glVertex3d(2, 28, 5);
		glVertex3d(2, 28, 15);
		glVertex3d(-2, 28, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 28, 5);
		glVertex3d(-2, 24, 5);
		glVertex3d(-2, 24, 15);
		glVertex3d(-2, 28, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 24, 5);
		glVertex3d(2, 24, 5);
		glVertex3d(2, 24, 15);
		glVertex3d(-2, 24, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(2, 28, 5);
		glVertex3d(2, 24, 5);
		glVertex3d(2, 24, 15);
		glVertex3d(2, 28, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 28, 15);
		glVertex3d(2, 28, 15);
		glVertex3d(2, 24, 15);
		glVertex3d(-2, 24, 15);
		glEnd();
		glPopMatrix();
	}
	glPopMatrix();
}

void base(Tower *object)
{
	glPushMatrix();
	if (object->exist)
	{
		glTranslated(object->x, object->y, object->z);
		glPushMatrix();
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 10);
			glVertex3d(-10, 0, 10);
			glVertex3d(10, 0, 10);
			glVertex3d(10, 22.5, 10);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 10);
			glVertex3d(-10, 0, 10);
			glVertex3d(-10, 0, -10);
			glVertex3d(-10, 22.5, -10);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(1.0f, 1.0f, 1.0f);
			glVertex3d(-10, 22.5, 10);
			glVertex3d(10, 22.5, 10);
			glVertex3d(10, 22.5, -10);
			glVertex3d(-10, 22.5, -10);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		glPopMatrix();

		glRotated(object->angle, 0, 1, 0);
		// 상단 1/4
		glPushMatrix();
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-8, 22.5, 8);
			glVertex3d(8, 22.5, 8);
			glVertex3d(8, 22.5, -8);
			glVertex3d(-8, 22.5, -8);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-8, 30, 8);
			glVertex3d(-8, 22.5, 8);
			glVertex3d(-8, 22.5, -8);
			glVertex3d(-8, 30, -8);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_QUADS);
			glColor3d(0.0f, 1.0f, 1.0f);
			glVertex3d(-8, 30, 8);
			glVertex3d(8, 30, 8);
			glVertex3d(8, 22.5, -8);
			glVertex3d(-8, 22.5, -8);
			glEnd();
			glRotated(180, 0, 1, 0);
		}
		glPopMatrix();

		// 포신
		glPushMatrix();
		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 29, 5);
		glVertex3d(2, 29, 5);
		glVertex3d(2, 29, 15);
		glVertex3d(-2, 29, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 29, 5);
		glVertex3d(-2, 25, 5);
		glVertex3d(-2, 25, 15);
		glVertex3d(-2, 29, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 25, 5);
		glVertex3d(2, 25, 5);
		glVertex3d(2, 25, 15);
		glVertex3d(-2, 25, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(2, 29, 5);
		glVertex3d(2, 25, 5);
		glVertex3d(2, 25, 15);
		glVertex3d(2, 29, 15);
		glEnd();

		glBegin(GL_QUADS);
		glColor3d(1.0f, 0.0f, 1.0f);
		glVertex3d(-2, 29, 15);
		glVertex3d(2, 29, 15);
		glVertex3d(2, 25, 15);
		glVertex3d(-2, 25, 15);
		glEnd();
		glPopMatrix();
	}
	glPopMatrix();
}

void towerattck(Tower *object, Tank *tankobject)
{
	if (object->cannonball.exist == false && object->cannonball.delaytime == 0 && object->hp > 0)
		{
			if (object->angle)
				object->cannonball.z = object->z - 10;
			else
				object->cannonball.z = object->z +10;
			object->cannonball.x = object->x;
			object->cannonball.track = 0;
			object->cannonball.y = object->y + 25;
			object->cannonball.angle = object->angle + 180;
			object->cannonball.exist = true;
			object->cannonball.delaytime = 80;
		}
		else
		{
			Cannonball_timer(&object->cannonball, 4);

			for (int i = 0; i < 9; i++)
			{
				if (object->cannonball.exist && tankobject[i].hp>0 && collisionball(object->cannonball, tankobject[i].x, tankobject[i].y, tankobject[i].z, 10, 10, 10))
				{
					object->cannonball.exist = false;
					tankobject[i].hp-=2;
				}
				destroytank(&tankobject[i]);
			}
		}
}

void destroytower(Tower *obhect)
{
	if (obhect->hp <= 0 && obhect->exist)
	{
		obhect->y -= 0.1;
		if (obhect->y + 15 < 0)
			obhect->exist = false;
	}
}