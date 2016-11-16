#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <GL/glut.h> // includes gl.h glu.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "struct_package.h"
#include "map.h"
#include "cannonball.h"
#include "tank.h"
#include "tower.h"
#include "basetower.h"
#include "guardian.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000

// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);

// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

GLvoid drawScene(GLvoid);							// 그리기
GLvoid Reshape(int w, int h);						// 화면 설정
GLvoid Keyborad(unsigned char key, int x, int y);	// 일반 키보드 입력
GLvoid SpecialKeyborad(int key, int x, int y);		// 특수키 입력
GLvoid SpecialUPKeyborad(int key, int x, int y);	// 특수키 입력(키업)
GLvoid TimerFunction(int value);					// 타이머
GLvoid setup();										// 첫 설정
bool collision(Point p1, Tank p2);					// 플레이어, 맵 충돌체크
bool quakecollision(Tank p1, float x, float z, float w, float r);	// 플레이어 지진 충돌체크


SOCKET sock; // 소켓
HANDLE hReadEvent, hWriteEvent; // 이벤트

bool viewmode = false;	// 뷰 전환 (1인칭시점, 전체뷰)
GLint Ttime = 0;		// 총 시간
Point quake[5];			// 지진
Basetower armybase, enemybase;	// 아군본부, 적군본부
Guardian_Data* GuardianBuf;
Guardian armyGuardian, enemyGuardian;	// 아군가디언, 적군가디언
Tower armytower[6], enemytower[6];
Tank self, armytank[9], enemytank[9];
GLint LRcontral, UDcontral;
Ball selfball;

void main(int argc, char *argv[])
{
	setup();
	srand((unsigned int)time(NULL));

	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return ;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return ;

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

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

	// 이벤트 제거
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return ;
}

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

// 사용자 정의 데이터 수신 함수
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

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// 윈속 초기화
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

	// 서버와 데이터 통신
	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기

		char* buf[sizeof(Guardian_Data)];

		// 아군 가디언
		retval = recv(sock, (char*)buf, sizeof(Guardian_Data), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		GuardianBuf = (Guardian_Data*)&buf;

		armyGuardian = *GuardianBuf;

		printf("[TCP 클라이언트] 아군 가디언 정보 armyGuardian의 hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d, Rangle : %.2f, Langle : %.2f \n", 
			armyGuardian.hp, armyGuardian.x, armyGuardian.y, armyGuardian.z, armyGuardian.angle, armyGuardian.Rangle, armyGuardian.Langle);

		// 아군 가디언 수신 완료 메세지 보내기
		retval = send(sock, (char*)TEXT("아군 가디언 수신 완료\n"), sizeof(char) * 23, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		// 적군 가디언
		retval = recv(sock, (char*)buf, sizeof(Guardian_Data), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		GuardianBuf = (Guardian_Data*)&buf;

		enemyGuardian = *GuardianBuf;

		printf("[TCP 클라이언트] 아군 가디언 정보 armyGuardian의 hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d, Rangle : %.2f, Langle : %.2f \n",
			enemyGuardian.hp, enemyGuardian.x, enemyGuardian.y, enemyGuardian.z, enemyGuardian.angle, enemyGuardian.Rangle, enemyGuardian.Langle);

		// 적군 가디언 수신 완료 메세지 보내기
		retval = send(sock, (char*)TEXT("적군 가디언 수신 완료\n"), sizeof(char) * 23, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
		SetEvent(hReadEvent); // 읽기 완료 알리기
	}

	return 0;
}

// 윈도우 출력 함수s
GLvoid drawScene(GLvoid)
{
	WaitForSingleObject(hReadEvent, INFINITE); // 쓰기 완료 기다리기
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
	for (int i = 0; i < 9; i++)
	{
		if (armytank[i].exist)
		{
			armytank[i].ranbertank(true);
			armytank[i].cannonball.ranberCannonball();
		}

		if (enemytank[i].exist)
		{
			enemytank[i].ranbertank(true);
			enemytank[i].cannonball.ranberCannonball();
		}
	}

	armyGuardian.ranberguardian();
	enemyGuardian.ranberguardian();
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers(); // 화면에 출력하기

	SetEvent(hWriteEvent); // 읽기 완료 알리기
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

	for (int i = 0; i < 6; i++)
	{
		if (selfball.exist && armytower[i].hp>0 && selfball.collisionball(armytower[i].x, armytower[i].y, armytower[i].z, 10, 10, 5))
			selfball.exist = false;
		if (selfball.exist && enemytower[i].hp>0 && selfball.collisionball(enemytower[i].x, enemytower[i].y, enemytower[i].z, 10, 10, 5))
		{
			selfball.exist = false;
			enemytower[i].hp-=2;
		}
		armytower[i].towerattck(enemytank);
		enemytower[i].towerattck(armytank);
		armytower[i].destroytower();
		enemytower[i].destroytower();
	}

	armyGuardian.destroyguardian();
	enemyGuardian.destroyguardian();

	for (int i = 0; i < 9; i++)
	{
		if (selfball.exist && armytank[i].hp>0 && selfball.collisionball(armytank[i].x, armytank[i].y, armytank[i].z, 10, 10, 10))
			selfball.exist = false;
		if (selfball.exist && enemytank[i].hp>0 && selfball.collisionball(enemytank[i].x, enemytank[i].y, enemytank[i].z, 10, 10, 10))
		{
			selfball.exist = false;
			enemytank[i].hp-=2;
		}
		armytank[i].destroytank();
		enemytank[i].destroytank();
	}


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

	armyGuardian.createguardian(0);
	enemyGuardian.createguardian(180);

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