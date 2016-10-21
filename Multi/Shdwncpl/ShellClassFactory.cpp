
#include "ShellClassFactory.h"

CClassFactory::CClassFactory()
{
    m_cRef = 1;
    InterlockedIncrement(&g_cDllRef);
}

CClassFactory::~CClassFactory()
{
    InterlockedDecrement(&g_cDllRef);
}

//
// IUnknown methods
//
STDMETHODIMP CClassFactory::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT hr = E_NOINTERFACE;
    *ppv = NULL;

    //CheckPointer(ppv, E_POINTER);
    //ValidateReadWritePtr(ppv, sizeof(void *));
    
    if (IsEqualIID(IID_IUnknown, riid) || IsEqualIID(IID_IClassFactory, riid))
    {
        *ppv = static_cast<IUnknown *>(this);
    }

    if (*ppv)
    {
        ((IUnknown*)*ppv)->AddRef();
	hr = S_OK;
    }

    return hr;
}

STDMETHODIMP_(ULONG) CClassFactory::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CClassFactory::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }
    return cRef;
}

//
// IClassFactory methods
//
STDMETHODIMP CClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
    HRESULT hr = !pUnkOuter ? S_OK : CLASS_E_NOAGGREGATION;
    *ppv = NULL;

    if (SUCCEEDED(hr))
    {
        CShellExt *pShellExt = new CShellExt();
        hr = pShellExt ? S_OK : E_OUTOFMEMORY;
        
        if (SUCCEEDED(hr))
        {
            hr = pShellExt->QueryInterface(riid, ppv);
            
            if (FAILED(hr))
            {
                pShellExt->Release();
            }
        }
    }

    return hr;
}

STDMETHODIMP CClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        InterlockedIncrement(&g_cDllRef);
    }
    else
    {
        InterlockedDecrement(&g_cDllRef);
    }

    return S_OK;
}

