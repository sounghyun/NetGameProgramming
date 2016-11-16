#pragma comment(lib, "ws2_32")

#include "struct_package.h"
#include "map.h"
#include "cannonball.h"
#include "tank.h"
#include "tower.h"
#include "basetower.h"
#include "guardian.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 8900

// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);

// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);

GLvoid drawScene(GLvoid);							// �׸���
GLvoid Reshape(int w, int h);						// ȭ�� ����
GLvoid Keyborad(unsigned char key, int x, int y);	// �Ϲ� Ű���� �Է�
GLvoid SpecialKeyborad(int key, int x, int y);		// Ư��Ű �Է�
GLvoid SpecialUPKeyborad(int key, int x, int y);	// Ư��Ű �Է�(Ű��)
GLvoid TimerFunction(int value);					// Ÿ�̸�
GLvoid setup();										// ù ����
template<class Object>
bool collision(Point p1, Object p2);				// ��ü, �� �浹 üũ

void Guardianrecv();
void Tankrecv();

SOCKET sock; // ����
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ

bool viewmode = false;	// �� ��ȯ (1��Ī����, ��ü��)
GLint Ttime = 0;		// �� �ð�
Basetower armybase, enemybase;	// �Ʊ�����, ��������
Guardian_Data* GuardianBuf;
Guardian armyGuardian, enemyGuardian;	// �Ʊ������, ���������
Tower armytower[6], enemytower[6];
Player self(0);
Tank_data* TankBuf;
list<Tank> armytank, enemytank;
GLint LRcontral, UDcontral;
Ball selfball;

void main(int argc, char *argv[])
{
	setup();
	srand((unsigned int)time(NULL));

	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return ;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return ;

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	//�ʱ�ȭ �Լ���
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
	glutCreateWindow("3D_Siege_Warfare"); // ������ ���� (������ �̸�)
	glutKeyboardFunc(Keyborad);
	glutSpecialFunc(SpecialKeyborad);
	glutSpecialUpFunc(SpecialUPKeyborad);
	glutTimerFunc(1, TimerFunction, 1);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ��� ����
	glutMainLoop();

	// �̺�Ʈ ����
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return ;
}

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

// TCP Ŭ���̾�Ʈ ���� �κ�
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ������ ���
	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ��ٸ���

		Tankrecv();
		Guardianrecv();

		system("cls");

		SetEvent(hReadEvent); // �б� �Ϸ� �˸���
	}

	return 0;
}

void Tankrecv()
{
	int retval, num;
	char* buf[sizeof(Tank_data)];

	// �Ʊ� ��ũ
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// ���� �������� �Ʊ� ��ũ �� �޾ƿ���
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	armytank.clear(); // ���� ������ �Ʊ� ��ũ ���� �ʱ�ȭ

	for (int i = 0; i < num; i++) {		// ���� �������� �Ʊ� ��ũ �� ��ŭ �ҷ�����
		retval = recv(sock, (char*)buf, sizeof(Tank_data), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return;
		}
		else if (retval == 0)
			return;

		TankBuf = (Tank_data*)&buf;

		Tank temp(TankBuf);
		armytank.push_back(temp);

//		printf("[TCP Ŭ���̾�Ʈ] �Ʊ� ��ũ ���� armytank�� hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d, wave : %d \n",
//			temp.hp, temp.x, temp.y, temp.z, temp.angle, temp.wave);
	}

	// �Ʊ� ��ũ ���� �Ϸ� �޼��� ������
	retval = send(sock, (char*)TEXT("1���� �Ʊ� ��ũ ���� �Ϸ�\n"), sizeof(char) * 28, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}


	// ���� ��ũ
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// ���� �������� ���� ��ũ �� �޾ƿ���
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	enemytank.clear(); // ���� ������ ���� ��ũ ���� �ʱ�ȭ

	for (int i = 0; i < num; i++) {		// ���� �������� ���� ��ũ �� ��ŭ �ҷ�����
		retval = recv(sock, (char*)buf, sizeof(Tank_data), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return;
		}
		else if (retval == 0)
			return;

		TankBuf = (Tank_data*)&buf;

		Tank temp2(TankBuf);
		enemytank.push_back(temp2);

//		printf("[TCP Ŭ���̾�Ʈ] ���� ��ũ ���� armytank�� hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d wave : %d \n",
//			temp2.hp, temp2.x, temp2.y, temp2.z, temp2.angle, temp2.wave);
	}
	// ���� ��ũ ���� �Ϸ� �޼��� ������
	retval = send(sock, (char*)TEXT("1���� ���� ��ũ ���� �Ϸ�\n"), sizeof(char) * 28, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
}

void Guardianrecv()
{
	int retval;
	char* buf[sizeof(Guardian_Data)];

	// �Ʊ� �����
	retval = recv(sock, (char*)buf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	GuardianBuf = (Guardian_Data*)&buf;

	armyGuardian = *GuardianBuf;

	printf("[TCP Ŭ���̾�Ʈ] �Ʊ� ����� ���� armyGuardian�� hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d, Rangle : %.2f, Langle : %.2f \n",
		armyGuardian.hp, armyGuardian.x, armyGuardian.y, armyGuardian.z, armyGuardian.angle, armyGuardian.Rangle, armyGuardian.Langle);

	// �Ʊ� ����� ���� �Ϸ� �޼��� ������
	retval = send(sock, (char*)TEXT("�Ʊ� ����� ���� �Ϸ�\n"), sizeof(char) * 23, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	// ���� �����
	retval = recv(sock, (char*)buf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	GuardianBuf = (Guardian_Data*)&buf;

	enemyGuardian = *GuardianBuf;

	printf("[TCP Ŭ���̾�Ʈ] �Ʊ� ����� ���� armyGuardian�� hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d, Rangle : %.2f, Langle : %.2f \n",
		enemyGuardian.hp, enemyGuardian.x, enemyGuardian.y, enemyGuardian.z, enemyGuardian.angle, enemyGuardian.Rangle, enemyGuardian.Langle);

	// ���� ����� ���� �Ϸ� �޼��� ������
	retval = send(sock, (char*)TEXT("���� ����� ���� �Ϸ�\n"), sizeof(char) * 23, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
}
// ������ ��� �Լ�s
GLvoid drawScene(GLvoid)
{
	WaitForSingleObject(hReadEvent, INFINITE); // ���� �Ϸ� ��ٸ���
	glClearColor(0, 0, 0, 1.0f); // ������ �������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

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
		glTranslated(-self.x, -self.y, -self.z);
		gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
	}

	glEnable(GL_DEPTH_TEST);		//��������

	glPushMatrix();
	ground();

	armybase.ranberbasetower();
	enemybase.ranberbasetower();

	for (int i = 0; i < 6; i++)
	{
		armytower[i].ranbertower();
		enemytower[i].ranbertower();
		armytower[i].cannonball.ranberCannonball();
		enemytower[i].cannonball.ranberCannonball();
	}
	glPopMatrix();

	glPushMatrix();
	if (viewmode)
		self.ranbertank(true);
	else
		self.ranbertank(false);

	selfball.ranberCannonball();
	glPopMatrix();

	glPushMatrix();
	for (auto d : armytank)
	{
		if (d.exist)
		{
			d.ranbertank(true);
			d.cannonball.ranberCannonball();
		}
	}

	for (auto d : enemytank)
	{
		if (d.exist)
		{
			d.ranbertank(true);
			d.cannonball.ranberCannonball();
		}
	}

	armyGuardian.ranberguardian();
	enemyGuardian.ranberguardian();
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�

	SetEvent(hWriteEvent); // �б� �Ϸ� �˸���
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

	if (self.y > 5)
		self.y-=10;
	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 20; x++)
			for (int z = 0; z < 50; z++)
				if (Map[x][y][z].state == 3 && collision(Map[x][y][z], self))
					self.y += 10;

	selfball.Cannonball_timer(0.2);

	if (selfball.exist)
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 20; x++)
				for (int z = 0; z < 50; z++)
					if (Map[x][y][z].state == 1 && selfball.collisionball(Map[x][y][z].x, Map[x][y][z].y, Map[x][y][z].z, 5, 5, 5))
						selfball.exist = false;

	if (selfball.exist && armybase.hp>0 && selfball.collisionball(armybase.x, armybase.y, armybase.z, 10, 10, 10))
		selfball.exist = false;
	if (selfball.exist && armybase.hp>0 && selfball.collisionball(enemybase.x, enemybase.y, enemybase.z, 10, 10, 10))
	{
		selfball.exist = false;
		if (!enemyGuardian.exist)
			enemybase.hp-=2;
	}
	armybase.destroytower();
	enemybase.destroytower();

	armyGuardian.destroyguardian();
	enemyGuardian.destroyguardian();


	glutPostRedisplay();
	glutTimerFunc(1, TimerFunction, 10);
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
}

template<class Object>
bool collision(Point p1, Object p2)
{
	if (p2.x + 3 >= p1.x - 5 && p2.x - 3 <= p1.x + 5 && p2.z + 3 >= p1.z - 5 && p2.z - 3 <= p1.z + 5 && p2.y >= p1.y -5 && p2.y <= p1.y + 5)
		return true;
	return false;
}