

#include "Shdwnhk.h"


SHDWNHK_API DWORD SHDWNHK_CALL GetLastInputTickCount()
{
	return g_dwLastTick;
}


void SaveLastInputTickCount(void)
{
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_APP, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, REGSTR_VALUE_LASTINPUTTICKCOUNT, 0, REG_DWORD, (CONST BYTE*)&g_dwLastTick, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
	    return CallNextHookEx(g_hKeyboard, nCode, wParam, lParam);

	//if ((lParam & 0x80000000) && (nCode == HC_ACTION))
	if (nCode == HC_ACTION)
    {
		g_dwLastTick = GetTickCount();
        SaveLastInputTickCount();
	}

	return CallNextHookEx(g_hKeyboard, nCode, wParam, lParam);
}


LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(g_hMouse, nCode, wParam, lParam);

 	if (nCode == HC_ACTION)
    {
		//MOUSEHOOKSTRUCTEX* pmhStruct = (MOUSEHOOKSTRUCTEX*)lParam;
        //if ((pmhStruct->pt.x != g_lMouseX) || (pmhStruct->pt.y != g_lMouseY))
		//{
			//g_lMouseX = pmhStruct->pt.x;
			//g_lMouseY = pmhStruct->pt.y;
			g_dwLastTick = GetTickCount();
            SaveLastInputTickCount();
	    //}
	}

	return CallNextHookEx(g_hMouse, nCode, wParam, lParam);
}


SHDWNHK_API BOOL SHDWNHK_CALL HookLastInputTickCount()
{
	if (g_hKeyboard == NULL)
		g_hKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hInst, 0);
	if (g_hMouse == NULL)
		g_hMouse = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, g_hInst, 0);
	g_dwLastTick = GetTickCount();
    SaveLastInputTickCount();
	return (g_hKeyboard && g_hMouse);
}


SHDWNHK_API BOOL SHDWNHK_CALL UnhookLastInputTickCount()
{
	BOOL fResult = TRUE;
	if (g_hKeyboard)
	{
		fResult = UnhookWindowsHookEx(g_hKeyboard);
		g_hKeyboard = NULL;
	}
	if (g_hMouse)
	{
		fResult = UnhookWindowsHookEx(g_hMouse);
		g_hMouse = NULL;
	}
    return fResult;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	    case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            g_hInst = (HINSTANCE)hModule;
		    break;
	    case DLL_PROCESS_DETACH:
            //UnhookLastInputTickCount();
		    break;
	}
	return TRUE;
}


extern "C" BOOL __stdcall _DllMainCRTStartup(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    return DllMain(hModule, dwReason, lpvReserved);
}
