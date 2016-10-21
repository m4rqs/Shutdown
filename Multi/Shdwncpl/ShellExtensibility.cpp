
#include "ShellExtensibility.h"

CShellExt::CShellExt()
{
	m_cRef = 0L;
	m_pDataObj = NULL;
	InterlockedIncrement(&g_cDllRef);
}

CShellExt::~CShellExt()
{
	if (m_pDataObj)
		m_pDataObj->Release();
	InterlockedDecrement(&g_cDllRef);
}

//
// IUnknown methods
//
STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    HRESULT hr = E_NOINTERFACE;
    *ppv = NULL;

    //CheckPointer(ppv, E_POINTER);
    //ValidateReadWritePtr(ppv, sizeof(void *));

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IShellExtInit))
    {
        *ppv = static_cast<IShellExtInit *>(this);
    }
    else if (IsEqualIID(riid, IID_IShellPropSheetExt))
    {
        *ppv = static_cast<IShellPropSheetExt *>(this);
    }

    if (*ppv)
    {
	((IUnknown*)*ppv)->AddRef();
	hr = S_OK;
    }

    return hr;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }
    return cRef;
}

//
// IShellExtInit methods
//
STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST pIDFolder,
                                   LPDATAOBJECT pDataObj,
                                   HKEY hRegKey)
{
    INITCOMMONCONTROLSEX icc;

    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_STANDARD_CLASSES;

    InitCommonControlsEx(&icc);

    if (m_pDataObj)
	    m_pDataObj->Release();

    if (pDataObj)
    {
	    m_pDataObj = pDataObj;
	    pDataObj->AddRef();
    }

    return S_OK;
}
