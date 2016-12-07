#pragma comment(lib, "ws2_32")

#include "struct_package.h"
#include "map.h"
#include "cannonball.h"
#include "player.h"
#include "tank.h"
#include "tower.h"
#include "basetower.h"
#include "guardian.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000

// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);

// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

GLvoid drawScene(GLvoid);							// 그리기
GLvoid Reshape(int w, int h);						// 화면 설정
GLvoid Keyborad(unsigned char key, int x, int y);	// 일반 키보드 입력
GLvoid SpecialKeyborad(int key, int x, int y);		// 특수키 입력
GLvoid SpecialUPKeyborad(int key, int x, int y);	// 특수키 입력(키업)
GLvoid TimerFunction(int value);					// 타이머
GLvoid setup();										// 첫 설정
template<class Object>
bool collision(Point p1, Object p2);				// 객체, 맵 충돌 체크

void Guardianrecv();
void Tankrecv();
void Towerrecv();
void Basetowerrecv();
void Client_Players_send();
void Client_Players_recv();

SOCKET sock; // 소켓
HANDLE hReadEvent, hWriteEvent; // 이벤트

bool viewmode = false;	// 뷰 전환 (1인칭시점, 전체뷰)
GLint Ttime = 0;		// 총 시간
Basetower_Data* BasetowerBuf;
Basetower armybase, enemybase;	// 아군본부, 적군본부
Guardian_Data* GuardianBuf;
Guardian armyGuardian, enemyGuardian;	// 아군가디언, 적군가디언
Tower_data* TowerBuf;
list<Tower> armytower, enemytower;
Player self;
vector <Player> playerlist;
Tank_data* TankBuf;
list<Tank> armytank, enemytank;
list<Tank>* armytank_buf, enemytank_buf;
Ball_data* ballbuf;
GLint LRcontral, UDcontral;
int playernumber;							//플레이어 번호
int totalplayernumber;						//전체 플레이어 숫자
bool onoff;
bool reset=false;

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

	BOOL optval = TRUE;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	retval = recv(sock, (char*)&playernumber, sizeof(int), 0);//플레이어 번호 받기
	printf("플레이어 번호 : %d\n", playernumber);

	self.create(playernumber);

	player_data buf;
	buf = self;
	retval = send(sock, (char*)&buf, sizeof(player_data), 0);  //플레이어 정보 제공

	// 서버와 데이터 통신
	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기
		Client_Players_send();

		Tankrecv();
		Guardianrecv();
		Towerrecv();
		Basetowerrecv();

		Client_Players_recv();

		SetEvent(hReadEvent); // 읽기 완료 알리기
	}

	return 0;
}

void Client_Players_send()
{
	int retval;
	player_data buf;

	if (onoff) {
		self.ballcreate();
		onoff = false;
	}

	buf = self;
	retval = send(sock, (char*)&buf, sizeof(player_data), 0);		// 플레이어 정보 보내기
}

void Client_Players_recv()
{

	recv(sock, (char*)&totalplayernumber, sizeof(int), 0);
	char* playerdatabuf;
	player_data* playerdata;

	playerlist.clear();
	if (totalplayernumber > 0) {
		playerdatabuf = new char[sizeof(player_data)*totalplayernumber];

		recv(sock, (char*)playerdatabuf, sizeof(player_data) * totalplayernumber, 0);

		playerdata = (player_data*)playerdatabuf;

		for (int i = 0; i < totalplayernumber; i++)
			playerlist.push_back(playerdata[i]);

		for (auto& d : playerlist) {
			if (d.id == self.id)
			{
				self.exist = d.exist;
				self.hp = d.hp;
				if (self.hp <= 0) {
					self.y = d.y;
				}
				if (self.cannonball.exist != d.cannonball.exist)
					self.cannonball.exist = d.cannonball.exist;
			}
		}
	}
}

void Tankrecv()
{
	int retval, num;
	char* Tankdatabuf;
	Tank_data* tempTankdata;

	
	// 아군 탱크
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// 현재 출현중인 아군 탱크 수 받아오기
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	armytank.clear(); // 과거 시점의 아군 탱크 정보 초기화

	if (num > 0) {
		Tankdatabuf = new char[sizeof(Tank_data) * num];
		retval = recv(sock, (char*)Tankdatabuf, sizeof(Tank_data) * num, 0);		// 현재 출현중인 아군 탱크 수 받아오기
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return;
		}
		else if (retval == 0)
			return;


		tempTankdata = (Tank_data*)Tankdatabuf;
		for (int i = 0; i < num; i++)
			armytank.push_back(&tempTankdata[i]);
	}
	// 적군 탱크
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// 현재 출현중인 적군 탱크 수 받아오기
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	enemytank.clear(); // 과거 시점의 적군 탱크 정보 초기화

	if (num > 0) {
		Tankdatabuf = new char[sizeof(Tank_data) * num];
		retval = recv(sock, (char*)Tankdatabuf, sizeof(Tank_data) * num, 0);		// 현재 출현중인 아군 탱크 수 받아오기
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return;
		}
		else if (retval == 0)
			return;


		tempTankdata = (Tank_data*)Tankdatabuf;
		for (int i = 0; i < num; i++)
			enemytank.push_back(&tempTankdata[i]);
	}
}

void Towerrecv()
{
	int retval, num;
	char* Towerdatabuf;
	Tower_data* tempTowerdata;

	// 아군 타워
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// 현재 출현중인 아군 타워 수 받아오기
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	armytower.clear(); // 과거 시점의 아군 타워 정보 초기화

	if (num > 0) {
		Towerdatabuf = new char[sizeof(Tower_data) * num];
		retval = recv(sock, (char*)Towerdatabuf, sizeof(Tower_data) * num, 0);		// 현재 출현중인 아군 타워
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return;
		}
		else if (retval == 0)
			return;


		tempTowerdata = (Tower_data*)Towerdatabuf;
		for (int i = 0; i < num; i++)
			armytower.push_back(&tempTowerdata[i]);
	}
	// 적군 타워
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// 현재 출현중인 적군 타워 수 받아오기
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	enemytower.clear(); // 과거 시점의 적군 탱크 정보 초기화

	if (num > 0) {
		Towerdatabuf = new char[sizeof(Tower_data) * num];
		retval = recv(sock, (char*)Towerdatabuf, sizeof(Tower_data) * num, 0);		// 현재 출현중인 적군 타워
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return;
		}
		else if (retval == 0)
			return;


		tempTowerdata = (Tower_data*)Towerdatabuf;
		for (int i = 0; i < num; i++)
			enemytower.push_back(&tempTowerdata[i]);
	}
}

void Guardianrecv()
{
	int retval;
	char* buf[sizeof(Guardian_Data)];

	// 아군 가디언
	retval = recv(sock, (char*)buf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	GuardianBuf = (Guardian_Data*)&buf;

	armyGuardian = *GuardianBuf;

	// 적군 가디언
	retval = recv(sock, (char*)buf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	GuardianBuf = (Guardian_Data*)&buf;

	enemyGuardian = *GuardianBuf;

}

void Basetowerrecv()
{
	int retval;
	char* buf[sizeof(Basetower_Data)];

	// 아군 본진
	retval = recv(sock, (char*)buf, sizeof(Basetower_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	BasetowerBuf = (Basetower_Data*)&buf;

	armybase = *BasetowerBuf;

	// 적군 본진
	retval = recv(sock, (char*)buf, sizeof(Basetower_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	BasetowerBuf = (Basetower_Data*)&buf;

	enemybase = *BasetowerBuf;

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
		glTranslated(-self.x, -(self.y+5), -self.z);
		gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
	}

	glEnable(GL_DEPTH_TEST);		//은면제거

	glPushMatrix();
	ground();

	armybase.ranberbasetower();
	enemybase.ranberbasetower();

	for (auto& d : armytower)
	{
		d.ranbertower();
		d.cannonball.ranberCannonball();
	}
	for (auto& d : enemytower)
	{
		d.ranbertower();
		d.cannonball.ranberCannonball();
	}
	
	glPopMatrix();

	glPushMatrix();
	if (viewmode)
		for (auto& d : playerlist)
			d.ranbertank(true);
	else
	{
		auto& d = playerlist.begin();
		while (d != playerlist.end())
		{
			if (d->id == self.id)
				self.ranbertank(false);
			else
				d->ranbertank(true);
			d++;
		}
	}

	for (auto &d : playerlist) {
		if(d.cannonball.exist)
			d.cannonball.ranberCannonball();
	}

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
		self.create(playernumber);
	}
	if (key == 'm')
		viewmode = (viewmode + 1) % 2;

	if (key == 32 && self.cannonball.exist == false && self.cannonball.delaytime == 0 && !onoff && self.y >= 0)
	{
		onoff = true;
	}
	if (key == 'r')
		reset = true;

	glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
	glutPostRedisplay();
}

GLvoid SpecialKeyborad(int key, int x, int y)
{
	if (self.hp > 0) {
		if (key == GLUT_KEY_UP)
			UDcontral = 1;
		if (key == GLUT_KEY_DOWN)
			UDcontral = 2;
		if (key == GLUT_KEY_LEFT)
			LRcontral = 1;
		if (key == GLUT_KEY_RIGHT)
			LRcontral = 2;
	}
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
	if (self.hp > 0) {
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
						if (((Map[x][y][z].state != 2 && Map[x][y][z].state != 4 && Map[x][y][z].state != 3 && Map[x][y][z].state) || (Map[x][y][z].state == 2 && (z < 5 || z > 45))) && collision(Map[x][y][z], self))
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
						if (((Map[x][y][z].state != 2 && Map[x][y][z].state != 4 && Map[x][y][z].state) || (Map[x][y][z].state == 2 && (z < 5 || z > 45))) && collision(Map[x][y][z], self))
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

		if (self.y > 9)
			self.y -= 10;
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 20; x++)
				for (int z = 0; z < 50; z++)
					if (Map[x][y][z].state == 3 && collision(Map[x][y][z], self))
						self.y += 10;

		if (self.cannonball.exist || self.cannonball.delaytime)
			self.cannonball.Cannonball_timer(0.2);
	}
	if (!self.exist)
		self.create(playernumber);

	glutPostRedisplay();
	glutTimerFunc(1, TimerFunction, 1);
}

GLvoid setup()
{
	mapload();

	armybase.setup(20, 100, -20, 180, true);
	enemybase.setup(20, 100, -480, 0, true);

	for (int x = 0; x < 20; x++)
	{
		for (int z = 5; z < 45; z++)
		{
			if (Map[x][0][z].state == 2 && Map[x - 1][0][z].state == 2)
			{
				if (z < 25)
				{
					Tower temp;
					armytower.push_back(temp);
				}
				else
				{
					Tower temp;
					enemytower.push_back(temp);
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