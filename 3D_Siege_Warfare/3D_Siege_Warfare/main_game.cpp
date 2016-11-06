#include <GL/glut.h> // includes gl.h glu.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "struct_package.h"
#include "map.h"
#include "cannonball.h"
#include "guardian.h"

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyborad(unsigned char key, int x, int y);
GLvoid SpecialKeyborad(int key, int x, int y);
GLvoid SpecialUPKeyborad(int key, int x, int y);
GLvoid TimerFunction(int value);
GLvoid setup();
bool collision(Point p1, Tank p2);
bool quakecollision(Tank p1, float x, float z, float w, float r);

bool viewmode = false;
GLint Ttime = 0;
Point quake[5];
Basetower armybase, enemybase;
Guardian armyGuardian, enemyGuardian;
Tower armytower[6], enemytower[6];
Tank self, armytank[9], enemytank[9];
GLint LRcontral, UDcontral;
Ball selfball;

void main(int argc, char *argv[])
{
	setup();
	srand((unsigned int)time(NULL));
	//초기화 함수들
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
	glutCreateWindow("3D_Siege_Warfare"); // 윈도우 생성 (윈도우 이름)
	glutKeyboardFunc(Keyborad);
	glutSpecialFunc(SpecialKeyborad);
	glutSpecialUpFunc(SpecialUPKeyborad);
	glutTimerFunc(1, TimerFunction, 1);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수의 지정
	glutMainLoop();
}
// 윈도우 출력 함수s
GLvoid drawScene(GLvoid)
{
	glClearColor(0, 0, 0, 1.0f); // 바탕은 흰색으로
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	glLoadIdentity();
	if (viewmode)
	{
		gluLookAt(0, 500, 5, 0, 0, 0, 0, 1, 0);
		glRotated(-90, 0, 1, 0);
		glTranslated(-100, 0, 250);
	}
	else
	{
		glRotated(-self.angle, 0, 1, 0);
		glTranslated(-self.x, -self.y - self.quake, -self.z);
		gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
	}

	glEnable(GL_DEPTH_TEST);		//은면제거

	glPushMatrix();
	ground();

	armybase.basetower();
	enemybase.basetower();

	for (int i = 0; i < 6; i++)
	{
		armytower[i].tower();
		enemytower[i].tower();
		Cannonball(&armytower[i].cannonball);
		Cannonball(&enemytower[i].cannonball);
	}
	glPopMatrix();

	glPushMatrix();
	if (viewmode)
		self.tank(true);
	else
		self.tank(false);

	Cannonball(&selfball);
	glPopMatrix();

	glPushMatrix();
	for (int i = 0; i < 9; i++)
	{
		if (armytank[i].exist)
		{
			armytank[i].tank(true);
			Cannonball(&armytank[i].cannonball);
		}

		if (enemytank[i].exist)
		{
			enemytank[i].tank(true);
			Cannonball(&enemytank[i].cannonball);
		}
	}

	guardian(&armyGuardian);
	guardian(&enemyGuardian);
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, w / h, 1.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GLvoid Keyborad(unsigned char key, int x, int y)
{
	if (key == 'i')
	{
		self.angle = 0;
		self.y = 5;
		self.z = -35;
		self.x = 100;
	}
	if (key == 'm')
		viewmode = (viewmode + 1) % 2;

	if (key == 32 && selfball.exist == false && selfball.delaytime == 0)
	{
		selfball.z = self.z + 1;
		selfball.x = self.x;
		selfball.track = 0;
		selfball.y = self.y - 1;
		selfball.angle = self.angle;
		selfball.exist = true;
		selfball.delaytime = 60;
	}

	glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
	glutPostRedisplay();
}

GLvoid SpecialKeyborad(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		UDcontral = 1;
	if (key == GLUT_KEY_DOWN)
		UDcontral = 2;
	if (key == GLUT_KEY_LEFT)
		LRcontral = 1;
	if (key == GLUT_KEY_RIGHT)
		LRcontral = 2;
	glutPostRedisplay();
}
GLvoid SpecialUPKeyborad(int key, int x, int y)
{
	if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)
		UDcontral = 0;
	if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT)
		LRcontral = 0;
	glutPostRedisplay();
}
GLvoid TimerFunction(int value)
{
	Ttime++;
	if (Ttime > 800)
	{
		for (int i = 0; i < 6; i++)
		{
			if (!armytank[i].exist)
				armytank[i].createtank(0, i % 3);
			if (!enemytank[i].exist)
				enemytank[i].createtank(180, i % 3);
		}
	}
	if (Ttime > 1600)
	{
		for (int i = 0; i < 9; i++)
		{
			if (!armytank[i].exist)
				armytank[i].createtank(0, i % 3);
			if (!enemytank[i].exist)
				enemytank[i].createtank(180, i % 3);
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (!armytank[i].exist)
				armytank[i].createtank(0, i % 3);
			if (!enemytank[i].exist)
				enemytank[i].createtank(180, i % 3);
		}
	}
	if (UDcontral == 1)
	{
		self.x -= 0.5*sin(self.angle * (3.14 / 180));
		self.z -= 0.5*cos(self.angle * (3.14 / 180));
		for (int y = 0; y < 3; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				for (int z = 0; z < 50; z++)
				{
					if (((Map[x][y][z].state != 2 && Map[x][y][z].state != 4 && Map[x][y][z].state != 3 && Map[x][y][z].state) || (Map[x][y][z].state == 2 && (z<5 || z > 45))) && collision(Map[x][y][z], self))
					{
						self.x += 0.5*sin(self.angle * (3.14 / 180));
						self.z += 0.5*cos(self.angle * (3.14 / 180));
					}
				}
			}
		}
	}
	if (UDcontral == 2)
	{
		self.x += 0.5*sin(self.angle * (3.14 / 180));
		self.z += 0.5*cos(self.angle * (3.14 / 180));
		for (int y = 0; y < 3; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				for (int z = 0; z < 50; z++)
				{
					if (((Map[x][y][z].state != 2 && Map[x][y][z].state != 4 && Map[x][y][z].state) || (Map[x][y][z].state == 2 && (z<5 || z > 45))) && collision(Map[x][y][z], self))
					{
						self.x -= 0.5*sin(self.angle * (3.14 / 180));
						self.z -= 0.5*cos(self.angle * (3.14 / 180));
					}
				}
			}
		}
	}
	if (LRcontral == 1)
		self.angle += 1;
	if (LRcontral == 2)
		self.angle -= 1;

	if (UDcontral)
	{
		int i;
		for ( i = 0; i < 5; i++)
		{
			if (quakecollision(self, quake[i].x, quake[i].z, 10, 10))
			{
				self.quake = (self.quake + 1) % 5;
				break;
			}
		}
		if (i == 5)
			self.quake = 0;

	}

	if (self.y > 5)
		self.y-=10;
	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 20; x++)
			for (int z = 0; z < 50; z++)
				if (Map[x][y][z].state == 3 && collision(Map[x][y][z], self))
					self.y += 10;

	Cannonball_timer(&selfball, 0.2);

	if (selfball.exist)
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 20; x++)
				for (int z = 0; z < 50; z++)
					if (Map[x][y][z].state == 1 && collisionball(selfball, Map[x][y][z].x, Map[x][y][z].y, Map[x][y][z].z, 5, 5, 5))
						selfball.exist = false;

	if (selfball.exist && armybase.hp>0 && collisionball(selfball, armybase.x, armybase.y, armybase.z, 10, 10, 10))
		selfball.exist = false;
	if (selfball.exist && armybase.hp>0 && collisionball(selfball, enemybase.x, enemybase.y, enemybase.z, 10, 10, 10))
	{
		selfball.exist = false;
		if (!enemyGuardian.exist)
			enemybase.hp-=2;
	}
	armybase.destroytower();
	enemybase.destroytower();

	for (int i = 0; i < 6; i++)
	{
		if (selfball.exist && armytower[i].hp>0 && collisionball(selfball, armytower[i].x, armytower[i].y, armytower[i].z, 10, 10, 5))
			selfball.exist = false;
		if (selfball.exist && enemytower[i].hp>0 && collisionball(selfball, enemytower[i].x, enemytower[i].y, enemytower[i].z, 10, 10, 5))
		{
			selfball.exist = false;
			enemytower[i].hp-=2;
		}
		armytower[i].towerattck(enemytank);
		enemytower[i].towerattck(armytank);
		armytower[i].destroytower();
		enemytower[i].destroytower();
	}

	if (selfball.exist && armyGuardian.hp > 0 && collisionball(selfball, armyGuardian.x, armyGuardian.y, armyGuardian.z, 10, 15, 5))
		selfball.exist = false;
	if (selfball.exist && enemyGuardian.hp > 0 && collisionball(selfball, enemyGuardian.x, enemyGuardian.y, enemyGuardian.z, 10, 15, 5))
	{
		selfball.exist = false;
		enemyGuardian.hp-=2;
	}
	destroyguardian(&armyGuardian);
	destroyguardian(&enemyGuardian);

	for (int i = 0; i < 9; i++)
	{
		if (selfball.exist && armytank[i].hp>0 && collisionball(selfball, armytank[i].x, armytank[i].y, armytank[i].z, 10, 10, 10))
			selfball.exist = false;
		if (selfball.exist && enemytank[i].hp>0 && collisionball(selfball, enemytank[i].x, enemytank[i].y, enemytank[i].z, 10, 10, 10))
		{
			selfball.exist = false;
			enemytank[i].hp-=2;
		}
		armytank[i].destroytank();
		enemytank[i].destroytank();
	}

	for (int i = 0; i < 9; i++)
	{
		if (armytank[i].exist) {
			for (int j = 0; j < 9; j++)
				armytank[i].tankmove(i % 3 + 1, &enemytank[j]);
			for (int j = 0; j < 6; j++)
				armytank[i].tankmove(i % 3 + 1, &enemytower[j]);
			armytank[i].tankmove(i % 3 + 1, &enemyGuardian);
			armytank[i].tankmove(i % 3 + 1, &enemybase);
		}
		if (enemytank[i].exist) {
			enemytank[i].tankmove(i % 3 + 1, armytank);
			enemytank[i].tankmove(i % 3 + 1, armytower);
			enemytank[i].tankmove(i % 3 + 1, &armyGuardian);
			enemytank[i].tankmove(i % 3 + 1, &armybase);
		}
	}
	guardianmove(&armyGuardian);
	guardianmove(&enemyGuardian);

	glutPostRedisplay();
	glutTimerFunc(1, TimerFunction, 1);
}

GLvoid setup()
{
	mapload();

	self.angle = 0;
	self.y = 5;
	self.z = -35;
	self.x = 100;

	armybase.setup(20, 100, -20, 180, true);
	enemybase.setup(20, 100, -480, 0, true);

	quake[0].x = 60;	quake[0].z = -120;
	quake[1].x = 140;	quake[1].z = -120;
	quake[2].x = 60;	quake[2].z = -380;
	quake[3].x = 140;	quake[3].z = -380;
	quake[4].x = 100;	quake[4].z = -250;

	int armycount = 0, enemycount = 0;
	for (int x = 0; x < 20; x++)
	{
		for (int z = 5; z < 45; z++)
		{
			if (Map[x][0][z].state == 2 && Map[x - 1][0][z].state == 2)
			{
				if (z < 25)
				{
					armytower[armycount].setup(10, \
						(Map[x][0][z].x + Map[x - 1][0][z].x) / 2, Map[x][0][z].z,\
						180, true);
					armycount = (armycount + 1) % 6;
				}
				else
				{
					enemytower[enemycount].setup(10, \
						(Map[x][0][z].x + Map[x - 1][0][z].x) / 2, Map[x][0][z].z, \
						0, true);
					enemycount = (enemycount + 1) % 6;
				}
			}
		}
	}

	createguardian(&armyGuardian, 0);
	createguardian(&enemyGuardian, 180);

	for (int i = 0; i < 3; i++)
	{
		armytank[i].createtank(0, i);
		enemytank[i].createtank(180, i);
	}
}

bool collision(Point p1, Tank p2)
{
	if (p2.x + 3 >= p1.x - 5 && p2.x - 3 <= p1.x + 5 && p2.z + 3 >= p1.z - 5 && p2.z - 3 <= p1.z + 5 && p2.y >= p1.y -5 && p2.y <= p1.y + 5)
		return true;
	return false;
}

bool quakecollision(Tank p1, float x, float z, float w, float r)
{
	if (p1.x + 3 >= x - w && p1.x - 3 <= x + w && p1.z + 3 >= z - r && p1.z - 3 <= z + r)
		return true;
	return false;
}