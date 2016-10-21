
#include "Shdwncpl.h"
 

/////////////////////////////////////////////////////////////////////
//
//  FUNCTION: PropSheetPageCallback(HWND, UINT, LPPROPSHEETPAGE)
//
//  PURPOSE: Callback  procedure for the property page
//
//  PARAMETERS:
//    hWnd      - Reserved (will always be NULL)
//    uMessage  - Action flag: Are we being created or released
//    ppsp      - The page that is being created or destroyed
//
//  RETURN VALUE:
//
//    Depends on message. 
//
//    For PSPCB_CREATE it's TRUE to let the page be created
//    or false to prevent it from being created.  
//    For PSPCB_RELEASE the return value is ignored.
//
//
UINT CALLBACK
PropSheetPageCallback(HWND hWnd,
					  UINT uMessage,
					  LPPROPSHEETPAGE ppsp)
{
    switch(uMessage)
    {
        case PSPCB_CREATE:
            return TRUE;

        case PSPCB_RELEASE:
            if (ppsp->lParam) 
            {
               ((LPCSHELLEXT)(ppsp->lParam))->Release();
            }
            return TRUE; 
    }
    return TRUE;
}
/////////////////////////////////////////////////////////////////////
//
//  FUNCTION: PropSheetPageDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Callback dialog procedure for the property page
//
//  PARAMETERS:
//    hDlg      - Dialog box window handle
//    uMessage  - current message
//    wParam    - depends on message
//    lParam    - depends on message
//
//  RETURN VALUE:
//
//    Depends on message.  In general, return TRUE if we process it.
//
//
BOOL CALLBACK
PropSheetPageDlgProc(HWND hDlg,
					 UINT uMessage,
					 WPARAM wParam,
					 LPARAM lParam)
{
    //LPPROPSHEETPAGE psp = (LPPROPSHEETPAGE)GetWindowLong(hDlg, DWL_USER);
    //int  iID;
    //LPCSHELLEXT lpcs;
	DWORD dwInfo;
	HKEY hKey;
	SYSTEMTIME st = {2008,1,2,1,14,30,0,0};
	BOOL fAdminMode = false;

    switch (uMessage)
    {
        //
        // When the shell creates a dialog box for a property sheet page,
        // it passes the pointer to the PROPSHEETPAGE data structure as
        // lParam. The dialog procedures of extensions typically store it
        // in the DWL_USER of the dialog box window.
        //
        case WM_INITDIALOG:
            //SetWindowLong(hDlg, DWL_USER, lParam);

            //psp = (LPPROPSHEETPAGE)lParam;

			SendDlgItemMessage(hDlg, IDC_DTP_TIME, DTM_SETFORMAT, 0L, (LPARAM)TEXT("HH:mm"));
			
			for (int i=0; i<IDLE_OPTION_ITEMS; i++)
            {
				SendDlgItemMessage(hDlg, IDC_CB_TIME, CB_ADDSTRING, 0L, (LPARAM)sIdleOption[i].achTime);
				SendDlgItemMessage(hDlg, IDC_CB_IDLE, CB_ADDSTRING, 0L, (LPARAM)sIdleOption[i].achTime);
            }

			// Wczytanie domyœlnych opcji -----------------------
			SendDlgItemMessage(hDlg, IDC_DTP_TIME, DTM_SETSYSTEMTIME, (WPARAM)(DWORD)GDT_VALID, (LPARAM)(LPSYSTEMTIME)&st);
			SendDlgItemMessage(hDlg, IDC_CB_TIME, CB_SETCURSEL, 0L, 0L);
			SendDlgItemMessage(hDlg, IDC_CB_IDLE, CB_SETCURSEL, 1L, 0L);
			SendDlgItemMessage(hDlg, IDC_RB_SHDWNNO, BM_CLICK, 0L, 0L);
			//---------------------------------------------------

			// Wczytanie opcji z rejestru -----------------------
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) 
			{
				DWORD dwData;
				if (RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
				{
					RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, NULL, (BYTE *)&dwData, &dwInfo);
                    DWORD dwServiceState = QueryServiceState(LPCTSTR_SERVICE_NAME);
                    if (dwData && (dwServiceState != SERVICE_RUNNING))
                        SendDlgItemMessage(hDlg, IDC_RB_SHDWNNO, BM_CLICK, TRUE, 0L);
                    else
                        SendDlgItemMessage(hDlg, IDC_RB_SHDWNNO + (dwData == (SHDWNMODE_IDLETIME) ? SHDWNMODE_TIME : dwData), BM_CLICK, TRUE, 0L);
					//if (dwData)
					//	ServiceStart(LPCTSTR_SERVICE_NAME);
				}
				if (RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNTIME, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
				{
					RegQueryValueEx(hKey, REGSTR_VALUE_SHUTDOWNTIME, 0, NULL, (BYTE *)&st, &dwInfo);
					SendDlgItemMessage(hDlg, IDC_DTP_TIME, DTM_SETSYSTEMTIME, (WPARAM)(DWORD)GDT_VALID, (LPARAM)(LPSYSTEMTIME)&st);
				}
				if (RegQueryValueEx(hKey, REGSTR_VALUE_NEXTTRIAL, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
				{
					RegQueryValueEx(hKey, REGSTR_VALUE_NEXTTRIAL, 0, NULL, (BYTE *)&dwData, &dwInfo);
					SendDlgItemMessage(hDlg, IDC_CB_TIME, CB_SETCURSEL, dwData, 0L);
				}
				if (RegQueryValueEx(hKey, REGSTR_VALUE_IDLETIME, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
				{
					RegQueryValueEx(hKey, REGSTR_VALUE_IDLETIME, 0, NULL, (BYTE *)&dwData, &dwInfo);
					SendDlgItemMessage(hDlg, IDC_CB_IDLE, CB_SETCURSEL, dwData, 0L);
				}
				if (RegQueryValueEx(hKey, REGSTR_VALUE_ADMINMODE, 0, NULL, NULL, &dwInfo) == ERROR_SUCCESS)
				{
					RegQueryValueEx(hKey, REGSTR_VALUE_ADMINMODE, 0, NULL, (BYTE *)&dwData, &dwInfo);
					fAdminMode = (BOOL)dwData;
					for (int i=0; i<OPTION_ITEMS_COUNT; i++)
						for (int j=1; j<=aiOption[i][0]; j++)
							EnableWindow(GetDlgItem(hDlg, aiOption[i][j]), fAdminMode);
				}
				RegCloseKey(hKey);
			}
			//---------------------------------------------------
 			break;

        case WM_DESTROY:
            //RemoveProp(hDlg, TEXT("SHDWN_ID"));
            break;

        case WM_COMMAND:
			switch (HIWORD(wParam))
			{
				case CBN_SELCHANGE:
				case BN_CLICKED:
					PropSheet_Changed(GetParent(hDlg), hDlg);
					break;

				default:
					break;
			}
            switch (LOWORD(wParam))
            {
                case IDC_RB_SHDWNTIME:
                case IDC_RB_SHDWNIDLE:
                case IDC_RB_SHDWNNO:
					SendDlgItemMessage(hDlg, LOWORD(wParam), BM_SETCHECK, TRUE, 0L);
					for (int i=0; i<OPTION_ITEMS_COUNT; i++)
					{
						BOOL fEnable = LOWORD(wParam) == aiOption[i][1];
						for (int j=2; j<=aiOption[i][0]; j++)
							EnableWindow(GetDlgItem(hDlg, aiOption[i][j]), fEnable);
					}
                    //SetProp(hDlg, TEXT("SHDWN_ID"), (HANDLE)lParam);
                    break;

                case IDC_DTP_TIME:
                    //SetProp(hDlg, TEXT("SHDWN_ID"), (HANDLE)lParam);
                    break;

                case IDC_CB_TIME:
                case IDC_CB_IDLE:
                    //SetProp(hDlg, TEXT("SHDWN_ID"), (HANDLE)lParam);
                    break;

                default:
                    break;
            }
            break;

        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {
 				case DTN_DATETIMECHANGE:
					PropSheet_Changed(GetParent(hDlg), hDlg);
                    break;

                case PSN_SETACTIVE:
                    break;

                case PSN_APPLY:
                    //User has clicked the OK or Apply button so we'll
                    //update the information in the registry
                    //lpcs = (LPCSHELLEXT)psp->lParam;
                    //iID  = GetDlgCtrlID((HWND)GetProp(hDlg, TEXT("SHDWN_ID")));
					if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS)
					{
						DWORD dwData;
						SendDlgItemMessage(hDlg, IDC_DTP_TIME, DTM_GETSYSTEMTIME, 0L, (LPARAM)(LPSYSTEMTIME)&st);
						RegSetValueEx(hKey, REGSTR_VALUE_SHUTDOWNTIME, 0, REG_BINARY, (CONST BYTE*)&st, sizeof(SYSTEMTIME));
						dwData = SendDlgItemMessage(hDlg, IDC_CB_TIME, CB_GETCURSEL, 0L, 0L);
						RegSetValueEx(hKey, REGSTR_VALUE_NEXTTRIAL, 0, REG_DWORD, (CONST BYTE*)&dwData, sizeof(DWORD));
						dwData = SendDlgItemMessage(hDlg, IDC_CB_IDLE, CB_GETCURSEL, 0L, 0L);
						RegSetValueEx(hKey, REGSTR_VALUE_IDLETIME, 0, REG_DWORD, (CONST BYTE*)&dwData, sizeof(DWORD));
						for (int i=0; i<OPTION_ITEMS_COUNT; i++)
							if (BST_CHECKED == SendDlgItemMessage(hDlg, aiOption[i][1], BM_GETCHECK, 0L, 0L))
							{
								dwData = aiOption[i][1] - IDC_RB_SHDWNNO;
								RegSetValueEx(hKey, REGSTR_VALUE_SHUTDOWNMODE, 0, REG_DWORD, (CONST BYTE*)&dwData, sizeof(DWORD));
                                if (dwData)
									ServiceStart(LPCTSTR_SERVICE_NAME);
								else
									ServiceStop(LPCTSTR_SERVICE_NAME);
								break;
							}
						RegCloseKey(hKey);
					}
					break;

				case PSN_QUERYCANCEL:
					break;
            
                default:
                    break;
            }
            break;

        default:
            return FALSE;
    }

    return TRUE;
}
/////////////////////////////////////////////////////////////////////
//
//  FUNCTION: CShellExt::AddPages(LPFNADDPROPSHEETPAGE, LPARAM)
//
//  PURPOSE: Called by the shell just before the property sheet is displayed.
//
//  PARAMETERS:
//    lpfnAddPage -  Pointer to the Shell's AddPage function
//    lParam      -  Passed as second parameter to lpfnAddPage
//
//  RETURN VALUE:
//
//    NOERROR in all cases.  If for some reason our pages don't get added,
//    the Shell still needs to bring up the Properties... sheet.
//
//  COMMENTS:
//
STDMETHODIMP
CShellExt::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage,
					LPARAM lParam)
{
    PROPSHEETPAGE psp;
    HPROPSHEETPAGE hPage;
    ZeroMemory(&psp, sizeof(psp));
    //STGMEDIUM medium;
	// The FORMATETC structure is a generalized Clipboard format.
    /*FORMATETC fmte = {
		CF_HDROP,
		(DVTARGETDEVICE FAR *)NULL,
		DVASPECT_CONTENT,
		-1,
		TYMED_HGLOBAL 
	};
    HRESULT hres = 0;

    if (m_pDataObj)
       hres = m_pDataObj->GetData(&fmte, &medium);

    if (SUCCEEDED(hres))
    {
        LPCSHELLEXT lpcsext = this;

        {*/

			//
			// Create a property sheet page object from a dialog box.
			//
			// We store a pointer to our class in the psp.lParam, so we
            // can access our class members from within the PropSheetPageDlgProc
			//
			// If the page needs more instance data, you can append
			// arbitrary size of data at the end of this structure,
			// and pass it to the CreatePropSheetPage. In such a case,
			// the size of entire data structure (including page specific
			// data) must be stored in the dwSize field.   Note that in
            // general you should NOT need to do this, as you can simply
            // store a pointer to date in the lParam member.
			//
        
            psp.dwSize      = sizeof(psp);	// no extra data.
            psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
            psp.hInstance   = g_hInst;
            psp.pszTemplate = MAKEINTRESOURCE(IDD_SHUTDOWN);
            psp.hIcon       = 0;
            psp.pszTitle    = TEXT("Zamykanie");
            psp.pfnDlgProc  = PropSheetPageDlgProc;
            psp.pcRefParent = (UINT*)&g_cDllRef;
            psp.pfnCallback = PropSheetPageCallback;
            psp.lParam      = (LPARAM)this;
            
            if (hPage = CreatePropertySheetPage(&psp)) 
            {
				if(lpfnAddPage(hPage, lParam))
					this->AddRef();
				else
					DestroyPropertySheetPage(hPage);
				return S_OK;
            }
			else
				return E_OUTOFMEMORY;
    //    }
    //}

    return E_FAIL;
}
/////////////////////////////////////////////////////////////////////
//
//  FUNCTION: CShellExt::ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM)
//
//  PURPOSE: Called by the shell only for Control Panel property sheet 
//           extensions
//
//  PARAMETERS:
//    uPageID         -  ID of page to be replaced
//    lpfnReplaceWith -  Pointer to the Shell's Replace function
//    lParam          -  Passed as second parameter to lpfnReplaceWith
//
//  RETURN VALUE:
//
//    E_FAIL, since we don't support this function.  It should never be
//    called.
//
//
STDMETHODIMP
CShellExt::ReplacePage(UINT uPageID,
					   LPFNADDPROPSHEETPAGE lpfnReplaceWith,
					   LPARAM lParam)
{
    return E_FAIL;
}
