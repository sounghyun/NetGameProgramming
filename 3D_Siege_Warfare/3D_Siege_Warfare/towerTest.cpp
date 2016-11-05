#include <GL/glut.h> // includes gl.h glu.h
GLint ȸ�� = 0;
GLint xȸ�� = 0;

GLvoid drawScene(GLvoid)
{
	glClearColor(0, 0, 0, 1.0f); // ������ �������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);
	glRotated(ȸ��, 0, 1, 0);
	glRotated(xȸ��, 1, 0, 0);

	// �ϴ� 3/4
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

	// ��� 1/4
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

	// ����
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
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
	if (key == 'g')
		ȸ�� += 5;
	if (key == 'x')
		xȸ�� += 5;
	glutPostRedisplay();
}
void main(int argc, char *argv[])
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
	glutCreateWindow("�ǽ� 28"); // ������ ���� (������ �̸�)
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutKeyboardFunc(KeyBoard);
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ��� ����
	glutMainLoop();
}