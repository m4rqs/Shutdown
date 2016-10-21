.586
.MODEL FLAT, STDCALL
OPTION CASEMAP:NONE

INCLUDE		windows.inc
INCLUDE		kernel32.inc
INCLUDE		user32.inc
INCLUDE		advapi32.inc
INCLUDELIB	kernel32.lib
INCLUDELIB	user32.lib
INCLUDELIB	advapi32.lib

.DATA

.DATA?
		hInst				DD ?
		hKeyHook			DD ?
		hCBTHook			DD ?
		hWndPrev			DD ?
		dwKeyStatus			DD ?
.CODE

DllEntry PROC hInstance:HINSTANCE, dwReason:DWORD, lpReserved:DWORD

		push  hInstance
		pop   hInst
		
	    mov eax, TRUE
	    ret

DllEntry ENDP

CBTProc PROC nCode, wParam, lParam:DWORD
    
		.IF nCode < 0
	    	INVOKE CallNextHookEx, hCBTHook, nCode, wParam, lParam
	    	ret
	    .ENDIF
	    
		.IF nCode == HCBT_ACTIVATE
		.ENDIF
		
	    INVOKE CallNextHookEx, hCBTHook, nCode, wParam, lParam
		ret
	
CBTProc ENDP

KeyboardProc PROC nCode, wParam, lParam:DWORD

		.IF nCode < 0
	    	INVOKE CallNextHookEx, hKeyHook, nCode, wParam, lParam
	    	ret
	    .ENDIF
		
		mov eax, lParam
		and eax, 80000000h ; sprawdzamy ostatni bit, jesli jest rowny 0 to mamy nacisniecie kawisza
		
		.IF (eax == 0) && (nCode == HC_ACTION)
		.ENDIF
	    	
	    INVOKE CallNextHookEx, hKeyHook, nCode, wParam, lParam
	    ret

KeyboardProc ENDP

SetActiveServer PROC fHook:BYTE

	    LOCAL fResult:DWORD
	    LOCAL dwSetHook:DWORD
	    LOCAL dwSize:DWORD

	    .IF fHook
			push offset szHookStartError
			pop dwSetHook
	    	INVOKE SetWindowsHookEx, WH_CBT, ADDR CBTProc, hInst, NULL
	    	mov hCBTHook, eax
	    	.IF hCBTHook != NULL
		    	INVOKE SetWindowsHookEx, WH_KEYBOARD, ADDR KeyboardProc, hInst, NULL
		    	mov hKeyHook, eax
				.IF hKeyHook != NULL
					push offset szHookStart
					pop dwSetHook
				.ENDIF
			.ENDIF
	   .ELSE
			mov fResult, 0
			.IF hCBTHook != NULL
			    INVOKE UnhookWindowsHookEx, hCBTHook
			    mov fResult, eax
			    .IF hKeyHook != NULL
				    INVOKE UnhookWindowsHookEx, hKeyHook
				    mov fResult, eax
				.ENDIF
			.ENDIF
			.IF fResult != 0
				push offset szHookStop
				pop dwSetHook
			.ELSE
				push offset szHookStopError
				pop dwSetHook
			.ENDIF
	    .ENDIF
	    
	    ret 

SetActiveServer ENDP

END DllEntry
