#pragma comment(lib, "ws2_32")

#include "struct_package.h"
#include "map.h"
#include "cannonball.h"
#include "tank.h"
#include "tower.h"
#include "basetower.h"
#include "guardian.h"

#define SERVERPORT 8900

// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);

// 소켓 통신 스레드 함수
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);
void Timer();

GLvoid setup();
template<class Object>
bool collision(Point p1, Object p2);

void TankSned(SOCKET client_sock, SOCKADDR_IN clientaddr);
void GuardianSend(SOCKET client_sock, SOCKADDR_IN clientaddr);

CRITICAL_SECTION cs; // 임계 영역

bool viewmode = false;
GLint Ttime = 0;
Basetower armybase, enemybase;
Guardian_Data GuardianBuf;
Guardian armyGuardian, enemyGuardian;
Tower armytower[6], enemytower[6];
Player self(0);
Tank_data tankBuf;
list<Tank> armytank, enemytank;
GLint LRcontral, UDcontral;
Ball selfball;

void main(int argc, char *argv[])
{
	setup();
	srand((unsigned int)time(NULL));

	InitializeCriticalSection(&cs);

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	struct timeb startimer, endtimer;
	int timer = 0, oldtimer = 0;


	ftime(&startimer);	// 타이머 시작 시간
	while (1) {

		ftime(&endtimer);

		timer = endtimer.millitm - startimer.millitm;		// 걸린 시간 계산

		if (timer != oldtimer) {
			if (timer % 1 == 0)
				Timer();
		}

		oldtimer = timer;
	}
	DeleteCriticalSection(&cs);

	WaitForSingleObject(ServerMain, INFINITY);
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

// TCP 서버 시작 부분
DWORD WINAPI ServerMain(LPVOID arg)
{
	printf("서버 쓰레드 생성 완료 \n");
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\r\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}


// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	FILE* f;

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while (1) {
		InitializeCriticalSection(&cs);

		TankSned(client_sock, clientaddr);
		GuardianSend(client_sock, clientaddr);

		system("cls");

		DeleteCriticalSection(&cs);
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

void TankSned(SOCKET client_sock, SOCKADDR_IN clientaddr)
{
	int retval, num;
	char magbuf[28];

	// 아군 탱크
	num = armytank.size();	// 현재 출현중인 아군 탱크 수 측정

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// 출현중인 아군 탱크 수 전송
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	for (auto& d : armytank) {		// 출현중인 아군 탱크 수 만큼 정보 보내기

		tankBuf = d;

		// 아군 탱크
		retval = send(client_sock, (char*)&tankBuf, sizeof(Tank_data), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		// 보낸 데이터 출력
//		printf("[TCP/%s:%d] armyGuardian의 hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d \n", inet_ntoa(clientaddr.sin_addr),
//			ntohs(clientaddr.sin_port), tankBuf.hp, tankBuf.x, tankBuf.y, tankBuf.z, tankBuf.angle, tankBuf.wave);
	}
	// 아군 탱크 수신 완료 메세지 받기
	retval = recv(client_sock, (char*)magbuf, sizeof(char) * 28, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	printf("%s", magbuf);

	// 적군 탱크
	num = enemytank.size();	// 현재 출현중인 적군 탱크 수 측정

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// 출현중인 적군 탱크 수 전송
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	for (auto& d : enemytank) {		// 출현중인 적군 탱크 수 만큼 정보 보내기

		tankBuf = d;

		// 적군 탱크
		retval = send(client_sock, (char*)&tankBuf, sizeof(Tank_data), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		// 보낸 데이터 출력
//		printf("[TCP/%s:%d] armyGuardian의 hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d \n", inet_ntoa(clientaddr.sin_addr),
//			ntohs(clientaddr.sin_port), tankBuf.hp, tankBuf.x, tankBuf.y, tankBuf.z, tankBuf.angle, tankBuf.wave);
	}
	// 적군 탱크 수신 완료 메세지 받기
	retval = recv(client_sock, (char*)magbuf, sizeof(char) * 28, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	printf("%s", magbuf);
}

void GuardianSend(SOCKET client_sock, SOCKADDR_IN clientaddr)
{

	int retval;

	GuardianBuf = armyGuardian;

	// 아군 가디언 HP
	retval = send(client_sock, (char*)&GuardianBuf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	// 보낸 데이터 출력
	printf("[TCP/%s:%d] armyGuardian의 hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d, Rangle : %.2f, Langle : %.2f \n", inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port), armyGuardian.hp, armyGuardian.x, armyGuardian.y, armyGuardian.z, armyGuardian.angle, armyGuardian.Rangle, armyGuardian.Langle);

	// 아군 가디언 수신 완료 메세지 받기
	char tempbuf[23];
	retval = recv(client_sock, (char*)tempbuf, sizeof(char) * 23, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	printf("%s", tempbuf);

	GuardianBuf = enemyGuardian;

	// 적군 가디언
	retval = send(client_sock, (char*)&GuardianBuf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	// 보낸 데이터 출력
	printf("[TCP/%s:%d] enemyGuardian의 hp : %d, x : %.2f, y : %.2f, z : %.2f, angle : %d, Rangle : %.2f, Langle : %.2f \n", inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port), enemyGuardian.hp, enemyGuardian.x, enemyGuardian.y, enemyGuardian.z, enemyGuardian.angle, enemyGuardian.Rangle, enemyGuardian.Langle);

	// 적군 가디언 수신 완료 메세지 받기
	retval = recv(client_sock, (char*)tempbuf, sizeof(char) * 23, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	printf("%s", tempbuf);
}
void Timer()
{
	InitializeCriticalSection(&cs);
	Ttime++;
	if (Ttime % 800 == 0)
	{
		for (int i = 0; i < 3; i++) {
			armytank.push_back(Tank(0, i % 3));
			enemytank.push_back(Tank(180, i % 3));
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

	if (self.y > 5)
		self.y -= 10;
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

	if (selfball.exist && armybase.hp > 0 && selfball.collisionball(armybase.x, armybase.y, armybase.z, 10, 10, 10))
		selfball.exist = false;
	if (selfball.exist && armybase.hp > 0 && selfball.collisionball(enemybase.x, enemybase.y, enemybase.z, 10, 10, 10))
	{
		selfball.exist = false;
		if (!enemyGuardian.exist)
			enemybase.hp -= 2;
	}
	armybase.destroytower();
	enemybase.destroytower();

	for (int i = 0; i < 6; i++)
	{
		if (selfball.exist && armytower[i].hp > 0 && selfball.collisionball(armytower[i].x, armytower[i].y, armytower[i].z, 10, 10, 5))
			selfball.exist = false;
		if (selfball.exist && enemytower[i].hp > 0 && selfball.collisionball(enemytower[i].x, enemytower[i].y, enemytower[i].z, 10, 10, 5))
		{
			selfball.exist = false;
			enemytower[i].hp -= 2;
		}
		armytower[i].towerattck(enemytank);
		enemytower[i].towerattck(armytank);
		armytower[i].destroytower();
		enemytower[i].destroytower();
	}

	if (selfball.exist && armyGuardian.hp > 0 && selfball.collisionball(armyGuardian.x, armyGuardian.y, armyGuardian.z, 10, 15, 5))
		selfball.exist = false;
	if (selfball.exist && enemyGuardian.hp > 0 && selfball.collisionball(enemyGuardian.x, enemyGuardian.y, enemyGuardian.z, 10, 15, 5))
	{
		selfball.exist = false;
		enemyGuardian.hp -= 2;
	}
	armyGuardian.destroyguardian();
	enemyGuardian.destroyguardian();

	for (Tank& d : armytank)
	{
		if (d.exist)
			d.tankmove(enemytank, enemytower, &enemyGuardian, &enemybase);
		if (selfball.exist && d.hp > 0 && selfball.collisionball(d.x, d.y, d.z, d.w, d.h, d.r))
			selfball.exist = false;
		d.destroytank();
	}

	for (Tank& d : enemytank)
	{
		if (d.exist)
			d.tankmove(armytank, armytower, &armyGuardian, &armybase);
		if (selfball.exist && d.hp > 0 && selfball.collisionball(d.x, d.y, d.z, 10, 10, 10))
			selfball.exist = false;
		d.destroytank();
	}
	armytank.remove_if([](Tank object)->bool {return !object.exist; });
	enemytank.remove_if([](Tank object)->bool {return !object.exist; });

	armyGuardian.guardianmove();
	enemyGuardian.guardianmove();

	DeleteCriticalSection(&cs);
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
						(Map[x][0][z].x + Map[x - 1][0][z].x) / 2, Map[x][0][z].z, \
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

	for (int i = 0; i < 3; i++) {
		armytank.push_back(Tank(0, i));
		enemytank.push_back(Tank(180, i));
	}
}

template<class Object>
bool collision(Point p1, Object p2)
{
	if (p2.x + 3 >= p1.x - 5 && p2.x - 3 <= p1.x + 5 && p2.z + 3 >= p1.z - 5 && p2.z - 3 <= p1.z + 5 && p2.y >= p1.y - 5 && p2.y <= p1.y + 5)
		return true;
	return false;
}