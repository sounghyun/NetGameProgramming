#ifndef WAITING_H
#define WAITING_H

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <list>
#include "resource.h"

#define SERVERPORT 9000
#define BUFSIZE    512

struct Data_Player {
	SOCKADDR_IN clientaddr;	// �÷��̾� ���� ����(ip, ��Ʈ��ȣ)
	int id;				// �÷��̾� id
	bool ready;			// �÷��̾� �غ����
};

// ������ ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...);
// �÷��̾� ����Ʈ ��� �߰� �Լ�
void DisplayPlayerAdd(char *fmt, ...);
// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);
// ���� ��� ������ �Լ�
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);
DWORD WINAPI ProcessTime(LPVOID arg);


#endif // !WAITING_H