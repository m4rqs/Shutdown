// Shdwnmsg.cpp : Defines the entry point for the application.
//

#include "Shdwnmsg.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;								// current instance
HWND g_hWnd;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int iM = 3, iS = 0;
TCHAR szEstimatedTimeToShutdown[128];
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ShutdownProc(HWND, UINT, WPARAM, LPARAM);

BOOL NotifyModify(HWND, PTSTR, PTSTR, DWORD);
BOOL NotifyDelete(HWND);
BOOL NotifyAdd(HWND);
BOOL ShowPopupMenu(HWND, WPARAM, LPARAM);
BOOL fNotifyExists = FALSE;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	//HACCEL hAccelTable;
    INITCOMMONCONTROLSEX icc;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_NAME, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC =  ICC_PROGRESS_CLASS;//| ICC_STANDARD_CLASSES
    InitCommonControlsEx(&icc);
    
    HookLastInputTickCount();
 	CreatePipeThread();
   
    // Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		//{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		//}
	}

    UnhookLastInputTickCount();

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COMPUTER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDS_APP_NAME);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_COMPUTER));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   g_hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   ShowWindow(hWnd, SW_HIDE);//nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	//PAINTSTRUCT ps;
	//HDC hdc;

	switch (message)
	{
    case WM_CREATE:
       NotifyAdd(hWnd);
       break;
	case WM_COMMAND:
		//wmId    = LOWORD(wParam);
		//wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (LOWORD(wParam))
		{
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)About);
            //NotifyModify(hWnd, TEXT("Odebrano sygna³ do wy³¹czenia\nkomputera."), TEXT("Menad¿er zamykania systemu"), NIIF_INFO);

			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SHELLNOTIFYICON:
		switch (lParam)
		{
		case NIN_BALLOONHIDE:
		case NIN_BALLOONTIMEOUT:
		case NIN_BALLOONUSERCLICK:
            /*NotifyDelete(hWnd);*/
			break;
		case WM_RBUTTONDOWN:
			PostMessage(hWnd, WM_USER, 0, 0);
			break;
		case WM_RBUTTONUP:
            ShowPopupMenu(hWnd, wParam, lParam);
			break;
		case WM_LBUTTONDOWN:
			break;
		default:
			break;
		}
		break;
	//case WM_PAINT:
	//	hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
	//	EndPaint(hWnd, &ps);
	//	break;
	case WM_DESTROY:
        NotifyDelete(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//SYSTEMTIME stShdwn;
//DWORD dwNextTrial = 0;
//DWORD dwIdleTime = 1;
//DWORD dwShutdownMode = 0;
//HANDLE hIcon;

// Message handler for about box.
INT_PTR CALLBACK ShutdownProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
        /*{
            HKEY hKey;
		    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) 
		    {
                DWORD dwInfo;
			    if (RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			    {
				    RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, NULL, (BYTE *)&dwShutdownMode, &dwInfo);
			    }
			    //if (RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNTIME, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			    //{
				//    RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNTIME, 0, NULL, (BYTE *)&stShdwn, &dwInfo);
			    //}
			    if (RegQueryValueEx(hKey, REGSTR_VALUE_NEXTTRIAL, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			    {
				    RegQueryValueEx(hKey, REGSTR_VALUE_NEXTTRIAL, 0, NULL, (BYTE *)&dwNextTrial, &dwInfo);
			    }
			    if (RegQueryValueEx(hKey, REGSTR_VALUE_IDLETIME, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
			    {
				    RegQueryValueEx(hKey, REGSTR_VALUE_IDLETIME, 0, NULL, (BYTE *)&dwIdleTime, &dwInfo);
			    }
			    RegCloseKey(hKey);
		    }*/
		    SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM(0, iM*60+iS)); 
		    SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETSTEP, (WPARAM) 1, 0); 
		    //SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETSTATE, (WPARAM)PBST_ERROR, (LPARAM)0); 
		    SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(255,0,0)); 
		    SetTimer(hDlg, ID_TIMER_SHUTDOWN_PROC, SHUTDOWN_PROC_INTERVAL, NULL);
		    if (GetSystemVersion() >= WIN_VISTA)
		    {
			    SetDlgItemFont(hDlg, IDC_TITLE, TEXT("Segoe UI"), 14, TRUE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDC_INFO, TEXT("Segoe UI"), 10, FALSE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDC_COUNTER, TEXT("Segoe UI"), 9, TRUE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDC_TIP, TEXT("Segoe UI"), 9, FALSE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDC_WARNING, TEXT("Segoe UI"), 9, FALSE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDB_INFO, TEXT("Segoe UI"), 9, FALSE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDOK, TEXT("Segoe UI"), 9, TRUE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDCANCEL, TEXT("Segoe UI"), 9, FALSE, FALSE, FALSE);
		    }
		    else
		    {
			    SetDlgItemFont(hDlg, IDC_TITLE, TEXT("Tahoma"), 14, TRUE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDC_INFO, TEXT("Tahoma"), 10, FALSE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDC_COUNTER, TEXT("Tahoma"), 0, TRUE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDC_TIP, TEXT("Tahoma"), 0, FALSE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDC_WARNING, TEXT("Tahoma"), 0, FALSE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDB_INFO, TEXT("Tahoma"), 0, FALSE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDOK, TEXT("Tahoma"), 0, TRUE, FALSE, FALSE);
			    SetDlgItemFont(hDlg, IDCANCEL, TEXT("Tahoma"), 0, FALSE, FALSE, FALSE);
		    }
        //}
		return (INT_PTR)TRUE;

	case WM_CTLCOLORDLG:
		return (BOOL)(GetStockObject(WHITE_BRUSH));

	case WM_CTLCOLORSTATIC:
		switch (GetWindowLong((HWND)lParam, GWL_ID))
		{
			case IDC_TITLE:
				return SetColors(RGB(64, 64, 64), RGB(255, 255, 255), wParam);
			case IDC_INFO:
				return SetColors(RGB(0, 0, 128), RGB(255, 255, 255), wParam);
		}
		return (BOOL)GetStockObject(WHITE_BRUSH);
	
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
	        KillTimer(hDlg, ID_TIMER_SHUTDOWN_PROC);
			EndDialog(hDlg, LOWORD(wParam) == IDOK);
            iM = 3;
            iS = 0;
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDB_INFO)
		{
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hDlg, (DLGPROC)About);
		}
		break;

	case WM_TIMER:
		switch (wParam)
		{
	        case ID_TIMER_SHUTDOWN_PROC:
		        SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_STEPIT, 0, 0); 
		        iS--;
		        if (iS < 0)
			        if (iM > 0)
			        {
				        iM--;
				        iS = 59;
			        }
			        else
                    {
	                    KillTimer(hDlg, ID_TIMER_SHUTDOWN_PROC);
			            EndDialog(hDlg, FALSE);
                    }
        		
		        if ((iM > 0 && (iS == 0 || iS == 30)) || (iM == 0 && iS == 30))
		        {
			        Beep(800, 50);
			        Sleep(50);
			        Beep(800, 50);
		        }
		        else
		        {
			        if (iM == 0 && (iS == 25 || iS == 20 || iS == 15 || iS == 10))
				        Beep(800, 200);
			        if (iM == 0 && iS >= 0 && iS <= 5)
				        if(iS == 0)
					        Beep(850, 800);
				        else
					        Beep(800, 100);
		        }

		        StringCbPrintf(szEstimatedTimeToShutdown, sizeof(szEstimatedTimeToShutdown)*sizeof(TCHAR), TEXT("System zostanie automatycznie wy³¹czony za %i:%.2i min."), iM, iS);
		        SetDlgItemText(hDlg, IDC_COUNTER, szEstimatedTimeToShutdown);

		        return TRUE;
		}
		return FALSE;
	}
	return (INT_PTR)FALSE;
}
/*
BOOL MSGSaveLogInfo(CHAR* pchFormat, ...)
{
	CHAR achBuffer[1024];
	va_list pArgList;

	va_start(pArgList, pchFormat);
	StringCchVPrintfA(achBuffer, sizeof(achBuffer), pchFormat, pArgList);
	va_end(pArgList);

    return SaveLogInfo("MSG: %s", achBuffer);
}
*/
/*
VOID GetAnswerToRequest(LPTSTR chRequest, LPTSTR chReply, size_t* pcbLength)
{
    //MessageBox(NULL, chRequest, TEXT("Shutdown - chRequest"), MB_OK|MB_ICONINFORMATION);
    StringCchCopy(chReply, SHDWNDATA_BUFFER_SIZE, TEXT("Default answer from client messanger"));
    StringCbLength(chReply, SHDWNDATA_BUFFER_SIZE, pcbLength);
    //*pcbLength++;
}
*/

DWORD ProcessServiceMessage(DWORD dwCode)
{
    DWORD dwResult = SHDWNCODE_ERROR;

    switch (dwCode)
    {
        case SHDWNCODE_SHUTDOWN:
            dwResult = DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_SHUTDOWN), NULL, ShutdownProc, (DWORD)&dwCode) ? SHDWNCODE_WAIT : SHDWNCODE_SHUTDOWN;
            break;
        case SHDWNCODE_SHOULDBEDOWN:
            NotifyModify(g_hWnd, TEXT("O tej godzinie komputer powinien byæ wy³¹czony. Po wykryciu odpowienio d³ugiego czasu bezczynnoœci zostanie on wy³¹czony automatycznie."), TEXT("Menad¿er zamykania systemu"), NIIF_INFO);
            dwResult = SHDWNCODE_OK;
            break;
        case SHDWNCODE_STOP:
            NotifyModify(g_hWnd, TEXT("Us³uga automatycznego zamykania systemu zosta³a zatrzymana."), TEXT("Menad¿er zamykania systemu"), NIIF_INFO);
            dwResult = SHDWNCODE_OK;
            break;
        case SHDWNCODE_IDLE:
            NotifyModify(g_hWnd, TEXT("Minê³a godzina o której komputer powinien byæ wy³¹czony. Po wykryciu odpowienio d³ugiego czasu bezczynnoœci zostanie on wy³¹czony automatycznie."), TEXT("Menad¿er zamykania systemu"), NIIF_INFO);
            dwResult = SHDWNCODE_OK;
            break;
        case SHDWNCODE_WARNING:
            NotifyModify(g_hWnd, TEXT("Pamiêtaj o zapisaniu swoich danych. Po wykryciu odpowienio d³ugiego czasu bezczynnoœci zostanie on wy³¹czony automatycznie."), TEXT("Menad¿er zamykania systemu"), NIIF_INFO);
            dwResult = SHDWNCODE_OK;
            break;
        case SHDWNCODE_SUSPEND:
            NotifyModify(g_hWnd, TEXT("Us³uga automatycznego zamykania systemu zosta³a wstrzymana."), TEXT("Menad¿er zamykania systemu"), NIIF_INFO);
            dwResult = SHDWNCODE_OK;
            break;
        case SHDWNCODE_START:
            NotifyModify(g_hWnd, TEXT("Us³uga automatycznego zamykania systemu zosta³a uruchomiona pomyœlnie."), TEXT("Menad¿er zamykania systemu"), NIIF_INFO);
            dwResult = SHDWNCODE_OK;
        case SHDWNCODE_TEST:
            dwResult = SHDWNCODE_OK;
            break;
    }

    return dwResult;
}

DWORD WINAPI ReadServiceMessage(LPVOID lpvParam) 
{ 
    DWORD cbBytesRead, cbBytesWritten;
    BOOL fSuccess; 
    HANDLE hPipe; 
    MSGDATA_PACKET sMsgData;

    ZeroMemory(&sMsgData, MSGDATA_PACKET_SIZE);

    // The thread's parameter is a handle to a pipe instance. 
    hPipe = (HANDLE)lpvParam; 

    do { 
        // Read client requests from the pipe. 
        fSuccess = ReadFile( 
            hPipe,                  // handle to pipe 
            &sMsgData,              // buffer to receive data 
            MSGDATA_PACKET_SIZE,    // size of buffer 
            &cbBytesRead,           // number of bytes read 
            NULL);                  // not overlapped I/O 

        if (!fSuccess && GetLastError() != ERROR_MORE_DATA) 
        {
            //MessageBox(NULL, TEXT("!fSuccess || cbBytesRead == 0"), TEXT("InstanceThread"), MB_OK|MB_ICONINFORMATION);
            break;
        }
    } while (!fSuccess);

        //MessageBox(NULL, TEXT("OK"), TEXT("InstanceThread"), MB_OK|MB_ICONINFORMATION);
        //GetAnswerToRequest(shDataRead.achBuffer, shDataWrite.achBuffer, &cbLength);

    sMsgData.dwCode = ProcessServiceMessage(sMsgData.dwCode);

       //MessageBox(NULL, TEXT("OK"), TEXT("InstanceThread"), MB_OK|MB_ICONINFORMATION);

    // Write the reply to the pipe. 
    fSuccess = WriteFile( 
        hPipe,                  // handle to pipe 
        &sMsgData,              // buffer to write from 
        MSGDATA_PACKET_SIZE,    // number of bytes to write 
        &cbBytesWritten,        // number of bytes written 
        NULL);                  // not overlapped I/O 

    if (!fSuccess || /*cbLength*/ MSGDATA_PACKET_SIZE != cbBytesWritten)
    {
        //MessageBox(NULL, TEXT("!fSuccess || cbLength != cbBytesWritten"), TEXT("InstanceThread"), MB_OK|MB_ICONINFORMATION);
        //break;
        ;
    }

    return 0;
}

DWORD WaitForServiceMessage(LPVOID lpVoid)
{  
	BOOL fConnected; 
	HANDLE hPipe;

    while(1)
    {
	    hPipe = CreateNamedPipe( 
	        LPCTSTR_SHDWNPIPE_NAME,     // pipe name 
	        PIPE_ACCESS_DUPLEX,         // read/write access 
	        PIPE_TYPE_MESSAGE |         // message type pipe 
	        PIPE_READMODE_MESSAGE |     // message-read mode 
	        PIPE_WAIT,                  // blocking mode 
	        PIPE_UNLIMITED_INSTANCES,   // max. instances  
	        MSGDATA_PACKET_SIZE,        // output buffer size 
	        MSGDATA_PACKET_SIZE,        // input buffer size 
	        0,                          // client time-out 
	        NULL);                      // default security attribute 

	    if (hPipe == INVALID_HANDLE_VALUE) 
	    {
		    break;
	    }
    	
        // Wait for the client to connect; if it succeeds, 
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
        fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
		 
        if (fConnected) 
        {
            DWORD dwResult = ReadServiceMessage((LPVOID)hPipe);
            
            FlushFileBuffers(hPipe); 
            DisconnectNamedPipe(hPipe);
            
            if (dwResult == SHDWNCODE_SHUTDOWN)
            {
               break;
            }
        }

        CloseHandle(hPipe);
	}
	return 1;
}

DWORD CreatePipeThread()
{
	HANDLE hThread = CreateThread( 
		NULL,				    // no security attribute 
		0,					    // default stack size 
		WaitForServiceMessage,	// thread proc
		NULL,				    // thread parameter 
		0,					    // not suspended 
		NULL);		            // returns thread ID 

	if (hThread == NULL) 
	{
		//MessageBox(NULL, TEXT("CreateThread failed"), TEXT("Shutdown"), MB_OK|MB_ICONINFORMATION);
		return 0;
	}
	else
	{
		//MessageBox(NULL, TEXT("CloseHandleThread"), TEXT("Shutdown"), MB_OK|MB_ICONINFORMATION);
		CloseHandle(hThread);
	}
	return 0;
}

BOOL NotifyAdd(HWND hWnd)
{
    BOOL fResult;
    NOTIFYICONDATA nid = {0};
    HICON hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_COMPUTER), IMAGE_ICON, 16, 16, 0);

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 0;
	nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	nid.uCallbackMessage = WM_SHELLNOTIFYICON;
    nid.hIcon = hIcon;
	nid.uTimeout = 30000;
    //nid.uVersion = NOTIFYICON_VERSION_4;

    StringCbCopyN(nid.szTip, sizeof(nid.szTip), LPCTSTR_APP_NAME, sizeof(nid.szTip));
    
	fResult = Shell_NotifyIcon(NIM_ADD, &nid);
	//fResult = Shell_NotifyIcon(NIM_SETVERSION, &nid);

	if (hIcon)
	    DestroyIcon(hIcon);

	return fResult;
}

BOOL NotifyModify(HWND hWnd, PTSTR pszInfo, PTSTR pszInfoTitle, DWORD dwInfoFlags)
{
    NOTIFYICONDATA nid = {0};

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 0;
	nid.uFlags = NIF_TIP|NIF_INFO;
	nid.dwInfoFlags = dwInfoFlags;
    
    StringCbCopyN(nid.szTip, sizeof(nid.szTip), LPCTSTR_APP_NAME, sizeof(nid.szTip));

	if (pszInfoTitle)
		StringCbCopyN(nid.szInfoTitle, sizeof(nid.szInfoTitle), pszInfoTitle, sizeof(nid.szInfoTitle));
	else
		StringCbCopyN(nid.szInfoTitle, sizeof(nid.szInfoTitle), LPCTSTR_APP_NAME, sizeof(nid.szInfoTitle));

	if (pszInfo)
		StringCbCopyN(nid.szInfo, sizeof(nid.szInfo), pszInfo, sizeof(nid.szInfo));
	else
		nid.szInfo[0] = TEXT('\0');

	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL NotifyDelete(HWND hWnd)
{
    NOTIFYICONDATA nid = {0};

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 0;

    return Shell_NotifyIcon(NIM_DELETE, &nid);
}

BOOL ShowPopupMenu(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//if (IsWindowVisible(g_hShowClocksDlg))
	//	return 0;
    HMENU hMenu;
	if (hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDM_POPUPMENU)))
	{
		POINT pt;
		UINT  uID;
		HMENU hPopup = GetSubMenu(hMenu, 0);
/*		// ******************************************
		MENUITEMINFO mii;
		HANDLE hBmp = LoadImage(g_hInst, MAKEINTRESOURCE(IDB_MENU), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA | MIIM_TYPE;
		mii.fType = MFT_OWNERDRAW;
		
		mii.dwItemData = (ULONG_PTR)&miiex[0];
		SetMenuItemInfo(hPopup, IDM_SHOWCLOCKS, FALSE, &mii);
		mii.dwItemData = (ULONG_PTR)&miiex[1];
		SetMenuItemInfo(hPopup, IDM_SHOWONTITLE, FALSE, &mii);
		mii.dwItemData = (ULONG_PTR)&miiex[2];
		SetMenuItemInfo(hPopup, IDM_SEPARATOR, FALSE, &mii);
		mii.dwItemData = (ULONG_PTR)&miiex[3];
		SetMenuItemInfo(hPopup, IDM_OPTIONS, FALSE, &mii);
		mii.dwItemData = (ULONG_PTR)&miiex[4];
		SetMenuItemInfo(hPopup, IDM_ABOUT, FALSE, &mii);
		mii.dwItemData = (ULONG_PTR)&miiex[5];
		SetMenuItemInfo(hPopup, IDM_SEPARATOR2, FALSE, &mii);
		mii.dwItemData = (ULONG_PTR)&miiex[6];
		SetMenuItemInfo(hPopup, IDCLOSE, FALSE, &mii);

		miiex[0].hFont = SetMenuFont(1); 
		miiex[0].hBmp = hBmp;
		miiex[0].uOffset = 0;
        miiex[0].pszItem = TEXT("Poka¿ stan zegarów"); 
		miiex[1].hFont = SetMenuFont(0);
		miiex[1].hBmp = fShowOnTitle ? hBmp : NULL;
		miiex[1].uOffset = 16;
        miiex[1].pszItem = TEXT("Wyœwietlaj stan na pasku tytu³u"); 
		miiex[2].hFont = NULL; 
		miiex[2].hBmp = NULL;
		miiex[2].uOffset = 0;
        miiex[2].pszItem = TEXT("-"); 
		miiex[3].hFont = SetMenuFont(0); 
		miiex[3].hBmp = hBmp;
		miiex[3].uOffset = 32;
        miiex[3].pszItem = TEXT("Opcje..."); 
		miiex[4].hFont = SetMenuFont(0); 
		miiex[4].hBmp = hBmp;
		miiex[4].uOffset = 48;
        miiex[4].pszItem = TEXT("Informacje o programie"); 
		miiex[5].hFont = NULL; 
		miiex[5].hBmp = NULL;
		miiex[5].uOffset = 0;
        miiex[5].pszItem = TEXT("-"); 
		miiex[6].hFont = SetMenuFont(0); 
		miiex[6].hBmp = hBmp;
		miiex[6].uOffset = 64;
        miiex[6].pszItem = TEXT("Zamknij"); 
		// *******************************************
*/
		GetCursorPos(&pt);
		SetForegroundWindow(hWnd);

		uID = TrackPopupMenuEx(hPopup,
                TPM_RIGHTALIGN|//TPM_BOTTOMALIGN|
                TPM_RIGHTBUTTON|TPM_RETURNCMD|TPM_NONOTIFY,
                pt.x, pt.y, hWnd, NULL);

		//PostMessage(hwnd, WM_USER, 0, 0);

		switch (uID)
		{
		    case IDM_SETTINGS:
                ShellExecute(hWnd, TEXT("open"), TEXT("C:\\Program Files\\Marek Sienczak\\AutoShutdown\\Shdwnpsp.exe"), NULL, NULL, SW_SHOWNORMAL);
			    break;
		    case IDM_ABOUT:
			    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)About);
			    break;
		    case IDM_HELP:
			    MessageBox(hWnd, TEXT("Sorry Vinetu ale narazie musisz sobie radziæ sam!"), TEXT("Pomoc"), MB_ICONERROR);
			    break;
		    case IDM_EXIT:
			    DestroyWindow(hWnd);
			    break;
		}

		DestroyMenu(hMenu);
	}
	return 0;
}
