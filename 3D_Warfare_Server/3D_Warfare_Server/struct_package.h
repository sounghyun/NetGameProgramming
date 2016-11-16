#ifndef STRUCK_PACKAGE_H
#define STRUCK_PACKAGE_H

#include <winsock2.h>
#include <GL/glut.h> // includes gl.h glu.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>
#include <list>

using namespace std;

struct Point
{
	GLfloat x, y, z;
	GLint state;
};

#endif