// Shdwnpsp.cpp : Defines the entry point for the application.
//

#include "Shdwnpsp.h"

HWND g_hWnd = NULL;
/*
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    #define WND_TITLE_LENGTH 256
	TCHAR szWindowTitle[WND_TITLE_LENGTH] = {0};
    size_t cch;
    StringCchLength((LPCTSTR)lParam, WND_TITLE_LENGTH - 1, &cch);
	GetWindowText(hWnd, szWindowTitle, ++cch);
	if (StrCmpI(szWindowTitle, (LPCTSTR)lParam) == 0)
	{
		g_hWnd = hWnd;
		return FALSE;
	}
	return TRUE;
}*/

int APIENTRY
_tWinMain
(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine,
    int nCmdShow
)
{
    HWND hSystemProperties = FindWindow(NULL, TEXT("W³aœciwoœci systemu"));
    if (hSystemProperties == NULL)
    {
        ShellExecute(NULL, TEXT("open"), TEXT("Rundll32.exe"), TEXT("shell32.dll,Control_RunDLL Sysdm.cpl"), NULL, SW_SHOWNORMAL);
        do {
            hSystemProperties = FindWindow(NULL, TEXT("W³aœciwoœci systemu"));
        } while (hSystemProperties == NULL);
    }

    if (hSystemProperties != NULL)
    {
        HWND hTabControl = (HWND)SendMessage(hSystemProperties, PSM_GETTABCONTROL, 0, 0);
        int n = SendMessage(hTabControl, TCM_GETITEMCOUNT, 0, 0);
        HWND hWndShutdown = NULL;
        HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        // Nie wiem dlaczego ale SendMessage siê spóŸnia z odpowiedzi¹ na XP
        if (GetSystemVersion() < WIN_VISTA)
            WaitForSingleObject(hEvent, 50);
        CloseHandle(hEvent);
        for (int i=n-1; i>0; i--)
        {
            // Musimy wczytaæ do pamiêci zak³adki. Zaczynamy od ostatniej.
            SendMessage(hTabControl, TCM_SETCURFOCUS, i, 0);
            hWndShutdown = FindWindowEx(hSystemProperties, NULL, TEXT("#32770"), TEXT("AutoShutdown"));
            if (hWndShutdown != NULL)
            {
                int iIndex = SendMessage(hSystemProperties, PSM_HWNDTOINDEX, (WPARAM)hWndShutdown, 0);
                SendMessage(hSystemProperties, PSM_SETCURSEL, iIndex, 0);
                break;
            }
        }
    }

    return 0;
}
