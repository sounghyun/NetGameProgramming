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

// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);

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
void Towerrecv();
void Basetowerrecv();
void Client_Players_send();
void Client_Players_recv();

SOCKET sock; // ����
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ

bool viewmode = false;	// �� ��ȯ (1��Ī����, ��ü��)
GLint Ttime = 0;		// �� �ð�
Basetower_Data* BasetowerBuf;
Basetower armybase, enemybase;	// �Ʊ�����, ��������
Guardian_Data* GuardianBuf;
Guardian armyGuardian, enemyGuardian;	// �Ʊ������, ���������
Tower_data* TowerBuf;
list<Tower> armytower, enemytower;
Player self;
vector <Player> playerlist;
Tank_data* TankBuf;
list<Tank> armytank, enemytank;
list<Tank>* armytank_buf, enemytank_buf;
Ball_data* ballbuf;
GLint LRcontral, UDcontral;
int playernumber;							//�÷��̾� ��ȣ
int totalplayernumber;						//��ü �÷��̾� ����
bool onoff;
bool reset=false;

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

	retval = recv(sock, (char*)&playernumber, sizeof(int), 0);//�÷��̾� ��ȣ �ޱ�
	printf("�÷��̾� ��ȣ : %d\n", playernumber);

	self.create(playernumber);

	player_data buf;
	buf = self;
	retval = send(sock, (char*)&buf, sizeof(player_data), 0);  //�÷��̾� ���� ����

	// ������ ������ ���
	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ��ٸ���
		Client_Players_send();

		Tankrecv();
		Guardianrecv();
		Towerrecv();
		Basetowerrecv();

		Client_Players_recv();

		SetEvent(hReadEvent); // �б� �Ϸ� �˸���
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
	retval = send(sock, (char*)&buf, sizeof(player_data), 0);		// �÷��̾� ���� ������
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

	
	// �Ʊ� ��ũ
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// ���� �������� �Ʊ� ��ũ �� �޾ƿ���
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	armytank.clear(); // ���� ������ �Ʊ� ��ũ ���� �ʱ�ȭ

	if (num > 0) {
		Tankdatabuf = new char[sizeof(Tank_data) * num];
		retval = recv(sock, (char*)Tankdatabuf, sizeof(Tank_data) * num, 0);		// ���� �������� �Ʊ� ��ũ �� �޾ƿ���
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
	// ���� ��ũ
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// ���� �������� ���� ��ũ �� �޾ƿ���
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	enemytank.clear(); // ���� ������ ���� ��ũ ���� �ʱ�ȭ

	if (num > 0) {
		Tankdatabuf = new char[sizeof(Tank_data) * num];
		retval = recv(sock, (char*)Tankdatabuf, sizeof(Tank_data) * num, 0);		// ���� �������� �Ʊ� ��ũ �� �޾ƿ���
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

	// �Ʊ� Ÿ��
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// ���� �������� �Ʊ� Ÿ�� �� �޾ƿ���
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	armytower.clear(); // ���� ������ �Ʊ� Ÿ�� ���� �ʱ�ȭ

	if (num > 0) {
		Towerdatabuf = new char[sizeof(Tower_data) * num];
		retval = recv(sock, (char*)Towerdatabuf, sizeof(Tower_data) * num, 0);		// ���� �������� �Ʊ� Ÿ��
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
	// ���� Ÿ��
	retval = recv(sock, (char*)&num, sizeof(int), 0);		// ���� �������� ���� Ÿ�� �� �޾ƿ���
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	enemytower.clear(); // ���� ������ ���� ��ũ ���� �ʱ�ȭ

	if (num > 0) {
		Towerdatabuf = new char[sizeof(Tower_data) * num];
		retval = recv(sock, (char*)Towerdatabuf, sizeof(Tower_data) * num, 0);		// ���� �������� ���� Ÿ��
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

}

void Basetowerrecv()
{
	int retval;
	char* buf[sizeof(Basetower_Data)];

	// �Ʊ� ����
	retval = recv(sock, (char*)buf, sizeof(Basetower_Data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return;
	}
	else if (retval == 0)
		return;

	BasetowerBuf = (Basetower_Data*)&buf;

	armybase = *BasetowerBuf;

	// ���� ����
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
		glTranslated(-self.x, -(self.y+5), -self.z);
		gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
	}

	glEnable(GL_DEPTH_TEST);		//��������

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