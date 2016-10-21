#include "winmain.h"


void Register9xService(DWORD dwServiceType)
{
	HINSTANCE hLibrary;
	FARPROC RegisterServiceProcess;

	hLibrary = LoadLibrary(TEXT("KERNEL32.DLL"));
	if (hLibrary != NULL) 
	{ 
		RegisterServiceProcess = (FARPROC)GetProcAddress(hLibrary, "RegisterServiceProcess"); 
	
		if (NULL != RegisterServiceProcess) 
		{
			RegisterServiceProcess(NULL, dwServiceType);
		}
	
		FreeLibrary(hLibrary); 
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hDialog = NULL;
	BOOL fResult;
	MSG  msg;

	HANDLE hMapFile = CreateFileMapping((HANDLE)(int)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 1024, REG_VALUE_WSCTRL);

	if (hMapFile != NULL && GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		CloseHandle(hMapFile); 
		hMapFile = NULL;
		return 0;
	}

	hInst = hInstance;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	GetVersionEx((OSVERSIONINFO*)&osvi);

	//if (!fTest && osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		Register9xService(RSP_REGISTER_SERVICE);
	}

	NetUpdateWSCtrl(NULL);

	hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SHUTDOWN), NULL, ShutdownDlgProc);

	WSAStartupServer(hDialog);

	while((fResult = GetMessage(&msg, NULL, 0, 0 )) != 0)
	{
		if (fResult == -1)
		{
			// handle the error and possibly exit
		}
		else if (!IsWindow(hDialog) || !IsDialogMessage(hDialog, &msg)) 
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}

	if (hMapFile)
		CloseHandle(hMapFile);

	WSAClose();

	return (int)msg.wParam;
}
