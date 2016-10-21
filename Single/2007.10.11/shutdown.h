#include "common.h"


DWORD	dwUserName = UNLEN + CNLEN + 2;
DWORD	dwComputerName = UNLEN + CNLEN + 2;

int		iM = 3, iS = 0;

BOOL	fFindFirstFile = TRUE,
		fFinished = FALSE,
		fContinue = FALSE,
		fShutdown = FALSE,
		fFlashWindow;

void ControlRunningProcesses(HWND);
void Anchor(LPTSTR);

