struct Point
{
	GLfloat x, y, z;
	GLint state;
};

struct Ball
{
	GLfloat x, y, z;
	GLfloat sx, sy, sz;
	GLfloat track, angle;
	GLint delaytime;
	bool exist;
};

struct Tank
{
	int hp;
	float x, y, z;
	int angle, Sangle;
	int time;
	int delaytime;
	int quake;
	bool exist;
	Ball cannonball;
};

struct Tower
{
	int hp;
	float x, y, z;
	float angle;
	bool exist;
	Ball cannonball;
};

struct Guardian
{
	int hp;
	float x, y, z;
	int angle, Sangle;
	float Rangle, Langle;
	int time;
	bool RLck;
	bool exist;
};