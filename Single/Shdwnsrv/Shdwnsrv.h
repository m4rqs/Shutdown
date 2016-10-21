#pragma once

#include "..\common\targetver.h"

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <windows.h>
#include <winsvc.h>
#include <tchar.h>
#include <strsafe.h>
#include <shlwapi.h>
//#include <winuser.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <WtsApi32.h>

#include <comdef.h>
//  Include the task header file.
#include <taskschd.h>
# pragma comment(lib, "taskschd.lib")
# pragma comment(lib, "comsupp.lib")
# pragma comment(lib, "credui.lib")

#include "..\common\common.h"
#include "..\common\tools.h"

//#if WINVER == 0x0500
//	#define WTS_SESSION_LOGON                  0x5
//	#define WTS_SESSION_LOGOFF                 0x6
//#endif

SERVICE_STATUS          ServiceStatus; 
LPSERVICE_STATUS        lpServiceStatus = &ServiceStatus; 
SERVICE_STATUS_HANDLE   hServiceStatus; 

TCHAR szServiceName[] = LPCTSTR_SERVICE_NAME;
BOOL fRunning = false; // Czy serwis dzia³a
//BOOL fSuspend = false; // Czy serwis nie by³ zatrzymany
//BOOL fOptionChanged = false; // Czy zosta³ zmieniony tryb
BOOL fUserLoggedOn = false;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv); 

//VOID WINAPI Handler(DWORD dwControl);
VOID WINAPI HandlerEx(DWORD, DWORD, LPVOID, LPVOID);
BOOL RunShutdownMessenger(VOID);
DWORD GetActiveSessionID();
