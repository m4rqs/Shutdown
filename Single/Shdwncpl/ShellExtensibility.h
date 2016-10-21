#pragma once

#include "ShellCommon.h"

extern LONG g_cDllRef;		// Reference count of this DLL.
extern HINSTANCE g_hInst;	// Handle to this DLL itself.

class CShellExt : public IShellExtInit, IShellPropSheetExt
{
public:
	CShellExt();
	~CShellExt();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID FAR *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IShellExtInit methods
	STDMETHODIMP			Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	//IShellPropSheetExt methods
	STDMETHODIMP			AddPages(LPFNADDPROPSHEETPAGE, LPARAM);
	STDMETHODIMP			ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM);

	TCHAR m_szPropSheetFileUserClickedOn[MAX_PATH];

protected:
	LONG m_cRef;
	LPDATAOBJECT m_pDataObj;
};

typedef CShellExt *LPCSHELLEXT;
