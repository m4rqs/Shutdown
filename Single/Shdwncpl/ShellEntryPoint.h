
#include "ShellCommon.h"

HINSTANCE   g_hInst     = NULL;
LONG        g_cDllRef   = 0;

// {900FE8D7-1626-44e6-B432-F202BF1E0647}
static const GUID CLSID_ShellExt = 
{ 0x900fe8d7, 0x1626, 0x44e6, { 0xb4, 0x32, 0xf2, 0x2, 0xbf, 0x1e, 0x6, 0x47 } };

const LPCTSTR pHKLM_System_PSH = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Controls Folder\\System\\shellex\\PropertySheetHandlers\\%s";
const LPCTSTR pHKCR_CLSID = L"CLSID\\%s";

#define LPCTSTR_SERVICE_NAMEW   L"Shutdown"
