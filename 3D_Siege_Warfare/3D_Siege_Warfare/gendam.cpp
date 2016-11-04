#include <GL/glut.h> // includes gl.h glu.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "map.h"

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyborad(unsigned char key, int x, int y);
GLvoid SpecialKeyborad(int key, int x, int y);
GLvoid TimerFunction(int value);

GLint Vyspin, Vxspin;
GLfloat Cmovex, Cmovey, Cmovez;
GLfloat Cannonballx, Cannonbally = 10, Cannonballz;
GLint angle = 0, Rangle = 0, Langle = 0, RLck = 0;
bool viewmode = false;
void main(int argc, char *argv[])
{
	srand((unsigned int)time(NULL));
	//초기화 함수들
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
	glutCreateWindow("3D_Siege_Warfare"); // 윈도우 생성 (윈도우 이름)
	glutKeyboardFunc(Keyborad);
	glutSpecialFunc(SpecialKeyborad);
	glutTimerFunc(10, TimerFunction, 1);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수의 지정
	glutMainLoop();
}
// 윈도우 출력 함수s
GLvoid drawScene(GLvoid)
{
	glClearColor(1, 1, 1, 1.0f); // 바탕은 흰색으로
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	glLoadIdentity();
	if (viewmode)
	{
		gluLookAt(0, 500, 5, 0, 0, 0, 0, 1, 0);
		//glRotated(Vyspin, 0, 1, 0);
		glTranslated(-250, 0, -100);
	}
	else
	{
		glTranslated(Cmovex, 0, Cmovey);
		gluLookAt(0, 10, 30, 0, 10 + Cmovez, 0, 0, 1, 0);
		glRotated(Vxspin, 1, 0, 0);
		glRotated(Vyspin, 0, 1, 0);
	}
	glEnable(GL_DEPTH_TEST);		//은면제거

	glPushMatrix();
	glColor3d(0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3d(-50, 0, -50);
	glVertex3d(-50, 0, 50);
	glVertex3d(50, 0, 50);
	glVertex3d(50, 0, -50);
	glEnd();
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
	if (key == 'q')
		Vyspin -= 5;
	if (key == 'w')
		Vyspin += 5;
	if (key == 'e')
		Vxspin -= 5;
	if (key == 'r')
		Vxspin += 5;
	if (key == '+')
		Cmovez += 5;
	if (key == '-')
		Cmovez -= 5;
	if (key == 'i')
	{
		Vyspin = 0;
		Cmovex = Cmovey = Cmovez = 0;
	}
	if (key == 'm')
		viewmode = (viewmode + 1) % 2;
	glutPostRedisplay();
}

GLvoid SpecialKeyborad(int key, int x, int y)
{
	glutPostRedisplay();
}
GLvoid TimerFunction(int value)
{
	if (RLck)
	{
		Rangle += 1;
		if (Langle > -45)
			Langle -= 1;
		if (Rangle > 45)
			RLck = false;
	}
	else
	{
		Langle += 1;
		if (Rangle > -45)
			Rangle -= 1;
		if (Langle > 45)
			RLck = true;
	}
	glutPostRedisplay();
	glutTimerFunc(10, TimerFunction, 1);
}
