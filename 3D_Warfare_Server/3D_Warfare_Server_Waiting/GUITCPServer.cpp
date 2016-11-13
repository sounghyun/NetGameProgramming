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

HINSTANCE hInst; // 인스턴스 핸들
HWND hEdit1, hList1, hList2; // 편집 컨트롤
CRITICAL_SECTION cs; // 임계 영역

Data_Player UserData;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{	
	hInst = hInstance;
	InitializeCriticalSection(&cs);

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// 대화상자 생성
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// 메시지 루프
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteCriticalSection(&cs);
	
	return msg.wParam;
}

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		hList1 = GetDlgItem(hDlg, IDC_LIST1);
		hList2 = GetDlgItem(hDlg, IDC_LIST3);
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;
	
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;

}

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	
	char cbuf[BUFSIZE+256];
	vsprintf(cbuf, fmt, arg);
	
	EnterCriticalSection(&cs);
	int nLength = GetWindowTextLength(hEdit1);
	SendMessage(hEdit1, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit1, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	LeaveCriticalSection(&cs);
	
	va_end(arg);
}

// 플레이어 리스트 목록 추가 함수
void DisplayPlayerAdd(char *fmt, ...)
{
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
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
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
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
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread, hThreadTime;

	hThreadTime = CreateThread(NULL, 0, ProcessTime,
		NULL, 0, NULL);

	while(1){

		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		DisplayText("\r\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		SendMessage(hList1, LB_ADDSTRING, 0, (LPARAM)inet_ntoa(clientaddr.sin_addr));

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)client_sock, 0, NULL);
		if(hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }

	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

DWORD WINAPI ProcessTime(LPVOID arg)
{
	//대기 시간 관련 변수
	time_t startimer, timer, oldTimer = 0;

	time(&startimer);	// 타이머 시작 시간

	if (UserData.ready) {
		EnterCriticalSection(&cs);
		while (1)
		{
			timer = time(NULL) - startimer;		// 걸린 시간 계산

			if (timer != oldTimer)
				DisplayText("\r\n 게임 시작까지 %d초가 남았습니다.\r", 10 - timer);

			oldTimer = timer;

			if (timer >= 10) {
				DisplayText("\r\n 게임이 시작됩니다.\r");
				Sleep(1000);
				exit(NULL);
			}
		}
		LeaveCriticalSection(&cs);
	}

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

	while(1){
		UserData.clientaddr = clientaddr;

		if(UserData.ready)
			SendMessage(hList2, LB_INSERTSTRING, UserData.id-1, (LPARAM)TEXT("READY"));
		else
			SendMessage(hList2, LB_INSERTSTRING, UserData.id - 1, (LPARAM)TEXT(""));

		// 파일 이름 받기
		retval = recvn(client_sock, (char*)&UserData.id, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;


		// 파일 데이터의 크기정보 받기(고정 길이)
		retval = recvn(client_sock, (char *)&UserData.ready, sizeof(BOOL), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		if (UserData.ready) {
			DisplayText("[TCP/%s:%d] %d 플레이어의 준비상태는 True 입니다.\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), UserData.id, UserData.ready);
		}
		else {
			DisplayText("[TCP/%s:%d] %d 플레이어의 준비상태는 False 입니다.\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), UserData.id, UserData.ready);
		}

		SendMessage(hList2, LB_DELETESTRING, UserData.id-1, 0);
	}

	// closesocket()
	closesocket(client_sock);
	DisplayText("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}