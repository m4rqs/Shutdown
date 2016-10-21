/***************************************************************************************************
 *	
 */
#include <windows.h>
#include <wininet.h>
/***************************************************************************************************
 *	
 *//*
LPCTSTR GRS(int nResource)
{
	static TCHAR szBuffer[256];

    LoadString(hDLL, nResource, szBuffer, sizeof(szBuffer));

	return szBuffer;
}
/***************************************************************************************************
 *	Zmiana parametrów czcionki dla elementów okna dialogowego
 */
int CALLBACK EnumFontFamExProc(
  ENUMLOGFONT FAR *lpelf,  // pointer to logical-font data
  NEWTEXTMETRIC FAR *lpntm,// pointer to physical-font data
  int FontType,            // type of font
  LPARAM lParam            // pointer to application-defined data
)
{
    return (int)lParam;
}

BOOL IsFont(PTSTR lfFaceName)
{
	LOGFONT lf;
    BOOL fResult;
	HDC hDC = GetDC(NULL);
    lstrcpy(lf.lfFaceName, lfFaceName);
    fResult = (BOOL)((LPARAM)lf.lfFaceName == EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)lf.lfFaceName, 0));
	ReleaseDC(NULL, hDC);
    return fResult;
}

BOOL SetDlgItemFont(HWND hDlg,
					int nIDDlgItem,
					PTSTR lfFaceName,
					BYTE bSize,
					BOOL fBold, BOOL fItalic, BOOL fUnderline)
{
	HFONT hFont;
	LOGFONT lFont;

	nIDDlgItem == 0 ?
		SendMessage(hDlg, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0)):
		SendDlgItemMessage(hDlg, nIDDlgItem, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

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
				if (IsFont(lfFaceName))
                    lstrcpy(lFont.lfFaceName, lfFaceName);
			if ((hFont = CreateFontIndirect(&lFont)) != NULL)
			{
				nIDDlgItem == 0 ?
					SendMessage(hDlg, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0)):
					SendDlgItemMessage(hDlg, nIDDlgItem, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				return TRUE;
			}
		}
	}

	return FALSE;
}

void SetDlgFont(HWND hDlg, PTSTR lfFaceName)
{
    HWND hChild = NULL;

    while (hChild = FindWindowEx(hDlg, hChild, NULL, NULL))
	    SetDlgItemFont(hChild, 0, lfFaceName, 0, FALSE, FALSE, FALSE);
}

BOOL HttpDownloadFile(HINTERNET hConnect, LPCTSTR lpszObjectName, LPCTSTR lpszDestName, BOOL fDownloadAlways)
{
	BOOL fResult = FALSE;
	HINTERNET hRequest;

	if (NULL != (hRequest = HttpOpenRequest(hConnect, NULL, lpszObjectName, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0)))
	{
		if (HttpSendRequest(hRequest, NULL, 0, 0, 0))
		{
			char szLengthBuffer[32];
			DWORD dwLengthBuffer = sizeof(szLengthBuffer);
			SYSTEMTIME st;
			DWORD dwSystemTime = sizeof(SYSTEMTIME);
			DWORD dwNumberOfBytesRead = 0;
			DWORD dwFileSize;
			LPTSTR lpBuffer;

			if (HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, szLengthBuffer, &dwLengthBuffer, NULL))
			{
				dwFileSize = (DWORD)atol(szLengthBuffer);

				if (fDownloadAlways || HttpQueryInfo(hRequest, HTTP_QUERY_LAST_MODIFIED | HTTP_QUERY_FLAG_SYSTEMTIME, &st, &dwSystemTime, NULL))
				{
					BOOL fDownload = TRUE;

					if (fDownloadAlways == FALSE)
					{
						TIME_ZONE_INFORMATION tzi;
						SYSTEMTIME lt;
						FILETIME ftSourceLastWriteTime, ftDestLastWriteTime;
						HANDLE hFile;

						GetTimeZoneInformation(&tzi);
						SystemTimeToTzSpecificLocalTime(&tzi, &st, &lt);
						SystemTimeToFileTime(&lt, &ftSourceLastWriteTime);

						if (NULL != (hFile = CreateFile(lpszDestName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
						{
							GetFileTime(hFile, NULL, NULL, &ftDestLastWriteTime);
							if (0 >= CompareFileTime(&ftSourceLastWriteTime, &ftDestLastWriteTime))
							{
								CloseHandle(hFile);
								return FALSE;
							}
							CloseHandle(hFile);
							fDownload = TRUE;
						}
					}

					if (fDownload && (NULL != (lpBuffer = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize))))
					{
						HANDLE hFile;
						DWORD dwNumberOfBytesWritten;

						if (InternetReadFile(hRequest, lpBuffer, dwFileSize, &dwNumberOfBytesRead))
						{
							lpBuffer[dwFileSize] = 0;
							lpszObjectName++;

							if (NULL != (hFile = CreateFile(lpszObjectName, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL)))
							{
								if (WriteFile(hFile, lpBuffer, dwFileSize, &dwNumberOfBytesWritten, NULL))
								{
									fResult = TRUE;
								}
								CloseHandle(hFile);
							}
						}
					}
				}
			}
		}
		InternetCloseHandle(hRequest);
	}
	return fResult;
}

void PrintFormatMsg(HWND hWndEdit, TCHAR* szFormat, ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;

	va_start(pArgList, szFormat);
	wvsprintf(szBuffer, szFormat, pArgList);
	va_end(pArgList);

	SendMessage(hWndEdit, EM_SETSEL, (WPARAM) -1, (LPARAM) -1);
	SendMessage(hWndEdit, EM_REPLACESEL, FALSE, (LPARAM) szBuffer);
	SendMessage(hWndEdit, EM_SCROLLCARET, 0, 0);
}