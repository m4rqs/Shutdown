
#include "shdwnsrv.h"

int _tmain(int argc, TCHAR* argv[])
{
    SERVICE_TABLE_ENTRY DispatchTable[] = 
    { 
        { szServiceName, ServiceMain }, 
        { 0,             0 } 
    };

    //SaveLogInfo("Logowanie...");

    if (argc > 1 && !(StrCmpI(argv[1], TEXT("-i"))))
    {
        TCHAR szBuffer[255];
        TCHAR szPath[MAX_PATH];
        SC_HANDLE scmHandle, scHandle;
        SERVICE_DESCRIPTION srvDesc;

        SaveLogInfo("Instalacja serwisu...");

		scmHandle = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (scmHandle == NULL) // Perform error handling.
        {
            SaveLogInfo("B³¹d w OpenSCManager (Kod b³êdu: %d)", GetLastError()); 
        }

        GetModuleFileName(GetModuleHandle(NULL), szPath, MAX_PATH);
        // Why do we do this?  The path may contain spaces.
        //  If it does, the path must be quoted.
        StringCchCopy(szBuffer, sizeof(szBuffer), TEXT("\""));
        StringCchCat(szBuffer, sizeof(szBuffer), szPath);
        StringCchCat(szBuffer, sizeof(szBuffer), TEXT("\""));
        _tprintf(TEXT("\nInstalacja serwisu %s"), szPath);

        scHandle = CreateService (scmHandle, (LPCTSTR)szServiceName, 
            REGSTR_DATA_DISPLAYNAME, SERVICE_ALL_ACCESS, 
            SERVICE_WIN32_OWN_PROCESS, 
            SERVICE_AUTO_START,//SERVICE_DEMAND_START, 
            SERVICE_ERROR_NORMAL, 
			szBuffer,
			NULL, NULL, NULL, NULL, NULL);

        if (scHandle == NULL) // Process error
        {
            SaveLogInfo("B³¹d w CreateService (Kod b³êdu: %d)", GetLastError()); 
        }
		else
        {
            SaveLogInfo("Serwis zainstalowany");
			_tprintf(TEXT("\nSerwis %s zainstalowany"), szServiceName);
        }

        srvDesc.lpDescription = REGSTR_DATA_DESCRIPTION;
        ChangeServiceConfig2(scHandle, SERVICE_CONFIG_DESCRIPTION, (LPVOID)&srvDesc);

        CloseServiceHandle(scHandle);
        CloseServiceHandle(scmHandle);
    }
    else if (argc>1 && !(StrCmpI(argv[1], TEXT("-u"))))
    {
        SC_HANDLE scmHandle, scHandle;

        SaveLogInfo("Dezinstalacja serwisu...");

        scmHandle = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (scmHandle == NULL) // Perform error handling.
        {
            SaveLogInfo("B³¹d w OpenSCManager (Kod b³êdu: %d)", GetLastError()); 
        }

        scHandle = OpenService(scmHandle, szServiceName, SERVICE_ALL_ACCESS);
        DeleteService(scHandle);

        SaveLogInfo("Serwis odinstalowany");
        _tprintf(TEXT("\nSerwis %s odinstalowany"), szServiceName);
    }
    else
    {
        SaveLogInfo("Uruchamianie serwisu...");

		if (!StartServiceCtrlDispatcher(DispatchTable)) 
        { 
            SaveLogInfo("B³¹d w StartServiceCtrlDispatcher (Kod b³êdu: %d)", GetLastError()); 
        }
    } 
    return 0;
}

VOID ShutdownSystem(VOID)
{
    HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 

    SaveLogInfo("Zamykanie systemu przez serwis...");
    ShutdownBeep();

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken); 
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
     
    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
     
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
    //ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_FLAG_PLANNED);
    //WTSShutdownSystem(WTS_CURRENT_SERVER_HANDLE, WTS_WSD_POWEROFF);
}

BOOL Wide2Byte(LPSTR pbBuffer, size_t cbBuffer, LPWSTR pwBuffer)
{
	return (0 != WideCharToMultiByte(CP_ACP,                         // CodePage
						             0,                              // dwFlags
						             pwBuffer,						 // lpWideCharStr
						             -1,                             // cchWideChar
						             pbBuffer,                       // lpMultiByteStr
						             cbBuffer,		                 // cchMultiByte,
						             NULL,                           // lpDefaultChar,
						             NULL));                         // lpUsedDefaultChar
}

/*
BOOL IsUserLogged()
{
    SaveLogInfo("1");
    BOOL fResult = FALSE;
    PWTS_SESSION_INFO pSessionsInfo, pSessionInfo;
    DWORD dwCount;

    BOOL fWTS = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionsInfo, &dwCount);
    pSessionInfo = pSessionsInfo;

    SaveLogInfo("2 - %i - %d", fWTS, GetLastError());
    for (DWORD i=0; i<dwCount; i++)
    {
        SaveLogInfo("Sesja nr %i", i);
        if (pSessionInfo->State == WTSActive)
        {
            fResult = TRUE;
            SaveLogInfo("Name: %s; ID: %d; State: %i", pSessionInfo->pWinStationName, pSessionInfo->SessionId, pSessionInfo->State);
            break;
        }
        pSessionInfo++;
    }

    SaveLogInfo("3");
    WTSFreeMemory(pSessionsInfo);

    SaveLogInfo("4");
    return fResult;
}
*/
DWORD GetActiveSessionID()
{
    DWORD dwResult = 0xFFFFFFFF;

    if (GetSystemVersion() == WIN_2000)
    {
        PROCESSENTRY32 pe = {0};
	    pe.dwSize = sizeof(PROCESSENTRY32);

		HANDLE hProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hProcesses == INVALID_HANDLE_VALUE)
			return FALSE;

		if (Process32First(hProcesses, &pe))
		{
			do
			{
				if (StrCmpI(pe.szExeFile, TEXT("winlogon.exe")) == 0)
				{
					if (ProcessIdToSessionId(pe.th32ProcessID, &dwResult))
                    {
						break;
                    }
				}
			}
			while (Process32Next(hProcesses, &pe));
		}

		CloseHandle(hProcesses);
    }
    else
    {
        PWTS_SESSION_INFO pSessionsInfo, pSessionInfo;
        DWORD dwCount;

        WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionsInfo, &dwCount);
        pSessionInfo = pSessionsInfo;

        for (DWORD i=0; i<dwCount; i++)
        {
            if (pSessionInfo->State == WTSActive)
            {
                dwResult = pSessionInfo->SessionId;
                break;
            }
            pSessionInfo++;
        }

        WTSFreeMemory(pSessionsInfo);
    }
    return dwResult;
}
/*
BOOL GetActiveUserName(LPTSTR pszUserName, size_t cchUserName)
{
    BOOL fResult = FALSE;
    PWTS_SESSION_INFO pSessionsInfo, pSessionInfo;
    DWORD dwCount;

    WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionsInfo, &dwCount);
    pSessionInfo = pSessionsInfo;

    for (DWORD i=0; i<dwCount; i++)
    {
        if (pSessionInfo->State == WTSActive)
        {
            if ((pszUserName != NULL) && (cchUserName > 0))
            {
                LPTSTR pBuffer;
                DWORD dwBytesReturned;

                WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, pSessionInfo->SessionId, WTSUserName, &pBuffer, &dwBytesReturned);
                StringCbCopyN(pszUserName, cchUserName, pBuffer, dwBytesReturned);

                WTSFreeMemory(pBuffer);
            }
            fResult = TRUE;
            break;
        }
        pSessionInfo++;
    }

    WTSFreeMemory(pSessionsInfo);

    return fResult;
}
*/

DWORD SendMessageCode(DWORD dwCode)
{
    DWORD cbBytesRead;
    MSGDATA_PACKET sMsgDataWrite, sMsgDataRead;

    //if (IsUserLogged())
    if (GetActiveSessionID() != 0xFFFFFFFF)
    {
        DWORD dwErrorCode = ERROR_SUCCESS;

		do
        {
            RunShutdownMessenger();

            SaveLogInfo("Wys³anie informacji do menad¿era (kod: %i)", dwCode);

            ZeroMemory(&sMsgDataWrite, MSGDATA_PACKET_SIZE);
            ZeroMemory(&sMsgDataRead, MSGDATA_PACKET_SIZE);

            sMsgDataWrite.dwCode = dwCode;
		    BOOL fSuccess;

		    do
		    {
			    fSuccess = CallNamedPipe( 
				    LPCTSTR_SHDWNPIPE_NAME,	// pipe name 
				    &sMsgDataWrite,			// message to server 
				    MSGDATA_PACKET_SIZE,	// message length 
				    &sMsgDataRead,			// buffer to receive reply 
				    MSGDATA_PACKET_SIZE,	// size of read buffer 
				    &cbBytesRead,           // number of bytes read 
				    10000);                 // waits for 10 seconds 
		    }
            // Dajmy menad¿erowi czas, aby pomyœlnie zainicjowa³ potok
		    while ((dwCode == SHDWNCODE_TEST) && !fSuccess);

            dwErrorCode = GetLastError();

		    if (fSuccess || (dwErrorCode == ERROR_MORE_DATA)) 
		    { 
			    // The pipe is closed; no more data can be read. 
			    if (!fSuccess) 
			    {
				    SaveLogInfo("B³¹d w komunikacji z menad¿erem. Kod b³êdu: %i", dwErrorCode);
				    return SHDWNCODE_ERROR;
			    }
			    else
			    {
				    SaveLogInfo("Kod odpowiedzi od Menad¿era: %i%s", sMsgDataRead.dwCode, (sMsgDataRead.dwCode == 1) ? " (Roger That)" : "");
				    return sMsgDataRead.dwCode;
			    }
		    }
		    else
		    {
			    SaveLogInfo("Menad¿er nie odpowiada. Kod b³êdu: %i", dwErrorCode);
                // Na wypadek gdyby ktoœ zamkn¹³ menad¿era wyœwietlaj¹cego komunikat o zamykaniu
                // i tym samym zerwa³ potok
                if (dwErrorCode == ERROR_BROKEN_PIPE)
                {
			        SaveLogInfo("Prawdopodobnie menad¿er zosta³ wy³¹czony podczas wyœwietlanego okna komunikatu");
                    continue;
                }
			    SaveLogInfo("SprawdŸ czy menad¿er jest prawid³owo uruchamiany");
			    return SHDWNCODE_CANCEL;
		    }
        }
        // Na wypadek gdyby ktoœ zamkn¹³ menad¿era wyœwietlaj¹cego komunikat o zamykaniu
        // i tym samym zerwa³ potok
        while (dwErrorCode == ERROR_BROKEN_PIPE);
    }

    return SHDWNCODE_LOGOFF;
}

BOOL RunShutdownMessenger(VOID)
{

//#if WINVER > 0x0500
//	DWORD dwSessionId = WTSGetActiveConsoleSessionId();
//#else
	DWORD dwSessionId = GetActiveSessionID();
//#endif

	if (dwSessionId == 0xFFFFFFFF)
	{
		if (fUserLoggedOn)
		{
			SaveLogInfo("¯adna sesja nie jest aktywna. Uruchomienie Menad¿era anulowane.");
			fUserLoggedOn = false;
		}
		return FALSE;
	}

	fUserLoggedOn = true;

	HANDLE hProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcesses == INVALID_HANDLE_VALUE)
		return FALSE;

    BOOL fRunMessenger = true;
	PROCESSENTRY32 pe = {0};
	pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcesses, &pe))
	{
		do
		{
            if (StrCmpI(pe.szExeFile, TEXT("shdwnmsg.exe")) == 0)
            {
				DWORD dwShwndmsgSID = 0;
				if (ProcessIdToSessionId(pe.th32ProcessID, &dwShwndmsgSID) && dwShwndmsgSID == dwSessionId)
                {
                    fRunMessenger = false;
                    break;
                }
            }
		}
		while (Process32Next(hProcesses, &pe));
	}

	CloseHandle(hProcesses);

    if (fRunMessenger)
    {
        STARTUPINFO si;
		PROCESS_INFORMATION pi;
        HANDLE /*hUserToken,*/ hProcessToken, hDuplicateToken;
		DWORD dwWinLogonPID;

        SaveLogInfo("Nie mo¿na odnaleŸæ procesu Menad¿era!");
        SaveLogInfo("Próba uruchomienia Menad¿era...");

		hProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcesses == INVALID_HANDLE_VALUE)
			return FALSE;

		if (Process32First(hProcesses, &pe))
		{
			do
			{
				if (StrCmpI(pe.szExeFile, TEXT("winlogon.exe")) == 0)
				{
					// We found a winlogon process...
					// make sure it's running in the console session
					DWORD dwWinLogonSID = 0;
					if (ProcessIdToSessionId(pe.th32ProcessID, &dwWinLogonSID) && dwWinLogonSID == dwSessionId)
					{
						dwWinLogonPID = pe.th32ProcessID;
						break;
					}
				}
			}
			while (Process32Next(hProcesses, &pe));
		}

		CloseHandle(hProcesses);

        ZeroMemory( &si, sizeof(STARTUPINFO) );
        si.cb = sizeof(STARTUPINFO);
		si.lpDesktop = TEXT("winsta0\\default");

        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		
		//WTSQueryUserToken(dwSessionId, &hUserToken);

		//DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE;
		TOKEN_PRIVILEGES tp;
		LUID luid;

		HANDLE hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, dwWinLogonPID);

		if(!::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
			|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
			|TOKEN_READ|TOKEN_WRITE, &hProcessToken))
		{
			SaveLogInfo("OpenProcessToken failed (%d)", GetLastError());
		}

		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
		{
			SaveLogInfo("LookupPrivilegeValue failed (%d)", GetLastError());
		}

		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		DuplicateTokenEx(hProcessToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDuplicateToken);

		//Adjust Token privilege
		//SetTokenInformation(hDuplicateToken, TokenSessionId, (void*)dwSessionId, sizeof(DWORD));

		if (!AdjustTokenPrivileges(hDuplicateToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, NULL))
		{
			SaveLogInfo("Adjust Privilege value Error: %u\n",GetLastError());
		}

		TCHAR szPath[MAX_PATH];

		GetModuleFileName(NULL, szPath, MAX_PATH);
		LPTSTR pch = StrRChrI(szPath, NULL, TEXT('\\'));
		StringCchCopy(pch+1, MAX_PATH-(pch-szPath+1), TEXT("Shdwnmsg.exe"));

		// Start the child process. 
        if( !CreateProcessAsUser(hDuplicateToken,
			szPath,			// No module name (use command line)
            NULL,			// Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,			// Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory 
            &si,            // Pointer to STARTUPINFO structure
            &pi )   // Pointer to PROCESS_INFORMATION structure
            ) 
        {
            SaveLogInfo("CreateProcess failed (%d)", GetLastError());
			SaveLogInfo("Próba uruchomienia Menad¿era siê nie powiod³a");
            CloseHandle(hDuplicateToken);
			CloseHandle(hProcessToken);
        }
        else
        {
            SaveLogInfo("Menad¿er zosta³ pomyœlnie uruchomiony");
            CloseHandle(hDuplicateToken);
			CloseHandle(hProcessToken);
            //Sleep(50);
			if (SHDWNCODE_OK == SendMessageCode(SHDWNCODE_TEST)) // Czekamy, a¿ proces zd¹rzy utworzyæ potok
				SaveLogInfo("Komunikacja z manad¿erem nawi¹zana");
			else
				SaveLogInfo("Nie mo¿na nawi¹zaæ komunikacji z menad¿erem");
            return TRUE;
        }
    }

	return FALSE;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv) 
{ 
    hServiceStatus = RegisterServiceCtrlHandlerEx( 
        szServiceName, 
        (LPHANDLER_FUNCTION_EX)HandlerEx,
        (LPVOID)1); 
 
    if (hServiceStatus == (SERVICE_STATUS_HANDLE)0) 
    { 
        SaveLogInfo("B³¹d w RegisterServiceCtrlHandler (Kod b³êdu: %d)", GetLastError()); 
        return; 
    }

    lpServiceStatus->dwServiceType        = SERVICE_WIN32_OWN_PROCESS; 
    lpServiceStatus->dwCurrentState       = SERVICE_START_PENDING; 
    lpServiceStatus->dwControlsAccepted   = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN; 
    lpServiceStatus->dwWin32ExitCode      = 0; 
    lpServiceStatus->dwServiceSpecificExitCode = 0; 
    lpServiceStatus->dwCheckPoint         = 0; 
    lpServiceStatus->dwWaitHint           = 0; 
 
    if (!SetServiceStatus(hServiceStatus, lpServiceStatus)) 
    { 
        SaveLogInfo("B³¹d w SetServiceStatus (Kod b³êdu: %ld)", GetLastError()); 
        SaveLogInfo("Nie mo¿na zainicjalizowaæ serwisu...");
        return;
    }

    SaveLogInfo("Inicjalizacja serwisu...");

	DWORD dwInfo;
	HKEY hKey;
	BOOL fShutdown = false, fWarning = false;
	SYSTEMTIME st, stStop = {2008,1,2,1,14,30,0,0};
	DWORD dwLoopInterval = 5000; // Czas na opóŸnienie pêtli (Domyœlnie 1 min.)
	DWORD dwNextTrial = 0;
	DWORD dwIdleTime = 1;
	DWORD dwShutdownMode = 0;
	//DWORD dwCurrentSystemTickCount = GetTickCount();
    DWORD dwWaitForStart = 10000; // Czas na ewentualne zalogowanie sie u¿ytkownika (Domyœlnie 5 min.)
    DWORD dwMinSrvTickCount = 10000; // Minimalny czas dzia³ania systemu (Domyœlnie 5 min.)
    DWORD dwWarning = 0;
    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) 
	{
		if (RegQueryValueEx(hKey, REGSTR_VALUE_LOOPINTERVAL, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey, REGSTR_VALUE_LOOPINTERVAL, 0, NULL, (BYTE *)&dwLoopInterval, &dwInfo);
		}
		if (RegQueryValueEx(hKey, REGSTR_VALUE_MINSRVTICKCOUNT, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey, REGSTR_VALUE_MINSRVTICKCOUNT, 0, NULL, (BYTE *)&dwMinSrvTickCount, &dwInfo);
		}
		if (RegQueryValueEx(hKey, REGSTR_VALUE_WAIT4START, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey, REGSTR_VALUE_WAIT4START, 0, NULL, (BYTE *)&dwWaitForStart, &dwInfo);
		}
		RegCloseKey(hKey);
	}

    lpServiceStatus->dwCurrentState = SERVICE_RUNNING; 
 
    if (!SetServiceStatus(hServiceStatus, lpServiceStatus)) 
    { 
        SaveLogInfo("B³¹d w SetServiceStatus (Kod b³êdu: %ld)", GetLastError()); 
        SaveLogInfo("Nie mo¿na rozpocz¹æ dzia³ania serwisu");
        return;
    }

    fRunning = true;

    SaveLogInfo("Rozpoczêcie dzia³ania serwisu...");
	fUserLoggedOn = true; // Na potrzeby funkcji RunShutdownMessenger()
	SendMessageCode(SHDWNCODE_START);

    while (1)
    {
		if (fRunning == false)
        {
            SendMessageCode(SHDWNCODE_STOP);
            break;
        }

        if (dwWaitForStart > 0)
        {
            WaitForSingleObject(hEvent, dwWaitForStart);
            dwWaitForStart -= dwLoopInterval;
            continue;
        }

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) 
		{
			if (RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, NULL, (BYTE *)&dwShutdownMode, &dwInfo);
			}
			RegCloseKey(hKey);
		}

        if (dwShutdownMode != SHDWNMODE_STOPPED)
        {
	        if (dwShutdownMode == SHDWNMODE_TIME)
            {
		        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) 
		        {
			        if (RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNTIME, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			        {
				        RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNTIME, 0, NULL, (BYTE *)&stStop, &dwInfo);
			        }
			        RegCloseKey(hKey);
		        }
	            GetLocalTime(&st);
		        fShutdown = ((st.wHour > stStop.wHour) || ((st.wHour == stStop.wHour) && (st.wMinute >= stStop.wMinute))) ? TRUE : FALSE;
                //if ((GetTickCount() - dwCurrentSystemTickCount) < dwMinSrvTickCount)
                if (fShutdown && (GetTickCount() <= dwMinSrvTickCount))
                {
                    SaveLogInfo("Komputer powinien o tej godzinie byæ wy³¹czony");
                    SaveLogInfo("Poniewa¿ komputer zosta³ w³aœnie uruchomiony, wy³¹czanie zostanie uruchomione za: %i min.", dwMinSrvTickCount/60000);
                    SendMessageCode(SHDWNCODE_SHOULDBEDOWN);
                    WaitForSingleObject(hEvent, dwMinSrvTickCount);
                    continue;
                }
            }
            else if (dwShutdownMode & SHDWNMODE_IDLE)
            {
                DWORD dwLastInputTickCount = 0;
		        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) 
		        {
			        if (RegQueryValueEx(hKey, REGSTR_VALUE_IDLETIME, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			        {
				        RegQueryValueEx(hKey, REGSTR_VALUE_IDLETIME, 0, NULL, (BYTE *)&dwIdleTime, &dwInfo);
			        }
			        if (RegQueryValueEx(hKey, REGSTR_VALUE_LASTINPUTTICKCOUNT, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			        {
				        RegQueryValueEx(hKey, REGSTR_VALUE_LASTINPUTTICKCOUNT, 0, NULL, (BYTE *)&dwLastInputTickCount, &dwInfo);
			        }
			        if (RegQueryValueEx(hKey, REGSTR_VALUE_NEXTTRIAL, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			        {
				        RegQueryValueEx(hKey, REGSTR_VALUE_NEXTTRIAL, 0, NULL, (BYTE *)&dwNextTrial, &dwInfo);
			        }
			        RegCloseKey(hKey);
		        }
                DWORD dwItem = dwShutdownMode & SHDWNMODE_TIME ? dwNextTrial : dwIdleTime;
                DWORD dwIdleTickCount = GetTickCount() - dwLastInputTickCount;
                fShutdown = dwIdleTickCount >= sIdleOption[dwItem].dwTime;
                fWarning = dwWarning++ == 5;// == 5
            }
            else
            {
                dwShutdownMode = SHDWNMODE_STOPPED;
		        if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS)
		        {
			        RegSetValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, REG_DWORD, (CONST BYTE*)&dwShutdownMode, sizeof(DWORD));
			        RegCloseKey(hKey);
		        }
                SaveLogInfo("Nieznany tryb dzia³ania serwisu");
				SaveLogInfo("Zmiana trybu dzia³ania serwisu na status: zatrzymany");
				fRunning = false;
                continue;
            }
            
            // Co pi¹ty licznik wyœlij ostrze¿enie (co 5*dwLoopInterval)
            if (fWarning)
            {
                SendMessageCode(SHDWNCODE_WARNING);
                dwWarning = 0;
            }

		    if (fShutdown)
		    {
				SaveLogInfo("Próba zamkniêcia systemu...");

	            if (dwShutdownMode == SHDWNMODE_TIME)
                {
                    dwShutdownMode = SHDWNMODE_IDLETIME;
		            if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS)
		            {
			            RegSetValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, REG_DWORD, (CONST BYTE*)&dwShutdownMode, sizeof(DWORD));
			            RegCloseKey(hKey);
                        SaveLogInfo("Zmiana trybu dzia³ania serwisu na status: oczekiwanie na zamkniêcie");
                        SendMessageCode(SHDWNCODE_IDLE);
                        WaitForSingleObject(hEvent, dwLoopInterval);
                        continue;
		            }
                }

                DWORD dwCode = SendMessageCode(SHDWNCODE_SHUTDOWN);
			    if (dwCode == SHDWNCODE_WAIT)
                {
                    SaveLogInfo("OdpowiedŸ od menad¿era: czekaj");
                    WaitForSingleObject(hEvent, dwLoopInterval);
                    continue;
                }
                else if (dwCode == SHDWNCODE_ERROR)
                {
                    continue;
                }
                else if (dwCode == SHDWNCODE_CANCEL)
                {
                    SaveLogInfo("Anulowano zamykanie systemu z powodu braku mo¿liwoœci ostrze¿enia u¿ytkownika");
                    break;
                }
			    else if (dwCode == SHDWNCODE_SHUTDOWN)
                {
                    SaveLogInfo("OdpowiedŸ od menad¿era: zamknij system");
                }
                else if (dwCode == SHDWNCODE_LOGOFF)
                {
                    SaveLogInfo("¯aden u¿ytkownik nie jest zalogowany");
                    SaveLogInfo("System zostanie wy³¹czony bez ostrze¿enia");
                }

                fRunning = false;
				continue;
		    }
        }
        else
        {
            SaveLogInfo("Zmiana trybu dzia³ania serwisu na status: zatrzymany");
			fRunning = false;
			continue;
        }
        WaitForSingleObject(hEvent, dwLoopInterval);
	}
	
    SaveLogInfo("Zatrzymywanie dzia³ania serwisu...");

    lpServiceStatus->dwCurrentState = SERVICE_STOPPED; 

    if (!SetServiceStatus(hServiceStatus, lpServiceStatus)) 
    { 
        SaveLogInfo("B³¹d w SetServiceStatus (Kod b³êdu: %ld)", GetLastError()); 
    } 
    
    CloseHandle(hEvent);

    if (fShutdown)
        ShutdownSystem();

    return; 
} 

VOID WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{ 
	switch (dwControl) 
    { 
        case SERVICE_CONTROL_STOP: 
            SaveLogInfo("Zatrzymywanie serwisu...");
            lpServiceStatus->dwCurrentState = SERVICE_STOP_PENDING; 
            fRunning = false;
            if (!SetServiceStatus(hServiceStatus, lpServiceStatus))
            { 
                SaveLogInfo("B³¹d w SetServiceStatus (Kod b³êdu: %ld)", GetLastError()); 
            } 
            return; 
 
        case SERVICE_CONTROL_INTERROGATE: 
        // Fall through to send current status. 
            break;

        case SERVICE_CONTROL_SHUTDOWN: 
            SaveLogInfo("System jest wy³¹czany..."); 
            break;
 
        default: 
            SaveLogInfo("Nierozpoznany kod menad¿era us³ug: %ld", dwControl); 
    } 
 
    if (!SetServiceStatus(hServiceStatus, lpServiceStatus)) 
    { 
        SaveLogInfo("B³¹d w SetServiceStatus (Kod b³êdu: %ld)", GetLastError()); 
    } 
    return; 
} 
