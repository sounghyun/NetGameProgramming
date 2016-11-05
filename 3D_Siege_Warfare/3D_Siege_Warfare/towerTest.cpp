#include <GL/glut.h> // includes gl.h glu.h
GLint 회전 = 0;
GLint x회전 = 0;

GLvoid drawScene(GLvoid)
{
	glClearColor(0, 0, 0, 1.0f); // 바탕은 흰색으로
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);
	glRotated(회전, 0, 1, 0);
	glRotated(x회전, 1, 0, 0);

	// 하단 3/4
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
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
	if (key == 'g')
		회전 += 5;
	if (key == 'x')
		x회전 += 5;
	glutPostRedisplay();
}
void main(int argc, char *argv[])
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
	glutCreateWindow("실습 28"); // 윈도우 생성 (윈도우 이름)
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutKeyboardFunc(KeyBoard);
	glutReshapeFunc(Reshape); // 다시 그리기 함수의 지정
	glutMainLoop();
}