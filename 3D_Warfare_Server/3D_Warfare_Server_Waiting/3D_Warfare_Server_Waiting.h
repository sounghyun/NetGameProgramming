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
	SOCKADDR_IN clientaddr;	// 플레이어 접속 정보(ip, 포트번호)
	int id;				// 플레이어 id
	bool ready;			// 플레이어 준비상태
};

// 윈도우 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 플레이어 리스트 목록 추가 함수
void DisplayPlayerAdd(char *fmt, ...);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 소켓 통신 스레드 함수
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);
DWORD WINAPI ProcessTime(LPVOID arg);


#endif // !WAITING_H