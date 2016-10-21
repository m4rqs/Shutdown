#pragma once

#include "targetver.h"

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#define WIN_UNKNOWN	0
#define WIN_2000	2000
#define WIN_XP		2002
#define WIN_2003	2003
#define WIN_VISTA	2007
#define WIN_2008	2008

#include <windows.h>
#include <strsafe.h>
#include <shlwapi.h>

BOOL SaveLogInfo(CHAR*, ...);
void ShutdownBeep();
BOOL SetDlgItemFont(HWND, int, LPTSTR, BYTE, BOOL, BOOL, BOOL);
BOOL SetColors(COLORREF, COLORREF, WPARAM);
DWORD GetSystemVersion(VOID);
VOID SetBit(LPUINT, BYTE, BOOL);
BOOL CheckBit(DWORD, BYTE);
