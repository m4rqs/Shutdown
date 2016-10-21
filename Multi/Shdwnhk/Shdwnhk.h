// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SHDWNHK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SHDWNHK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once

#include "..\common\targetver.h"

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN             

// Windows Header Files:
#include <windows.h>

#include "..\common\common.h"

#pragma data_seg(".shdwn")
    HHOOK 	g_hKeyboard = NULL;
    HHOOK 	g_hMouse = NULL;
    DWORD	g_dwLastTick = 0;
    LONG	g_lMouseX = -1;
    LONG	g_lMouseY = -1;
#pragma data_seg()
#pragma comment(linker, "/section:.shdwn,rws")

HINSTANCE g_hInst = NULL;

#ifdef SHDWNHK_EXPORTS
    #ifdef __cplusplus
        #define SHDWNHK_API extern "C" __declspec(dllexport)
    #else
        #define SHDWNHK_API __declspec(dllexport)
    #endif
#else
    #define SHDWNHK_API __declspec(dllimport)
#endif
