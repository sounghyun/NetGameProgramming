

Point Map[20][3][50];

GLvoid mapload()
{
	FILE* mapdata = fopen("mapdata.txt", "r");

	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 20; x++)
			for (int z = 0; z < 50; z++)
				fscanf(mapdata, "%f %f %f %d", &Map[x][y][z].x, &Map[x][y][z].y, &Map[x][y][z].z, &Map[x][y][z].state);
	fclose(mapdata);
}

GLvoid ground()
{
	glPushMatrix();
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			for (int z = 0; z < 50; z++)
			{
				glPushMatrix();
				glTranslated(Map[x][y][z].x, Map[x][y][z].y, Map[x][y][z].z);
				if (Map[x][y][z].state == 1)
					glColor3d(0, 1, 0);
				if (Map[x][y][z].state == 3)
					glColor3d(1, 0, 1);
				if (Map[x][y][z].state && Map[x][y][z].state != 2 && Map[x][y][z].state != 4)
					glutSolidCube(10);
				if (y == 0)
				{
					if (Map[x][y][z].state == 4)
						glColor3d(1, 1, 0);
					else
						glColor3d(0.5, 1, 0.5);
					glTranslated(0, -Map[x][y][z].y, 0);
					glScaled(1, 0, 1);
					glutSolidCube(10);
				}
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
}

