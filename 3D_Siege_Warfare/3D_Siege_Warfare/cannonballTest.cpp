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
GLint angle = 0;
bool viewmode = false;
void main(int argc, char *argv[])
{
	srand((unsigned int)time(NULL));
	//�ʱ�ȭ �Լ���
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
	glutCreateWindow("3D_Siege_Warfare"); // ������ ���� (������ �̸�)
	glutKeyboardFunc(Keyborad);
	glutSpecialFunc(SpecialKeyborad);
	glutTimerFunc(10, TimerFunction, 1);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ��� ����
	glutMainLoop();
}
// ������ ��� �Լ�s
GLvoid drawScene(GLvoid)
{
	glClearColor(1, 1, 1, 1.0f); // ������ �������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

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
		gluLookAt(0, 20, 50, 0, 0 + Cmovez, 0, 0, 1, 0);
		glRotated(Vxspin, 1, 0, 0);
		glRotated(Vyspin, 0, 1, 0);
	}

	glEnable(GL_DEPTH_TEST);		//��������

	glPushMatrix();
	glColor3d(0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3d(-50, 0, -50);
	glVertex3d(-50, 0, 50);
	glVertex3d(50, 0, 50);
	glVertex3d(50, 0, -50);
	glEnd();

	glPopMatrix();

	glPushMatrix();
	glColor3d(1, 0, 0);
	glTranslated(Cannonballx, Cannonbally, Cannonballz);
	glutSolidSphere(1, 30, 30);

	glPopMatrix();


	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
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
	Cannonballx += 0.5 * cos(angle * (3.14/180));
	Cannonbally -= (0.5 * sin(angle * (3.14 / 180))) - (1/2) * 9.8;
	angle += 1;
	if (Cannonbally < 0)
	{
		angle = 0;
		Cannonballx = 0;
		Cannonbally = 10;
	}
	glutPostRedisplay();
	glutTimerFunc(10, TimerFunction, 1);
}
