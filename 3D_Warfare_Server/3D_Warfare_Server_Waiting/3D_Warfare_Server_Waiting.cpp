#include<time.h>
#include "3D_Warfare_Server_Waiting.h"

HINSTANCE hInst; // �ν��Ͻ� �ڵ�
HWND hEdit1, hList1, hList2; // ���� ��Ʈ��
CRITICAL_SECTION cs; // �Ӱ� ����
HANDLE hReadyEvent, hConnetEvent; // �̺�Ʈ

Data_Player UserData;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{	
	hInst = hInstance;
	InitializeCriticalSection(&cs);

	// �̺�Ʈ ����
	hReadyEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadyEvent == NULL) return 1;
	hConnetEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hConnetEvent == NULL) return 1;

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// ��ȭ���� ����
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// �޽��� ����
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteCriticalSection(&cs);
	// �̺�Ʈ ����
	CloseHandle(hReadyEvent);
	CloseHandle(hConnetEvent);

	return msg.wParam;
}

// ��ȭ���� ���ν���
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

// ���� ��Ʈ�� ��� �Լ�
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

// �÷��̾� ����Ʈ ��� �߰� �Լ�
void DisplayPlayerAdd(char *fmt, ...)
{
}

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		DisplayText("\r\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		SendMessage(hList1, LB_ADDSTRING, 0, (LPARAM)inet_ntoa(clientaddr.sin_addr));

		// ������ ����
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)client_sock, 0, NULL);
		if(hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }


	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

DWORD WINAPI ProcessTime(LPVOID arg)
{
	//��� �ð� ���� ����
	time_t startimer, timer, oldTimer = 0;

	WaitForSingleObject(hConnetEvent, INFINITE); // Ŭ�� ���� �Ϸ� ��ٸ���

	time(&startimer);	// Ÿ�̸� ���� �ð�

	if (UserData.ready) {
		while (1)
		{
			timer = time(NULL) - startimer;		// �ɸ� �ð� ���

			if (timer != oldTimer)
				DisplayText("\r\n ���� ���۱��� %d�ʰ� ���ҽ��ϴ�.\r", 10 - timer);

			oldTimer = timer;

			if (timer >= 10) {
				DisplayText("\r\n ������ ���۵˴ϴ�.\r");
				Sleep(1000);
				exit(1);
			}
		}
	}

	SetEvent(hReadyEvent);
	return 0;
}

// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while(1){
		WaitForSingleObject(hReadyEvent, INFINITE); // ���� �غ� ���� ��ٸ���

		UserData.clientaddr = clientaddr;

		if(UserData.ready)
			SendMessage(hList2, LB_INSERTSTRING, UserData.id-1, (LPARAM)TEXT("READY"));
		else
			SendMessage(hList2, LB_INSERTSTRING, UserData.id - 1, (LPARAM)TEXT(""));

		// �÷��̾� id �ޱ�
		retval = recvn(client_sock, (char*)&UserData.id, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;


		// �÷��̾� �غ���� �ޱ�
		retval = recvn(client_sock, (char *)&UserData.ready, sizeof(BOOL), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		if (UserData.ready) {
			DisplayText("[TCP/%s:%d] %d �÷��̾��� �غ���´� True �Դϴ�.\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), UserData.id);
		}
		else {
			DisplayText("[TCP/%s:%d] %d �÷��̾��� �غ���´� False �Դϴ�.\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), UserData.id);
		}

		SendMessage(hList2, LB_DELETESTRING, UserData.id-1, 0);

		SetEvent(hConnetEvent); // Ŭ�� ���� �ޱ� �Ϸ�
	}

	// closesocket()
	closesocket(client_sock);
	DisplayText("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}