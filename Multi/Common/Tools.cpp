
#include "tools.h"

BOOL SaveLogInfo(CHAR* pchFormat, ...)
{
	SYSTEMTIME st;
	TCHAR achLogPath[MAX_PATH];
	TCHAR achLogFileName[MAX_PATH];
	CHAR achInfo[1024];
	CHAR achBuffer[1024];
	LPTSTR lpEnd;
	HANDLE hFile;
	DWORD dwNumberOfBytesWritten;
	va_list pArgList;
	size_t cchDest = MAX_PATH;
	size_t cchInfo;

	va_start(pArgList, pchFormat);
	StringCchVPrintfA(achBuffer, sizeof(achBuffer), pchFormat, pArgList);
	va_end(pArgList);

	GetModuleFileName(NULL, achLogPath, sizeof(achLogPath));
	
    lpEnd = StrRChrI(achLogPath, NULL, TEXT('\\')) + 1;
	cchDest -= (lpEnd - achLogPath);
	StringCchCopy(lpEnd, cchDest, TEXT("Logs"));
	CreateDirectory(achLogPath, NULL);
	
    GetLocalTime(&st);
	StringCchPrintf(achLogFileName, sizeof(achLogFileName), TEXT("\\%i%02i%02i.log"), st.wYear, st.wMonth, st.wDay);
	StringCchCat(achLogPath, sizeof(achLogPath), achLogFileName);
 
	hFile = CreateFile(achLogPath,  // file to create
	    GENERIC_WRITE,              // open for writing
	    0,                          // do not share
	    NULL,                       // default security
	    OPEN_ALWAYS,                // open existing or create new
	    FILE_ATTRIBUTE_NORMAL,      // | // normal file
	    //FILE_FLAG_OVERLAPPED,     // asynchronous I/O
	    NULL);                      // no attr. template

	if (hFile == INVALID_HANDLE_VALUE) { 
		return FALSE;
	}

	SetFilePointer(hFile, 0, NULL, FILE_END);
	StringCchPrintfA(achInfo, sizeof(achInfo), "%i.%02i.%02i %02i:%02i:%02i:%03i - %s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, achBuffer);
	StringCchLengthA(achInfo, sizeof(achInfo), &cchInfo);
	WriteFile(hFile, achInfo, cchInfo, &dwNumberOfBytesWritten,  NULL);

	CloseHandle(hFile);

	return TRUE;
}

void ShutdownBeep()
{
	Beep(1000, 30);
	Beep(1200, 20);
	Sleep(10);
	Beep(800, 30);
	Beep(1000, 20);
	Sleep(20);
	Beep(600, 30);
	Beep(800, 20);
	for (int i=0; i<7; i++)
	{
		Beep(900+80*i, 30);
		Beep(1500+80*i, 20);
		Beep(1200+80*i, 25);
		Beep(1800+80*i, 15);
		Sleep(5);
	}
}

BOOL SetDlgItemFont(HWND hDlg,
					int nIDDlgItem,
					LPTSTR lfFaceName,
					BYTE bSize,
					BOOL fBold, BOOL fItalic, BOOL fUnderline)
{
	HFONT hFont;
	LOGFONT lFont;

	if ((hFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0L)) != NULL)
	{
		if (GetObject(hFont, sizeof(LOGFONT), (LPSTR)&lFont))
		{
			if (bSize)
			{
				HDC hDC = GetDC(hDlg);
				lFont.lfHeight = -MulDiv(bSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
				ReleaseDC(hDlg, hDC);
			}
			lFont.lfWeight = fBold ? FW_BOLD : FW_NORMAL;
			lFont.lfItalic = fItalic;
			lFont.lfUnderline = fUnderline;
			if (lfFaceName)
				StringCchCopy(lFont.lfFaceName, LF_FACESIZE, lfFaceName);
			if ((hFont = CreateFontIndirect(&lFont)) != NULL)
			{
				SendDlgItemMessage(hDlg, nIDDlgItem, WM_SETFONT, (WPARAM)hFont, 0L);
				return TRUE;
			}
		}
	}

	return FALSE;
}
/***************************************************************************************************
 *	Ustalenie koloru elementów statycznych w oknie dialogowym
 *
 *	case WM_CTLCOLORSTATIC:
 *		switch (GetWindowLong((HWND)lParam, GWL_ID))
 *		{
 *		case IDC_???:	return SetColors(RGB(0, 0, 255), COLOR_BTNFACE, wParam);
 *		}
 *		return FALSE;
 */
BOOL SetColors(COLORREF rgbText, COLORREF rgbBackground, WPARAM wParam)
{
	HBRUSH hBrush = CreateSolidBrush(rgbBackground);
	SetTextColor((HDC)wParam, rgbText);
	SetBkColor((HDC)wParam, rgbBackground);
	UnrealizeObject(hBrush);
	SetBrushOrgEx((HDC)wParam, 0, 0, NULL);
	return (BOOL)hBrush;
}

DWORD GetSystemVersion(VOID)
{
	OSVERSIONINFOEX osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    GetVersionEx((OSVERSIONINFO*)&osvi);

    if (osvi.dwMajorVersion >= 6)
	{
		return (osvi.wProductType == VER_NT_WORKSTATION) ? WIN_VISTA : WIN_2008;
	}
	
	if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1))
	{
		return (osvi.wProductType == VER_NT_WORKSTATION) ? WIN_XP : WIN_2003;
	}
	
	if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0))
	{
		return WIN_2000;
	}
	
	return WIN_UNKNOWN;

}

VOID SetBit(LPUINT puiData, BYTE bBit, BOOL fState)
{
    if (fState)
        *puiData |= (1 << bBit);
    else
        *puiData &= ~(1 << bBit);
}

BOOL CheckBit(DWORD dwData, BYTE bBit)
{
    return dwData & (1 << bBit);
}