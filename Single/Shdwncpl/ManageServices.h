#pragma once

#include "ShellCommon.h"

#include "..\common\tools.h"

#define LOGINFO_BUFFER_SIZE				1024

VOID ServiceStart(LPCTSTR);
VOID ServiceStop(LPCTSTR);
DWORD QueryServiceState(LPCTSTR);
