#pragma comment(lib, "ws2_32")

#include "struct_package.h"
#include "map.h"
#include "cannonball.h"
#include "player.h"
#include "tank.h"
#include "tower.h"
#include "basetower.h"
#include "guardian.h"

#define SERVERPORT 9000

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

int TankSend(SOCKET client_sock, SOCKADDR_IN clientaddr);
int GuardianSend(SOCKET client_sock, SOCKADDR_IN clientaddr);
int TowerSend(SOCKET client_sock, SOCKADDR_IN clientaddr);
int Server_Player_recv(SOCKET client_sock, SOCKADDR_IN clientaddr, int num);
int Server_Player_send(SOCKET client_sock);

CRITICAL_SECTION cs; // 임계 영역

static int playernumber = 0;
bool viewmode = false;
GLint Ttime = 0;
Basetower armybase, enemybase;
Guardian_Data GuardianBuf;
Guardian armyGuardian, enemyGuardian;
list<Tower> armytower, enemytower;
//Player self(0);
vector<Player> playerlist, sampleplayerlist;
Tank_data tankBuf;
list<Tank> armytank, enemytank;
Ball_data ballbuf;
GLint LRcontral, UDcontral;

HANDLE  Servermath;
vector<HANDLE> Player_recv, Player_send;

void main(int argc, char *argv[])
{
	setup();
	srand((unsigned int)time(NULL));

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);
	Servermath = CreateEvent(NULL, FALSE, FALSE, NULL);

	struct timeb startimer, endtimer;
	int timer = 0, oldtimer = 0;

	ftime(&startimer);	// 타이머 시작 시간
	while (1) {
		WaitForSingleObject(Servermath, INFINITE);
		ftime(&endtimer);

		timer = endtimer.millitm - startimer.millitm;		// 걸린 시간 계산

		if (timer - oldtimer > 10) {
			Timer();
		}

		oldtimer = timer;
		if (playernumber > 0)									///플레이어가 한명 이상일 경우
			SetEvent(Player_send[0]);

	}

	//WaitForSingleObject(ServerMain, INFINITY);
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

		if (playernumber > 4)
			break;

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

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	BOOL optval = TRUE;	//네이글 알고리즘 OFF
	setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));


	InitializeCriticalSection(&cs);
	int current_playernumber = playernumber;

	retval = send(client_sock, (char*)&playernumber, sizeof(int), 0);		// 플레이어 번호 보내기

	playernumber++; //플레이어가 들어왔으니 숫자를 늘린다
	HANDLE playerevent;
	if (playernumber == 1)
		playerevent = CreateEvent(NULL, FALSE, TRUE, NULL);
	else
		playerevent = CreateEvent(NULL, FALSE, FALSE, NULL);
	Player_recv.push_back(playerevent);

	playerevent = CreateEvent(NULL, FALSE, FALSE, NULL);
	Player_send.push_back(playerevent);

	player_data buf;
	retval = recv(client_sock, (char*)&buf, sizeof(player_data), 0);

	sampleplayerlist.push_back(buf);
	playerlist.push_back(buf);
	DeleteCriticalSection(&cs);

	while (1) {
		WaitForSingleObject(Player_recv[current_playernumber], INFINITE);

		retval = Server_Player_recv(client_sock, clientaddr, current_playernumber);
		if (retval == 0)
			break;
		if (playernumber - 1 > current_playernumber)					//자신의 다음 플레이어의 이벤트를 셋, 다음 값이 있다는 전제
		{
			SetEvent(Player_recv[current_playernumber + 1]);
		}

		if (current_playernumber == playernumber - 1)			//마지막 플레이어 전송 후 연산이벤트를 셋
		{
			SetEvent(Servermath);
		}

		WaitForSingleObject(Player_send[current_playernumber], INFINITE);

		retval = TankSend(client_sock, clientaddr);

		retval = GuardianSend(client_sock, clientaddr);

		retval = TowerSend(client_sock, clientaddr);

		retval = Server_Player_send(client_sock);



		if (playernumber - 1 > current_playernumber)					//자신의 다음 플레이어의 이벤트를 셋, 다음 값이 있다는 전제
		{
			SetEvent(Player_send[current_playernumber + 1]);
		}

		if (current_playernumber == playernumber - 1)			//마지막 플레이어 전송 후 연산이벤트를 셋
		{
			SetEvent(Player_recv[0]);
		}
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

int Server_Player_recv(SOCKET client_sock, SOCKADDR_IN clientaddr, int current_playernumber)
{
	int retval;
	player_data buf;
	retval = recv(client_sock, (char*)&buf, sizeof(player_data), 0);		// 플레이어 정보 받기
	
	sampleplayerlist[current_playernumber] = buf;

	return retval;
}

int Server_Player_send(SOCKET client_sock)
{
	int retval;
	player_data *buf;
	buf = new player_data[playernumber];
	retval = send(client_sock, (char*)(&playernumber), sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}
	int i = 0;
	for (auto& d : playerlist)
		buf[i++] = d;
	retval = send(client_sock, (char*)buf, sizeof(player_data)*(playernumber), 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	return retval;
}

int TankSend(SOCKET client_sock, SOCKADDR_IN clientaddr)
{
	int retval, num;
	Tank_data* temptankbuf;

	// 아군 탱크
	num = armytank.size();	// 현재 출현중인 아군 탱크 수 측정

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// 출현중인 아군 탱크 수 전송
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	temptankbuf = new Tank_data[num];
	// 아군 탱크
	int i = 0;
	for (auto& d : armytank)
		temptankbuf[i++] = d;

	retval = send(client_sock, (char*)temptankbuf, sizeof(Tank_data) * num, 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	// 적군 탱크
	num = enemytank.size();	// 현재 출현중인 적군 탱크 수 측정

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// 출현중인 적군 탱크 수 전송
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	temptankbuf = new Tank_data[num];
	// 적군 탱크
	i = 0;
	for (auto& d : enemytank)
		temptankbuf[i++] = d;

	retval = send(client_sock, (char*)temptankbuf, sizeof(Tank_data) * num, 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	return retval;
}

int GuardianSend(SOCKET client_sock, SOCKADDR_IN clientaddr)
{

	int retval;

	GuardianBuf = armyGuardian;

	// 아군 가디언 HP
	retval = send(client_sock, (char*)&GuardianBuf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	GuardianBuf = enemyGuardian;

	// 적군 가디언
	retval = send(client_sock, (char*)&GuardianBuf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	return retval;
}

int TowerSend(SOCKET client_sock, SOCKADDR_IN clientaddr)
{
	int retval, num;
	Tower_data* tempTowerbuf;

	// 아군 타워
	num = armytower.size();	// 현재 출현중인 아군 타워 수 측정

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// 출현중인 아군 타워 수 전송
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	tempTowerbuf = new Tower_data[num];
	// 아군 타워
	int i = 0;
	for (auto& d : armytower)
		tempTowerbuf[i++] = d;

	retval = send(client_sock, (char*)tempTowerbuf, sizeof(Tower_data) * num, 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	// 적군 타워
	num = enemytower.size();	// 현재 출현중인 적군 타워 수 측정

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// 출현중인 아군 타워 수 전송
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	tempTowerbuf = new Tower_data[num];
	// 적군 타워의 포탄
	i = 0;
	for (auto& d : enemytower)
		tempTowerbuf[i++] = d;

	retval = send(client_sock, (char*)tempTowerbuf, sizeof(Tower_data) * num, 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	return retval;
}

void Timer()
{
	Ttime++;
	if (Ttime % 800 == 0)
	{
		Ttime = 0;
		for (int i = 0; i < 3; i++) {
			armytank.push_back(Tank(0, i % 3));
			enemytank.push_back(Tank(180, i % 3));
		}
	}

	for (int i = 0; i < playernumber; i++)
	{
		playerlist[i] = sampleplayerlist[i];
	}

	int i = 0;

	for (auto &p : playerlist) 
	{
		
		if (p.cannonball.exist)
			for (int y = 0; y < 3; y++)
				for (int x = 0; x < 20; x++)
					for (int z = 0; z < 50; z++)
						if (Map[x][y][z].state == 1 && p.cannonball.collisionball(Map[x][y][z].x, Map[x][y][z].y, Map[x][y][z].z, 5, 5, 5))
							p.cannonball.exist = false;

		if (p.cannonball.exist && armybase.hp > 0 && p.cannonball.collisionball(armybase.x, armybase.y, armybase.z, 10, 10, 10))
			p.cannonball.exist = false;
		if (p.cannonball.exist && enemybase.hp > 0 && p.cannonball.collisionball(enemybase.x, enemybase.y, enemybase.z, 10, 10, 10))
		{
			p.cannonball.exist = false;
			if (!enemyGuardian.exist)
				enemybase.hp -= 2;
		}

		for (auto &d : armytower)
		{
			if (p.cannonball.exist && d.hp > 0 && p.cannonball.collisionball(d.x, d.y, d.z, 10, 10, 5)) {
				p.cannonball.exist = false;
				if (i % 2 == 1)
					d.hp -= 2;
			}
		}

		for (auto &d : enemytower)
		{
			if (p.cannonball.exist && d.hp > 0 && p.cannonball.collisionball(d.x, d.y, d.z, 10, 10, 5))
			{
				p.cannonball.exist = false;
				if(i % 2 == 0)
					d.hp -= 2;
			}
		}

		if (p.cannonball.exist && armyGuardian.hp > 0 && p.cannonball.collisionball(armyGuardian.x, armyGuardian.y, armyGuardian.z, 10, 15, 5))
			p.cannonball.exist = false;
		if (p.cannonball.exist && enemyGuardian.hp > 0 && p.cannonball.collisionball(enemyGuardian.x, enemyGuardian.y, enemyGuardian.z, 10, 15, 5))
		{
			p.cannonball.exist = false;
			enemyGuardian.hp -= 2;
		}

		for (Tank& d : armytank)
		{
			if (p.cannonball.exist && d.hp > 0 && p.cannonball.collisionball(d.x, d.y, d.z, d.w, d.h, d.r))
				p.cannonball.exist = false;
		}

		for (Tank& d : enemytank)
		{
			if (p.cannonball.exist && d.hp > 0 && p.cannonball.collisionball(d.x, d.y, d.z, 10, 10, 10)) {
				p.cannonball.exist = false;
				d.hp -= 2;
			}
		}

		i++;
	}

	

	

	
	armybase.destroytower();
	enemybase.destroytower();

	for (auto &d : armytower)
	{
		d.towerattck(enemytank);
		d.destroytower();
	}
	for (auto &d : enemytower)
	{
		d.towerattck(enemytank);
		d.destroytower();
	}


	armyGuardian.destroyguardian();
	enemyGuardian.destroyguardian();

	for (Tank& d : armytank)
	{
		if (d.exist)
			d.tankmove(enemytank, enemytower, &enemyGuardian, &enemybase);
		d.destroytank();
	}

	for (Tank& d : enemytank)
	{
		if (d.exist)
			d.tankmove(armytank, armytower, &armyGuardian, &armybase);
		d.destroytank();
	}

	armytank.remove_if([](Tank object)->bool {return !object.exist; });
	enemytank.remove_if([](Tank object)->bool {return !object.exist; });

	armytower.remove_if([](Tower object)->bool {return !object.exist; });
	enemytower.remove_if([](Tower object)->bool {return !object.exist; });

	armyGuardian.guardianmove();
	enemyGuardian.guardianmove();
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
					Tower temp(10, (Map[x][0][z].x + Map[x - 1][0][z].x) / 2, Map[x][0][z].z, 180, true);
					armytower.push_back(temp);
				}
				else
				{
					Tower temp(10, (Map[x][0][z].x + Map[x - 1][0][z].x) / 2, Map[x][0][z].z, 0, true);
					enemytower.push_back(temp);
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