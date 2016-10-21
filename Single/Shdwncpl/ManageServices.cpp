
#include "ManageServices.h"


BOOL MngSrvSaveLogInfo(CHAR* pchFormat, ...)
{
    CHAR achBuffer[LOGINFO_BUFFER_SIZE];
    va_list pArgList;

    va_start(pArgList, pchFormat);
    StringCchVPrintfA(achBuffer, LOGINFO_BUFFER_SIZE, pchFormat, pArgList);
    va_end(pArgList);

    return SaveLogInfo("MSV: %s", achBuffer);
}

VOID ServiceStart(LPCTSTR lpServiceName)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS_PROCESS srvStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;

    MngSrvSaveLogInfo("W³¹czanie serwisu...");
    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager)
    {
        MngSrvSaveLogInfo("B³¹d w OpenSCManager (Kod b³êdu: %d)", GetLastError());
        return;
    }

    // Get a handle to the service
    schService = OpenService(
        schSCManager,         // SCM database
        lpServiceName,        // name of service
        SERVICE_ALL_ACCESS);  // full access

    if (schService == NULL)
    {
        MngSrvSaveLogInfo("B³¹d w OpenService (Kod b³êdu: %d)", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    // Attempt to start the service
    if (!StartService(
            schService, // handle to service
            0,          // number of arguments
            NULL))      // no arguments
    {
        MngSrvSaveLogInfo("B³¹d w StartService (Kod b³êdu: %d)", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }
    else
        MngSrvSaveLogInfo("Trwa uruchamianie srewisu...");

    // Check the status until the service is no longer start pending
    if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // info level
            (LPBYTE) &srvStatus,            // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))                // if buffer too small
    {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = srvStatus.dwCheckPoint;

    while (srvStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        // Do not wait longer than the wait hint. A good interval is
        // one-tenth the wait hint, but no less than 1 second and no
        // more than 10 seconds.

        dwWaitTime = srvStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Check the status again.
 
        if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // info level
            (LPBYTE) &srvStatus,            // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))                // if buffer too small
            break;

        if (srvStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // The service is making progress.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = srvStatus.dwCheckPoint;
        }
        else
        {
            if(GetTickCount() - dwStartTickCount > srvStatus.dwWaitHint)
            {
                // No progress made within the wait hint.
                break;
            }
        }
    }

    // Determine whether the service is running.
    if (srvStatus.dwCurrentState == SERVICE_RUNNING)
    {
        MngSrvSaveLogInfo("Serwis uruchomiony pomyœlnie");
    }
    else
    { 
        MngSrvSaveLogInfo("Serwis nie zosta³ uruchomiony:");
        MngSrvSaveLogInfo("\t Stan: %d", srvStatus.dwCurrentState);
        MngSrvSaveLogInfo("\t  Kod: %d", srvStatus.dwWin32ExitCode);
        MngSrvSaveLogInfo("\tPunkt: %d", srvStatus.dwCheckPoint);
        MngSrvSaveLogInfo("\t Czas: %d", srvStatus.dwWaitHint);
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}


VOID ServiceStop(LPCTSTR lpServiceName)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS_PROCESS srvStatus;
    DWORD dwStartTime = GetTickCount();
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000; // 30-second time-out

    MngSrvSaveLogInfo("Wy³¹czanie serwisu...");

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager)
    {
        MngSrvSaveLogInfo("B³¹d w OpenSCManager (Kod b³êdu: %d)", GetLastError());
        return;
    }

    // Get a handle to the service.
    schService = OpenService(
        schSCManager,         // SCM database
        lpServiceName,        // name of service
        SERVICE_STOP |
        SERVICE_QUERY_STATUS |
        SERVICE_ENUMERATE_DEPENDENTS);

    if (schService == NULL)
    {
        MngSrvSaveLogInfo("B³¹d w OpenService (Kod b³êdu: %d)", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    // Make sure the service is not already stopped.
    if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&srvStatus,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
    {
        MngSrvSaveLogInfo("B³¹d w QueryServiceStatusEx (Kod b³êdu: %d)", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    if (srvStatus.dwCurrentState == SERVICE_STOPPED)
    {
        MngSrvSaveLogInfo("Serwis ju¿ jest wy³¹czony");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    // If a stop is pending, wait for it.
    while (srvStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        MngSrvSaveLogInfo("Trwa wy³¹czanie serwisu...");
        Sleep(srvStatus.dwWaitHint);
        if (!QueryServiceStatusEx(
                schService,
                SC_STATUS_PROCESS_INFO,
                (LPBYTE)&srvStatus,
                sizeof(SERVICE_STATUS_PROCESS),
                &dwBytesNeeded))
        {
            MngSrvSaveLogInfo("B³¹d w QueryServiceStatusEx (Kod b³êdu: %d)", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }

        if (srvStatus.dwCurrentState == SERVICE_STOPPED)
        {
            MngSrvSaveLogInfo("Serwis zosta³ pomyœlnie wy³¹czony");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            MngSrvSaveLogInfo("Serwis nie mo¿e zostaæ wy³¹czony");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }
    }

    // If the service is running, dependencies must be stopped first.
    //StopDependentServices();

    // Send a stop code to the service.
    if (!ControlService(
            schService,
            SERVICE_CONTROL_STOP,
            (LPSERVICE_STATUS) &srvStatus))
    {
        MngSrvSaveLogInfo("B³¹d w ControlService (Kod b³êdu: %d)", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    MngSrvSaveLogInfo("Oczekiwanie na wy³¹czenie serwisu...");
    // Wait for the service to stop.
    while (srvStatus.dwCurrentState != SERVICE_STOPPED)
    {
        Sleep(srvStatus.dwWaitHint);
        if (!QueryServiceStatusEx(
                schService,
                SC_STATUS_PROCESS_INFO,
                (LPBYTE)&srvStatus,
                sizeof(SERVICE_STATUS_PROCESS),
                &dwBytesNeeded))
        {
            MngSrvSaveLogInfo("B³¹d w QueryServiceStatusEx (Kod b³êdu: %d)", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }

        if (srvStatus.dwCurrentState == SERVICE_STOPPED)
            break;

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            MngSrvSaveLogInfo("Czas oczekiwania zosta³ przekroczony");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }
    }
    MngSrvSaveLogInfo("Serwis zosta³ pomyœlnie wy³¹czony");
}

DWORD QueryServiceState(LPCTSTR lpServiceName)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS_PROCESS srvStatus;
    DWORD dwBytesNeeded;

    MngSrvSaveLogInfo("Sprawdzanie stanu serwisu...");
    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager)
    {
        MngSrvSaveLogInfo("B³¹d w OpenSCManager (Kod b³êdu: %d)", GetLastError());
        return 0;
    }

    // Get a handle to the service.
    schService = OpenService(
        schSCManager,         // SCM database
        lpServiceName,        // name of service
        SERVICE_STOP |
        SERVICE_QUERY_STATUS |
        SERVICE_ENUMERATE_DEPENDENTS);

    if (schService == NULL)
    {
        MngSrvSaveLogInfo("B³¹d w OpenService (Kod b³êdu: %d)", GetLastError());
        CloseServiceHandle(schSCManager);
        return 0;
    }

    if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&srvStatus,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
    {
        MngSrvSaveLogInfo("B³¹d w QueryServiceStatusEx (Kod b³êdu: %d)", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 0;
    }

    MngSrvSaveLogInfo("Stanu serwisu: %d", srvStatus.dwCurrentState);

    return srvStatus.dwCurrentState;
}
/*
    SERVICE_PAUSED
    0x00000007
    The service is paused.
     
    SERVICE_PAUSE_PENDING
    0x00000006
    The service is pausing.
     
    SERVICE_CONTINUE_PENDING
    0x00000005
    The service is about to continue.
     
    SERVICE_RUNNING
    0x00000004
    The service is running.
     
    SERVICE_START_PENDING
    0x00000002
    The service is starting.
     
    SERVICE_STOP_PENDING
    0x00000003
    The service is stopping.
     
    SERVICE_STOPPED
    0x00000001
    The service has stopped.
*/
