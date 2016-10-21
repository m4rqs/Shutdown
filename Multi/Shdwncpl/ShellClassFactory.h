#pragma once

#include "ShellCommon.h"

extern LONG g_cDllRef;

class CClassFactory : public IClassFactory
{
public:
    CClassFactory();
    ~CClassFactory();

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IClassFactory methods
    STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
    STDMETHODIMP LockServer(BOOL fLock);

protected:
    LONG m_cRef;
};
