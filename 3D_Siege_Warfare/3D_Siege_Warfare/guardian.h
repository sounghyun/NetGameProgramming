void createguardian(Guardian *object, float angle)
{
	object->hp = 20;
	object->Sangle = angle;
	object->angle = 90 + angle;
	if (object->Sangle)
		object->z = -460;
	else
		object->z = -40;
	object->x = 100;
	object->y = 0;
	object->exist = true;
	object->time = 250;
}

void guardian(Guardian *object)
{
	glPushMatrix();
	if (object->Rangle < 0)
		glTranslated(object->x, object->y + object->Rangle * 0.02, object->z);
	else
		glTranslated(object->x, object->y + object->Langle * 0.02, object->z);

	glRotated(object->angle, 0, 1, 0);

	glPushMatrix();			//¸Ó¸®
	glColor3d(0, 0, 1);
	glTranslated(0, 15, 0);
	glutSolidSphere(2, 30, 30);
	glPopMatrix();

	glPushMatrix();			//¸öÅë
	glTranslated(0, 10, 0);
	glScaled(1, 2, 0.7);
	glutSolidCube(3.5);
	glPopMatrix();

	glPushMatrix();			//¿ÞÂÊÆÈ
	glTranslated(-2, 12.5, 0);
	glScaled(1, 1, 1);
	glutSolidSphere(0.75, 30, 30);		//¾î±ú

	glRotated(-object->Rangle, 1, 0, 0);

	glRotated(-30, 0, 0, 1);

	glTranslated(0, -1.5, 0);
	glScaled(1, 3, 1);
	glutSolidCube(1);							//ÆÈ
	glPopMatrix();


	glPushMatrix();			//¿À¸¥ÂÊÆÈ
	glTranslated(2, 12.5, 0);
	glScaled(1, 1, 1);
	glutSolidSphere(0.75, 30, 30);		//¾î±ú

	glRotated(-object->Langle, 1, 0, 0);

	glRotated(30, 0, 0, 1);

	glTranslated(0, -1.5, 0);
	glScaled(1, 3, 1);
	glutSolidCube(1);							//ÆÈ
	glPopMatrix();

	glPushMatrix();			//¿ÞÂÊ ÇÏÃ¼
	glTranslated(-0.75, 6, 0);
	glutSolidSphere(0.75, 30, 30);	//°üÀý1

	glRotated(-object->Langle, 1, 0, 0);

	glTranslated(0, -1.5, 0);
	glScaled(1, 2, 1);
	glutSolidCube(1);						//´Ù¸®1
	glScaled(1, 0.5, 1);

	glTranslated(0, -1.5, 0);
	glutSolidSphere(0.75, 30, 30);	//°üÀý2

	glRotated(object->Langle, 1, 0, 0);

	glTranslated(0, -1, 0);
	glScaled(1, 2, 1);
	glutSolidCube(1);						//´Ù¸®2
	glScaled(1, 0.5, 1);

	glTranslated(0, -1, 0.5);
	glScaled(2, 1, 3);
	glutSolidSphere(0.5, 30, 30);		//¹ß
	glPopMatrix();

	glPushMatrix();			//¿À¸¥ÂÊ ÇÏÃ¼
	glTranslated(0.75, 6, 0);
	glutSolidSphere(0.75, 30, 30);	//°üÀý1

	glRotated(-object->Rangle, 1, 0, 0);

	glTranslated(0, -1.5, 0);
	glScaled(1, 2, 1);
	glutSolidCube(1);						//´Ù¸®1
	glScaled(1, 0.5, 1);

	glTranslated(0, -1.5, 0);
	glutSolidSphere(0.75, 30, 30);	//°üÀý2

	glRotated(object->Rangle, 1, 0, 0);

	glTranslated(0, -1, 0);
	glScaled(1, 2, 1);
	glutSolidCube(1);						//´Ù¸®2
	glScaled(1, 0.5, 1);

	glTranslated(0, -1, 0.5);
	glScaled(2, 1, 3);
	glutSolidSphere(0.5, 30, 30);		//¹ß
	glPopMatrix();

	glPopMatrix();
}

void guardianmove(Guardian *object)
{ 
	if (object->hp > 0)
	{
		if (object->time % 500 == 0)
		{
			object->angle += 5;
			if ((object->angle - 90) % 180 == 0)
				object->time++;
		}
		else
		{
			object->x -= 0.2*sin(object->angle * (3.14 / 180));
			object->z -= 0.2*cos(object->angle * (3.14 / 180));
			object->time++;
		}

		if (object->RLck)
		{
			object->Rangle += 1;
			if (object->Langle > -45)
				object->Langle -= 1;
			if (object->Rangle > 45)
				object->RLck = false;
		}
		else
		{
			object->Langle += 1;
			if (object->Rangle > -45)
				object->Rangle -= 1;
			if (object->Langle > 45)
				object->RLck = true;
		}
	}
}

void destroyguardian(Guardian *obhect)
{
	if (obhect->hp <= 0 && obhect->exist)
	{
		obhect->y -= 0.1;
		if (obhect->y + 15 < 0)
			obhect->exist = false;
	}
}