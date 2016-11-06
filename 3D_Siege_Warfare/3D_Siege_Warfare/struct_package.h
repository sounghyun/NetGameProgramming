#ifndef STRUCK_PACKAGE_H
#define STRUCK_PACKAGE_H

#include <GL/glut.h> // includes gl.h glu.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

class Point
{
public:
	GLfloat x, y, z;
	GLint state;
};

class Ball
{
public:
	GLfloat x, y, z;
	GLfloat sx, sy, sz;
	GLfloat track, angle;
	GLint delaytime;
	bool exist;
};

class Guardian
{
public:
	int hp;
	float x, y, z;
	int w, h, r;
	int angle, Sangle;
	float Rangle, Langle;
	int time;
	bool RLck;
	bool exist;
};

#endif