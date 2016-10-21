

#include "ShellEntryPoint.h"


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, void *)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = hInst;
        break;
    }

    return TRUE;
}


STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

    if (IsEqualCLSID(CLSID_ShellExt, rclsid))
    {
        CClassFactory *pClassFactory = new CClassFactory();
        hr = pClassFactory ? S_OK : E_OUTOFMEMORY;
        
        if (SUCCEEDED(hr))
        {
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
        }
    }

    return hr;
}


STDAPI DllCanUnloadNow()
{
    return g_cDllRef > 0 ? S_FALSE : S_OK;
}


HRESULT RegisterServer()
{
    LPOLESTR psz;
    WCHAR szCLSID[MAX_PATH];
    HRESULT hr = StringFromCLSID(CLSID_ShellExt, &psz);
    if (SUCCEEDED(hr))
    {
        hr = StringCchCopyW(szCLSID, MAX_PATH/*ARRAYSIZE(szCLSID)*/, psz);
        CoTaskMemFree(psz);
    }

    WCHAR szSubkey[MAX_PATH];
    HKEY hKey;
    DWORD dwDisp;

    if (SUCCEEDED(hr))
    {
        WCHAR szName[] = LPCTSTR_SERVICE_NAMEW;
        hr = StringCchPrintfW(szSubkey, MAX_PATH/*ARRAYSIZE(szSubkey)*/, pHKLM_System_PSH, szName);
        if (SUCCEEDED(hr))
        {
            hr = E_FAIL;
            if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_LOCAL_MACHINE, 
                                                 szSubkey, 
                                                 0, 
                                                 NULL, 
                                                 REG_OPTION_NON_VOLATILE, 
                                                 KEY_WRITE, 
                                                 NULL, 
                                                 &hKey, 
                                                 &dwDisp))
            {
                if (ERROR_SUCCESS == RegSetValueExW(hKey, 
                                                    NULL, 
                                                    0, 
                                                    REG_SZ, 
                                                    (LPBYTE)szCLSID, 
                                                    MAX_PATH))
                {
                    hr = S_OK;
               }

                RegCloseKey(hKey);
            }
        }
        hr = StringCchPrintfW(szSubkey, MAX_PATH/*ARRAYSIZE(szSubkey)*/, L"CLSID\\%s", szCLSID);
        if (SUCCEEDED(hr))
        {
            hr = E_FAIL;
            if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_CLASSES_ROOT, 
                                                 szSubkey, 
                                                 0, 
                                                 NULL, 
                                                 REG_OPTION_NON_VOLATILE, 
                                                 KEY_WRITE, 
                                                 NULL, 
                                                 &hKey, 
                                                 &dwDisp))
            {
                WCHAR szName[] = LPCTSTR_SERVICE_NAMEW;
                if (ERROR_SUCCESS == RegSetValueExW(hKey, 
                                                    NULL, 
                                                    0, 
                                                    REG_SZ, 
                                                    (LPBYTE)szName, 
                                                    sizeof(szName)))
                {
                    hr = S_OK;
                }

                RegCloseKey(hKey);
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = StringCchPrintfW(szSubkey, MAX_PATH/*ARRAYSIZE(szSubkey)*/, TEXT("CLSID\\%s\\InprocServer32"), szCLSID);
        if (SUCCEEDED(hr))
        {
            hr = E_FAIL;
            if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_CLASSES_ROOT, 
                                                 szSubkey, 
                                                 0, 
                                                 NULL, 
                                                 REG_OPTION_NON_VOLATILE, 
                                                 KEY_WRITE, 
                                                 NULL, 
                                                 &hKey, 
                                                 &dwDisp))
            {
                WCHAR szModule[MAX_PATH];
                if (GetModuleFileNameW(g_hInst, szModule, ARRAYSIZE(szModule)))
                {
                    DWORD cch;
                    hr = StringCchLengthW(szModule, ARRAYSIZE(szModule), (size_t *)(&cch));
                    if (SUCCEEDED(hr))
                    {
                        if (ERROR_SUCCESS != RegSetValueExW(hKey, 
                                                            NULL, 
                                                            0, 
                                                            REG_SZ, 
                                                            (LPBYTE) szModule, 
                                                            cch * sizeof(WCHAR)))
                        {
                            hr = E_FAIL;
                        }
                    }
                }

                if (SUCCEEDED(hr))
                {
                    WCHAR szModel[] = L"Apartment";
                    if (ERROR_SUCCESS != RegSetValueExW(hKey, 
                                                        L"ThreadingModel", 
                                                        0, 
                                                        REG_SZ, 
                                                        (LPBYTE) szModel, 
                                                        sizeof(szModel)))
                    {
                        hr = E_FAIL;
                    }
                }

                RegCloseKey(hKey);
            }
        }
    }

    return hr;
}


HRESULT RegisterComCat()
{
    CoInitialize(NULL);

    ICatRegister *pcr;
    HRESULT hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
                                  NULL, 
                                  CLSCTX_INPROC_SERVER, 
                                  IID_PPV_ARGS(&pcr));
    if (SUCCEEDED(hr))
    {
        CATID catid = CATID_DeskBand;
        hr = pcr->RegisterClassImplCategories(CLSID_ShellExt, 1, &catid);
        pcr->Release();
    }

    CoUninitialize();

    return hr;
}


STDAPI DllRegisterServer()
{
    // Register the object
    HRESULT hr = RegisterServer();
    if (SUCCEEDED(hr))
    {
        // Register the component category for the object
        hr = RegisterComCat();
    }

    return SUCCEEDED(hr) ? S_OK : SELFREG_E_CLASS;
}

//*************************************************************
//
//  RegDeleteNodeRecurse()
//
//  Purpose:    Deletes a registry key and all it's subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDeleteNodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
    LPTSTR lpEnd;
    LONG lResult;
    DWORD dwSize;
    TCHAR szName[MAX_PATH];
    HKEY hKey;
    FILETIME ftWrite;

    // First, see if we can delete the key without having
    // to recurse.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    lResult = RegOpenKeyEx (hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

    if (lResult != ERROR_SUCCESS) 
		return (lResult == ERROR_FILE_NOT_FOUND) ? TRUE : FALSE;

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlen(lpSubKey);

    if (*(lpEnd - 1) != TEXT('\\')) 
    {
        *lpEnd =  TEXT('\\');
        lpEnd++;
        *lpEnd =  TEXT('\0');
    }

    // Enumerate the keys

    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                           NULL, NULL, &ftWrite);

    if (lResult == ERROR_SUCCESS) 
    {
        do {
            StringCchCopy (lpEnd, MAX_PATH*2, szName);

            if (!RegDeleteNodeRecurse(hKeyRoot, lpSubKey))
                break;

            dwSize = MAX_PATH;

            lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);

        } while (lResult == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = TEXT('\0');

    RegCloseKey (hKey);

    // Try again to delete the key.
    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS)
        return TRUE;

    return FALSE;
}

//*************************************************************
//
//  RegDeleteNode()
//
//  Purpose:    Deletes a registry key and all it's subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDeleteNode(HKEY hKeyRoot, LPTSTR lpSubKey)
{
    TCHAR szDelKey[2 * MAX_PATH];

    StringCchCopy (szDelKey, MAX_PATH*2, lpSubKey);
    return RegDeleteNodeRecurse(hKeyRoot, szDelKey);
}


STDAPI DllUnregisterServer()
{
    LPOLESTR psz;
    WCHAR szCLSID[MAX_PATH];
    HRESULT hr = StringFromCLSID(CLSID_ShellExt, &psz);
    if (SUCCEEDED(hr))
    {
        hr = StringCchCopyW(szCLSID, ARRAYSIZE(szCLSID), psz);
        CoTaskMemFree(psz);
    }

    if (SUCCEEDED(hr))
    {
        WCHAR szName[] = LPCTSTR_SERVICE_NAMEW;
        WCHAR szSubkey[MAX_PATH];
        hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), pHKLM_System_PSH, szName);
        if (SUCCEEDED(hr))
        {
            if (!RegDeleteNode(HKEY_LOCAL_MACHINE, szSubkey))
            {
                hr = E_FAIL;
            }
        }
        hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), TEXT("CLSID\\%s"), szCLSID);
        if (SUCCEEDED(hr))
        {
            //if (ERROR_SUCCESS != RegDeleteTreeW(HKEY_CLASSES_ROOT, szSubkey))
            if (!RegDeleteNode(HKEY_CLASSES_ROOT, szSubkey))
            {
                hr = E_FAIL;
            }
        }
    }

    return SUCCEEDED(hr) ? S_OK : SELFREG_E_CLASS;
}
