#pragma once

// Deklaracje sta³ych
#define LPCTSTR_SERVICE_NAME			TEXT("AutoShutdown")
#define LPCTSTR_SHDWNPIPE_NAME			TEXT("\\\\.\\pipe\\msnpshdwn")

#define LPCTSTR_BUSINESS				TEXT("Marek Sienczak")
#define LPCTSTR_APP_NAME				TEXT("Menad¿er zamykania systemu")

#define REGSTR_PATH_BUSINESS			TEXT("Software\\") LPCTSTR_BUSINESS
#define REGSTR_PATH_APP					REGSTR_PATH_BUSINESS TEXT("\\") LPCTSTR_APP_NAME

#define REGSTR_VALUE_ADMINMODE			TEXT("AdminMode")
#define REGSTR_VALUE_IDLETIME			TEXT("IdleTime")
#define REGSTR_VALUE_LASTINPUTTICKCOUNT	TEXT("LastInputTickCount")
#define REGSTR_VALUE_LOOPINTERVAL		TEXT("LoopInterval")
#define REGSTR_VALUE_MESSAGETEXT		TEXT("MessageText")
#define REGSTR_VALUE_MINSRVTICKCOUNT	TEXT("MinSrvTickCount")
#define REGSTR_VALUE_NEXTTRIAL			TEXT("NextTrial")
#define REGSTR_VALUE_SHUTDOWNMODE		TEXT("ShutdownMode")
#define REGSTR_VALUE_SHUTDOWNTIME		TEXT("ShutdownTime")
#define REGSTR_VALUE_WAIT4START		    TEXT("WaitForStart")
#define REGSTR_VALUE_ACTIVETHREADS      TEXT("ActiveThreads")

#define REGSTR_MAX_VALUE_LENGTH			256
#define REGSTR_MAX_KEY_LENGTH			256

#define REGSTR_PATH_SETUP				TEXT("Software\\Microsoft\\Windows\\CurrentVersion")
#define REGSTR_PATH_SETUP_NT			TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion")
#define REGSTR_PATH_MAIL				TEXT("Software\\Clients\\Mail")
#define REGSTR_PATH_CLIENT				REGSTR_PATH_MAIL TEXT("\\%s\\Protocols\\mailto\\shell\\open\\command")

//#define REGSTR_PATH_SERVICES			TEXT("SYSTEM\\CurrentControlSet\\Services")
//#define REGSTR_VALUE_DISPLAYNAME		TEXT("DisplayName")
//#define REGSTR_VALUE_DESCRIPTION		TEXT("Description")
#define REGSTR_DATA_DISPLAYNAME		    TEXT("Automatyczne zamykanie systemu")
#define REGSTR_DATA_DESCRIPTION		    TEXT("Us³uga wy³¹cza system o okreœlonej porze dnia lub po up³ywie okreœlonego czasu bezczynnoœci.")

#define REGSTR_VAL_REGOWNER				TEXT("RegisteredOwner")

#define LPCTSTR_MAILTO					TEXT("mailto:")
#define LPCTSTR_EMAIL					TEXT("x-y@o2.pl")
#define LPCTSTR_WWW						TEXT("http://ms.netmark.pl/")

#define LOGINFO_BUFFER_SIZE				1024

typedef struct _IDLE_OPTION {
    TCHAR achTime[16*sizeof(TCHAR)];
    DWORD dwTime;
} IDLE_OPTION, *LPIDLE_OPTION;
#define IDLE_OPTION_SIZE			    sizeof(IDLE_OPTION)

#define IDLE_OPTION_ITEMS			    6
IDLE_OPTION sIdleOption[IDLE_OPTION_ITEMS] = {
    {TEXT("15 sekund"), 15000},
    {TEXT("30 sekund"), 30000},
    {TEXT("45 sekund"), 45000},
    {TEXT("1 minuta"), 60000},
    {TEXT("1,5 minuty"), 90000},
    {TEXT("2 minuty"), 120000}
};/*
IDLE_OPTION sIdleOption[IDLE_OPTION_ITEMS] = {
    {TEXT("15 minut"), 90000},
    {TEXT("30 minut"), 180000},
    {TEXT("45 minut"), 270000},
    {TEXT("1 godzina"), 360000},
    {TEXT("1,5 godziny"), 540000},
    {TEXT("2 godziny"), 720000}
};*/

typedef struct _MSGDATA_PACKET {
    DWORD dwCode;
    BOOL fSend;
} MSGDATA_PACKET, *LPMSGDATA_PACKET;
#define MSGDATA_PACKET_SIZE			    sizeof(MSGDATA_PACKET)

#define SHDWNMODE_STOPPED               0x00000000
#define SHDWNMODE_TIME                  0x00000001
#define SHDWNMODE_IDLE                  0x00000002
#define SHDWNMODE_IDLETIME              SHDWNMODE_TIME | SHDWNMODE_IDLE

// Komnikaty serwis <-> menad¿er
#define SHDWNCODE_CANCEL                0x00000000 // Anulowanie zamkania systemu
#define SHDWNCODE_OK                    0x00000001 // 
#define SHDWNCODE_SHUTDOWN              0x00000002 // Zamykanie systemu
#define SHDWNCODE_WAIT                  0x00000003 // Informacja o od³o¿eniu czasu zamkniêcia systemu
#define SHDWNCODE_IDLE                  0x00000004 // 
#define SHDWNCODE_SHOULDBEDOWN          0x00000005 // Komputer powinien byæ o tej porze wy³¹czony
#define SHDWNCODE_LOGOFF                0x00000006 // U¿ytkownik nie jest zalogowany
#define SHDWNCODE_ERROR                 0x00000007 // 
#define SHDWNCODE_STOP                  0x00000008 // 
#define SHDWNCODE_WARNING               0x00000009 // 
#define SHDWNCODE_SUSPEND               0x0000000a // 
#define SHDWNCODE_START                 0x0000000b // 
#define SHDWNCODE_TEST	                0x0000000c // 
#define SHDWNCODE_NEWCLIENT             0x0000000d // 


#ifdef __cplusplus
    #define DLLEXPORT extern "C" __declspec(dllexport)
    #define DLLIMPORT extern "C" __declspec(dllimport)
#else
    #define DLLEXPORT __declspec(dllexport)
    #define DLLIMPORT __declspec(dllimport)
#endif

#define SHDWNHK_CALL __stdcall
