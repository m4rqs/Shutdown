// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SHDWNRES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SHDWNRES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//#ifdef SHDWNRES_EXPORTS
#define SHDWNRES_API __declspec(dllexport)
//#else
//#define SHDWNRES_API __declspec(dllimport)
//#endif

// This class is exported from the Shdwnres.dll
class SHDWNRES_API CShdwnres {
public:
	CShdwnres(void);
	// TODO: add your methods here.
};

extern SHDWNRES_API int nShdwnres;

SHDWNRES_API int fnShdwnres(void);
