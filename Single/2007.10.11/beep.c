#include <windows.h>

WORD _GetPort(WORD wAddr)
{
	BYTE bValue;
	__asm mov DX, wAddr;
	__asm in  AL, DX;
	__asm mov bValue, AL;
	return bValue;
}

void _SetPort(WORD wAddr, WORD wValue)
{
	BYTE bValue = (BYTE)(wValue & 255);
	__asm mov DX, wAddr;
	__asm mov AL, bValue;
	__asm out DX, AL;
}

void _NoSound(void)
{
	_SetPort(0x61, _GetPort(0x61) & 0xfc);
}

void _Sound(WORD wFreq)
{
	WORD wPort;
	if (wFreq > 18)
	{
		wFreq = (WORD)(1193181 / wFreq);
		wPort = _GetPort(0x61);
		if ((wPort & 3) == 0)
		{
			_SetPort(0x61, wPort | 3);
			_SetPort(0x43, 0xb6);
		}
		_SetPort(0x42, wFreq);
		_SetPort(0x42, (wFreq >> 8));
	}
}

void ProcessMessage(void)
{
	MSG msg;
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void _Delay(DWORD dwMSec)
{
	DWORD dwStart = GetTickCount();
	while (1)
	{
		ProcessMessage();
		if ((GetTickCount() - dwStart) >= dwMSec)
			break;
	}
}


void _Beep(WORD wFreq, DWORD dwMSec)
{
	extern OSVERSIONINFOEX osvi;

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		Beep(wFreq, dwMSec);
	}
	else
	{
		_Sound(wFreq);
		_Delay(dwMSec);
		_NoSound();
	}
}
