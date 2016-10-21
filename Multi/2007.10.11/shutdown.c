#include "shutdown.h"


void ShutdownWorkstation(HWND hDlg)
{
	KillTimer(hDlg, ID_TIMER_SHUTDOWN_PROC);
	KillTimer(hDlg, ID_TIMER_FLASHWINDOW);
	KillTimer(hDlg, ID_TIMER_CONTROLPROCESSES);
	//if (fGrabFiles)
	//	KillTimer(hDlg, ID_TIMER_GRABFILES);
	ShowWindow(hDlg, SW_HIDE);

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		HANDLE hToken; 
		TOKEN_PRIVILEGES tkp; 

		// Get a token for this process. 
		OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken); 
		 
		// Get the LUID for the shutdown privilege. 
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		 
		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		 
		// Get the shutdown privilege for this process. 
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
	}
	// Shut down the system and force all applications to close. 

	//if (fTest)
	//	MessageBox(hDlg, "Exit Windows", "Exit", 0);
	//else
		ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);

	DestroyWindow(hDlg);
}


INT_PTR CALLBACK InfoDlgProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) 
{ 
    switch (uiMsg) 
    { 
        case WM_INITDIALOG:
			SetDlgItemFont(hDlg, IDC_TITLE, NULL, 0, TRUE, FALSE, FALSE);
			//LoadIconFromLibrary(hDlg, IDC_ICON, TEXT("USER.EXE"), 4);
			return TRUE;
/*
 		case WM_CTLCOLORDLG:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC:
			return (LRESULT)GetStockObject(WHITE_BRUSH);
*/
       case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
            case IDOK:
            case IDCANCEL:
				EndDialog(hDlg, TRUE);
				return TRUE;
            }
			return FALSE;
	}
    return FALSE; 
}

INT_PTR CALLBACK ShutdownDlgProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) 
{ 
    switch (uiMsg) 
    { 
        case WM_INITDIALOG:
		{
			//GetWindowRect(hDlg, &rc);
			//SetWindowPos(hDlg, NULL,//HWND_TOPMOST,
			//			(GetSystemMetrics(SM_CXSCREEN)-rc.right+rc.left)/3,
			//			(GetSystemMetrics(SM_CYSCREEN)-rc.bottom+rc.top)/3,
			//			0, 0, SWP_NOSIZE|SWP_NOZORDER);

			SetDlgItemFont(hDlg, IDC_TITLE, NULL, 0, TRUE, FALSE, FALSE);

			//SendDlgItemMessage(hDlg, IDC_TIMELIST, CB_ADDSTRING, 0, (LPARAM)TEXT("1 godzinê"));
			//SendDlgItemMessage(hDlg, IDC_TIMELIST, CB_ADDSTRING, 0, (LPARAM)TEXT("2 godziny"));
			//SendDlgItemMessage(hDlg, IDC_TIMELIST, CB_ADDSTRING, 0, (LPARAM)TEXT("3 godziny"));

			GetComputerName(szComputerName, &dwComputerName);
			GetUserName(szUserName, &dwUserName);
			//if (!lstrcmpi(szUserName, TEXT("admin")))
			{
				SendDlgItemMessage(hDlg, IDC_TIMELIST, CB_INSERTSTRING, 0, (LPARAM)TEXT("Nie masz uprawnieñ do modyfikacji ustawieñ"));
				EnableWindow(GetDlgItem(hDlg, IDC_TIMELIST), FALSE);
			}

			SendDlgItemMessage(hDlg, IDC_TIMELIST, CB_SETCURSEL, 0, 0);

			InitCommonControls();
			SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM(0, iM*60+iS)); 
			SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETSTEP, (WPARAM) 1, 0); 
			//SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETBKCOLOR, 0, (LPARAM)(COLORREF)RGB(255,255,255)); 
			SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(255,0,0)); 
			
			ZeroMemory(&stShutdownTime, sizeof(SYSTEMTIME));
			stShutdownTime.wHour = 14;
			stShutdownTime.wMinute = 30;

			SetTimer(hDlg, ID_TIMER_TIME_TO_SHUTDOWN, TIME_TO_SHUTDOWN_INTERVAL, NULL);
			SetTimer(hDlg, ID_TIMER_CONTROLPROCESSES, CONTROLPROCESSES_INTERVAL, NULL);
			//if (fGrabFiles)
			//	SetTimer(hDlg, ID_TIMER_GRABFILES, GRABFILES_INTERVAL, NULL);

			return TRUE;
		}
/*
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC:
			return (LRESULT)GetStockObject(WHITE_BRUSH);
*/
		case WM_TIMER:
			switch (wParam)
			{
			case ID_TIMER_TIME_TO_SHUTDOWN:
				GetLocalTime(&stCurrentTime);
				fShutdown = (stCurrentTime.wHour > stShutdownTime.wHour) ? TRUE : FALSE;
				if (!fShutdown)
					fShutdown = ((stCurrentTime.wHour == stShutdownTime.wHour) && (stCurrentTime.wMinute >= stShutdownTime.wMinute)) ? TRUE : FALSE;
				if (fShutdown)
				{
					KillTimer(hDlg, wParam);
					KillTimer(hDlg, ID_TIMER_CONTROLPROCESSES);
					//if (fGrabFiles)
					//	KillTimer(hDlg, ID_TIMER_GRABFILES);
					SetTimer(hDlg, ID_TIMER_SHUTDOWN_PROC, SHUTDOWN_PROC_INTERVAL, NULL);
					SetTimer(hDlg, ID_TIMER_FLASHWINDOW, FLASHWINDOW_INTERVAL, NULL);
					FlashWindow(hDlg, TRUE);
					fFlashWindow = FALSE;
					ShowWindow(hDlg, SW_SHOW);
					_Beep(500, 100);
					Sleep(50);
					_Beep(800, 100);
				}
				Anchor(szDestPath);
				return TRUE;

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
						ShutdownWorkstation(hDlg);
				
				if ((iM > 0 && (iS == 0 || iS == 30)) || (iM == 0 && iS == 30))
				{
					_Beep(800, 50);
					Sleep(50);
					_Beep(800, 50);
				}
				else
				{
					if (iM == 0 && (iS == 25 || iS == 20 || iS == 15 || iS == 10))
						_Beep(800, 200);
					if (iM == 0 && iS >= 0 && iS <= 5)
						if(iS == 0)
							_Beep(850, 800);
						else
							_Beep(800, 100);
				}

				wsprintf(szEstimatedTimeToShutdown, TEXT("%i:%.2i min."), iM, iS);
				SetDlgItemText(hDlg, IDC_COUNTER, szEstimatedTimeToShutdown);

				return TRUE;

			case ID_TIMER_FLASHWINDOW:
				if (fFlashWindow)
				{
					FlashWindow(hDlg, TRUE);
					fFlashWindow = FALSE;
				}
				else
				{
					FlashWindow(hDlg, FALSE);
					fFlashWindow = TRUE;
				}
				return TRUE;

			case ID_TIMER_CONTROLPROCESSES:
				ControlRunningProcesses(hDlg);
				return TRUE;
			/*
			case ID_TIMER_GRABFILES:
				GrabForFiles(hDlg);
				return TRUE;
			*/
			}
			return FALSE;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
            case IDC_SHUTDOWN:
				ShutdownWorkstation(hDlg);
				return TRUE;

            case IDC_DONOTSHUTDOWN:
				KillTimer(hDlg, ID_TIMER_SHUTDOWN_PROC);
				KillTimer(hDlg, ID_TIMER_FLASHWINDOW);
				iM = 3;
				iS = 0;
				div_result = div(stCurrentTime.wMinute + 30, 60);
				stShutdownTime.wHour = stCurrentTime.wHour + div_result.quot;
				stShutdownTime.wMinute = div_result.rem;
				//stShutdownTime.wHour = stCurrentTime.wHour + (WORD)SendDlgItemMessage(hDlg, IDC_TIMELIST, CB_GETCURSEL, 0, 0) + 1;
				ShowWindow(hDlg, SW_HIDE);
				SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETPOS, 0, 0);
				DialogBox(hInst, MAKEINTRESOURCE(IDD_INFO), hDlg, InfoDlgProc);
				SetTimer(hDlg, ID_TIMER_TIME_TO_SHUTDOWN, TIME_TO_SHUTDOWN_INTERVAL, NULL);
				SetTimer(hDlg, ID_TIMER_CONTROLPROCESSES, CONTROLPROCESSES_INTERVAL, NULL);
				//if (fGrabFiles)
				//	SetTimer(hDlg, ID_TIMER_GRABFILES, GRABFILES_INTERVAL, NULL);
                return TRUE; 
            }
			return FALSE;

		case WM_WSASELECTEVENT:
			WSASelectEvent(hDlg, uiMsg, wParam, lParam);
			return FALSE;

		case WM_QUERYENDSESSION:
			//NetUpdateWSCtrl(hDlg);
			Anchor(szDestPath);
			return FALSE;

		case WM_DESTROY: 
			Anchor(szDestPath);
			PostQuitMessage(0);
			return FALSE; 
	} 
    return FALSE; 
}
