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

// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);

// ���� ��� ������ �Լ�
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);
void Timer();

GLvoid setup();
template<class Object>
bool collision(Point p1, Object p2);

int TankSend(SOCKET client_sock, SOCKADDR_IN clientaddr);
int GuardianSend(SOCKET client_sock, SOCKADDR_IN clientaddr);
int TowerSend(SOCKET client_sock, SOCKADDR_IN clientaddr);
int BasetowerSend(SOCKET client_sock, SOCKADDR_IN clientaddr);

int Server_Player_recv(SOCKET client_sock, SOCKADDR_IN clientaddr, int num);
int Server_Player_send(SOCKET client_sock);

CRITICAL_SECTION cs; // �Ӱ� ����

static int playernumber = 0;
bool viewmode = false;
GLint Ttime = 0;
Basetower_data BasetowerBuf;
Basetower armybase, enemybase;
Guardian_Data GuardianBuf;
Guardian armyGuardian, enemyGuardian;
list<Tower> armytower, enemytower;
vector<Player> playerlist;
Tank_data tankBuf;
list<Tank> armytank, enemytank;
Ball_data ballbuf;
GLint LRcontral, UDcontral;

HANDLE  Servermath;
vector<HANDLE*> Player_recv, Player_send;

void main(int argc, char *argv[])
{
	setup();
	srand((unsigned int)time(NULL));

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);
	Servermath = CreateEvent(NULL, FALSE, FALSE, NULL);

	struct timeb startimer, endtimer;
	int timer = 0, oldtimer = 0;

	ftime(&startimer);	// Ÿ�̸� ���� �ð�
	while (1) {
		WaitForSingleObject(Servermath, INFINITE);
		ftime(&endtimer);

		timer = (endtimer.time*1000+endtimer.millitm) - (startimer.time*1000+startimer.millitm);		// �ɸ� �ð� ���

		if (timer - oldtimer > 10) {
			Timer();

			oldtimer = timer;

			if (Player_send.size() > 0)									///�÷��̾ �Ѹ� �̻��� ���
				SetEvent(*Player_send[0]);
		}
		else
			continue;

	}

	// ���� ����
	WSACleanup();
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

// TCP ���� ���� �κ�
DWORD WINAPI ServerMain(LPVOID arg)
{
	printf("���� ������ ���� �Ϸ� \n");
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\r\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		if (playernumber > 4)
			break;

		// ������ ����
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	printf("�ִ� Ŭ���̾�Ʈ ���� �Ϸ�\n");
	// closesocket()
	closesocket(listen_sock);

	return 0;
}


// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE playerevent_recv, playerevent_send;

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	BOOL optval = TRUE;	//���̱� �˰��� OFF
	setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));


	InitializeCriticalSection(&cs);
	int current_playernumber = playernumber;

	retval = send(client_sock, (char*)&current_playernumber, sizeof(int), 0);		// �÷��̾� ��ȣ ������

	playernumber++; //�÷��̾ �������� ���ڸ� �ø���

	if (playernumber == 1)
		playerevent_recv = CreateEvent(NULL, FALSE, TRUE, NULL);
	else
		playerevent_recv = CreateEvent(NULL, FALSE, FALSE, NULL);
	Player_recv.push_back(&playerevent_recv);

	playerevent_send = CreateEvent(NULL, FALSE, FALSE, NULL);
	Player_send.push_back(&playerevent_send);

	player_data buf;
	retval = recv(client_sock, (char*)&buf, sizeof(player_data), 0);		// ���� �÷��̾� ���� ����

	playerlist.push_back(buf);
	DeleteCriticalSection(&cs);

	while (1) {
		WaitForSingleObject(playerevent_recv, INFINITE);

		printf("%d ��° �÷��̾� ���� �ޱ� \n", current_playernumber);
		retval = Server_Player_recv(client_sock, clientaddr, current_playernumber);

		// �ڽ��� recv �̺�Ʈ ��ġ ã��
		auto d = find(Player_recv.begin(), Player_recv.end(), &playerevent_recv);

		// ���� recv �̺�Ʈ�� �ִ��� �˻� �� ������ ����
		while (++d != Player_recv.end()) {
			SetEvent(**d);
			break;
		}

		// �̺�Ʈ ����� ������ ���ٸ�, ���� �̺�Ʈ ����
		if(d == Player_recv.end())
			SetEvent(Servermath);

		if (retval == 0 || retval == SOCKET_ERROR)
			break;

		WaitForSingleObject(playerevent_send, INFINITE);

		printf("%d ��° �÷��̾� ���� ������ \n", current_playernumber);

		TankSend(client_sock, clientaddr);
		GuardianSend(client_sock, clientaddr);
		TowerSend(client_sock, clientaddr);
		BasetowerSend(client_sock, clientaddr);

		Server_Player_send(client_sock);

		// �ڽ��� recv �̺�Ʈ ��ġ ã��
		d = find(Player_send.begin(), Player_send.end(), &playerevent_send);

		// ���� recv �̺�Ʈ�� �ִ��� �˻� �� ������ ����
		while (++d != Player_send.end()) {
			SetEvent(**d);
			break;
		}

		// �̺�Ʈ ����� ������ ���ٸ�, ���� �̺�Ʈ ����
		if (d == Player_send.end())
			SetEvent(*Player_recv[0]);
	}

	// �������� Ŭ���� �÷��̾� ���� �˻� �� ���� �÷��̾� ����Ʈ���� ����
	playerlist.erase(remove_if(playerlist.begin(), playerlist.end(), [&](Player A)->bool {return A.id == current_playernumber; }), playerlist.end());

	// �������� Ŭ��� ������ �̺�Ʈ�� ����Ʈ���� ����
	Player_recv.erase(remove(Player_recv.begin(), Player_recv.end(), playerevent_recv), Player_recv.end());
	Player_send.erase(remove(Player_send.begin(), Player_send.end(), playerevent_send), Player_send.end());

	// closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

int Server_Player_recv(SOCKET client_sock, SOCKADDR_IN clientaddr, int current_playernumber)
{
	int retval;
	player_data buf;
	retval = recv(client_sock, (char*)&buf, sizeof(player_data), 0);		// �÷��̾� ���� �ޱ�
	
	playerlist[current_playernumber] = buf;

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
	int i = 0;
	Tank_data* temptankbuf;

	// �Ʊ� ��ũ
	num = armytank.size();	// ���� �������� �Ʊ� ��ũ �� ����

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// �������� �Ʊ� ��ũ �� ����
	if (retval == SOCKET_ERROR) {
		return retval;
	}


	if (num > 0) {
		temptankbuf = new Tank_data[num];
		// �Ʊ� ��ũ

		for (auto& d : armytank)
			temptankbuf[i++] = d;

		retval = send(client_sock, (char*)temptankbuf, sizeof(Tank_data) * num, 0);
		if (retval == SOCKET_ERROR) {
			return retval;
		}
	}

	// ���� ��ũ
	num = enemytank.size();	// ���� �������� ���� ��ũ �� ����

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// �������� ���� ��ũ �� ����
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	if (num > 0) {
		temptankbuf = new Tank_data[num];
		// ���� ��ũ
		i = 0;
		for (auto& d : enemytank)
			temptankbuf[i++] = d;

		retval = send(client_sock, (char*)temptankbuf, sizeof(Tank_data) * num, 0);
		if (retval == SOCKET_ERROR) {
			return retval;
		}
	}
	return retval;
}

int GuardianSend(SOCKET client_sock, SOCKADDR_IN clientaddr)
{

	int retval;

	GuardianBuf = armyGuardian;

	// �Ʊ� ����� HP
	retval = send(client_sock, (char*)&GuardianBuf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	GuardianBuf = enemyGuardian;

	// ���� �����
	retval = send(client_sock, (char*)&GuardianBuf, sizeof(Guardian_Data), 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	return retval;
}

int TowerSend(SOCKET client_sock, SOCKADDR_IN clientaddr)
{
	int retval, num;
	int i;
	Tower_data* tempTowerbuf;

	// �Ʊ� Ÿ��
	num = armytower.size();	// ���� �������� �Ʊ� Ÿ�� �� ����

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// �������� �Ʊ� Ÿ�� �� ����
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	if (num > 0) {
		tempTowerbuf = new Tower_data[num];
		// �Ʊ� Ÿ��
		i = 0;
		for (auto& d : armytower)
			tempTowerbuf[i++] = d;

		retval = send(client_sock, (char*)tempTowerbuf, sizeof(Tower_data) * num, 0);
		if (retval == SOCKET_ERROR) {
			return retval;
		}
	}

	// ���� Ÿ��
	num = enemytower.size();	// ���� �������� ���� Ÿ�� �� ����

	retval = send(client_sock, (char*)&num, sizeof(int), 0);		// �������� ���� Ÿ�� �� ����
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	if (num > 0) {
		tempTowerbuf = new Tower_data[num];
		// ���� Ÿ��
		i = 0;
		for (auto& d : enemytower)
			tempTowerbuf[i++] = d;

		retval = send(client_sock, (char*)tempTowerbuf, sizeof(Tower_data) * num, 0);
		if (retval == SOCKET_ERROR) {
			return retval;
		}
	}
	return retval;
}

int BasetowerSend(SOCKET client_sock, SOCKADDR_IN clientaddr)
{

	int retval;

	BasetowerBuf = armybase;

	// �Ʊ� ����
	retval = send(client_sock, (char*)&BasetowerBuf, sizeof(Basetower_data), 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	BasetowerBuf = enemybase;

	// ���� ����
	retval = send(client_sock, (char*)&BasetowerBuf, sizeof(Basetower_data), 0);
	if (retval == SOCKET_ERROR) {
		return retval;
	}

	return retval;
}

void Timer()
{
	printf("���� ���� \n");
	Ttime++;
	if (Ttime % 800 == 0)
	{
		Ttime = 0;
		for (int i = 0; i < 3; i++) {
			armytank.push_back(Tank(0, i % 3));
			enemytank.push_back(Tank(180, i % 3));
		}
	}

	for (auto &p : playerlist) 
	{
		
		if (p.cannonball.exist)
			for (int y = 0; y < 3; y++)
				for (int x = 0; x < 20; x++)
					for (int z = 0; z < 50; z++)
						if (Map[x][y][z].state == 1 && p.cannonball.collisionball(Map[x][y][z].x, Map[x][y][z].y, Map[x][y][z].z, 5, 5, 5))
							p.cannonball.exist = false;

		if (p.cannonball.exist && armybase.hp > 0 && p.cannonball.collisionball(armybase.x, armybase.y, armybase.z, 10, 10, 10))
		{
			p.cannonball.exist = false;
			if (!armyGuardian.exist && p.id%2 == 1)
				armybase.hp -= 2;
		}
		if (p.cannonball.exist && enemybase.hp > 0 && p.cannonball.collisionball(enemybase.x, enemybase.y, enemybase.z, 10, 10, 10))
		{
			p.cannonball.exist = false;
			if (!enemyGuardian.exist && p.id%2 == 0)
				enemybase.hp -= 2;
		}

		for (auto &d : armytower)
		{
			if (p.cannonball.exist && d.hp > 0 && p.cannonball.collisionball(d.x, d.y, d.z, 10, 10, 5)) {
				p.cannonball.exist = false;
				if (p.id % 2 == 1)
					d.hp -= 2;
			}
		}

		for (auto &d : enemytower)
		{
			if (p.cannonball.exist && d.hp > 0 && p.cannonball.collisionball(d.x, d.y, d.z, 10, 10, 5))
			{
				p.cannonball.exist = false;
				if(p.id % 2 == 0)
					d.hp -= 2;
			}
		}

		if (p.cannonball.exist && armyGuardian.hp > 0 && p.cannonball.collisionball(armyGuardian.x, armyGuardian.y, armyGuardian.z, 10, 15, 5))
		{
			p.cannonball.exist = false;
			if (p.id % 2 == 1)
				armyGuardian.hp -= 2;
		}
		if (p.cannonball.exist && enemyGuardian.hp > 0 && p.cannonball.collisionball(enemyGuardian.x, enemyGuardian.y, enemyGuardian.z, 10, 15, 5))
		{
			p.cannonball.exist = false;
			if(p.id % 2 == 0)
				enemyGuardian.hp -= 2;
		}

		for (Tank& d : armytank)
		{
			if (p.cannonball.exist && d.hp > 0 && p.cannonball.collisionball(d.x, d.y, d.z, d.w, d.h, d.r)) {
				p.cannonball.exist = false;
				if (p.id % 2 == 1)
					d.hp -= 2;
			}
		}

		for (Tank& d : enemytank)
		{
			if (p.cannonball.exist && d.hp > 0 && p.cannonball.collisionball(d.x, d.y, d.z, 10, 10, 10)) {
				p.cannonball.exist = false;
				if(p.id % 2 == 0)
					d.hp -= 2;
			}
		}

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
					Tower temp(5, (Map[x][0][z].x + Map[x - 1][0][z].x) / 2, Map[x][0][z].z, 180, true);
					armytower.push_back(temp);
				}
				else
				{
					Tower temp(5, (Map[x][0][z].x + Map[x - 1][0][z].x) / 2, Map[x][0][z].z, 0, true);
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