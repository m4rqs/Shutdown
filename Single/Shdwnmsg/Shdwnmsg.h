#pragma once

#include "..\common\targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
#include <shellapi.h>
#include <commctrl.h>
#include <tchar.h>
#include <strsafe.h>
#include <shlwapi.h>
#include "resource.h"
#include <tlhelp32.h>

#include "..\common\common.h"
#include "..\common\tools.h"

#define ID_TIMER_SHUTDOWN_PROC		1		// Identyfikator licznika zamykania systemu
#define SHUTDOWN_PROC_INTERVAL		1000	// Interwa³ licznika zamykania systemu

#define WM_SHELLNOTIFYICON          (WM_USER + 1)

DWORD CreatePipeThread();

DLLIMPORT BOOL  SHDWNHK_CALL HookLastInputTickCount(void);
DLLIMPORT BOOL  SHDWNHK_CALL UnhookLastInputTickCount(void);
DLLIMPORT DWORD SHDWNHK_CALL GetLastInputTickCount(void);
