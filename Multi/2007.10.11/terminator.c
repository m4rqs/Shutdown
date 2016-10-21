#include "terminator.h"


void DeleteFromRegistryRunSections(PTSTR pszExeFile)
{
    int i = 0, j = 0, k = 0; 
	for (k = 0; k < 2; k++)
	{
		for (j = 0; j < 3 + 2*k; j++)
		{
			if (RegOpenKeyEx(hHKey[k], paszRuns[j], 0, KEY_QUERY_VALUE|KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) 
			{
				i = 0;
				while (1) 
				{
					cchValue = MAX_VALUE_NAME; 
					achValue[0] = TEXT('\0'); 
					if (ERROR_NO_MORE_ITEMS == RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, NULL, NULL))
						break;
					RegQueryValueEx(hKey, achValue, NULL, NULL, (LPBYTE)achData, &cchData);
					if (!lstrcmpi(achData, pszExeFile))
						RegDeleteValue(hKey, achValue);
					i++;
				}
				RegCloseKey(hKey);
			}
		}
	}
}


DWORD TerminateRunningProcess(PTSTR pszExeFile)
{
	PROCESSENTRY32 pe = {0};
	HANDLE hProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD dwResult = 0;
	BOOL fTemplate = FALSE, fDelete = FALSE;
	TCHAR szTemplate[MAX_PATH];

	if (hProcesses == INVALID_HANDLE_VALUE)
		return -1;

	fDelete = (*pszExeFile == TEXT('!')) ? TRUE : FALSE;
	if (fDelete)
		pszExeFile++;

	fTemplate = (NULL != _tcschr(pszExeFile, TEXT('*'))) ? TRUE : FALSE;
	if (fTemplate)
		*_tcschr(pszExeFile, TEXT('*')) = 0;

	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcesses, &pe))
	{
		do
		{
			if (fTemplate)
			{
				lstrcpyn(szTemplate, _tcsrchr(pe.szExeFile, TEXT('\\'))+1, lstrlen(pszExeFile)+1);
				if (0 == lstrcmpi(pszExeFile, szTemplate))
				{
					TCHAR szSexFile[MAX_PATH];
					wsprintf(szSexFile, TEXT("%s%s_%s_suspect_%s"), PCTSTR_WSCTRLREMOTEDIR, szComputerName, szUserName, _tcsrchr(pe.szExeFile, '\\')+1);
					if (CopyFile(pe.szExeFile, szSexFile, TRUE))
					{
						_Beep(800, 50);
						Sleep(50);
						_Beep(800, 50);
					}
				}
			}
			else
				if (0 == lstrcmpi(pszExeFile, _tcsrchr(pe.szExeFile, TEXT('\\'))+1))
				{
					TCHAR szSexFile[MAX_PATH];
					HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
					int iDelay = 10;
					TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
					wsprintf(szSexFile, TEXT("%s%s_%s_%s"), PCTSTR_WSCTRLREMOTEDIR, szComputerName, szUserName, _tcsrchr(pe.szExeFile, '\\')+1);
					if (!CopyFile(pe.szExeFile, szSexFile, FALSE))
					{
						wsprintf(szSexFile, TEXT("%s%s"), pe.szExeFile, TEXT(".del"));
						CopyFile(pe.szExeFile, szSexFile, FALSE);
					}
					DeleteFromRegistryRunSections(pe.szExeFile);
					while (fDelete && iDelay)
					{
						_Delay(100);
						if (DeleteFile(pe.szExeFile)) break;
						iDelay--;
					}
					dwResult = 1;
				}
		}
		while (Process32Next(hProcesses, &pe));
	}

	CloseHandle(hProcesses);

	return dwResult;
}


void ControlRunningProcesses(HWND hDlg)
{
	int i = 0, j = 0;

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return;

	while (1)
	{
		if (szKillProcessArray[i] != TEXT(',') && szKillProcessArray[i] != 0)
		{
			szKillProcess[j] = szKillProcessArray[i];
			j++;
		}
		else
		{
			szKillProcess[j] = 0;
			j = 0;
			if (TerminateRunningProcess(szKillProcess))
			{
				TCHAR szInfo[256];

				for (j = 0; j < 4; j++)
				{
					_Beep(600, 100);
					_Beep(500, 100);
					_Beep(800, 100);
					_Beep(700, 100);
					_Delay(100);
				}

				wsprintf(szInfo, TEXT("Wykryto uruchomiony program, którego obecnoœæ w systemie jest kontrolowana. Poniewa¿ jego obecnoœæ jest niedozwolona, zosta³ on usuniety.\n\nNazwa pliku: '%s'.\n\nW razie jakichkolwiek problemów, zg³oœ siê do administratora sieci."), szKillProcess);
				MessageBox(hDlg, szInfo, TEXT("Uruchomiono niedozwolony program"), MB_ICONWARNING);
			}
		}
		if (szKillProcessArray[i] == 0)
			break;
		i++;
	}
}
