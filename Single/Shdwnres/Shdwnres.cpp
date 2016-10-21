// Shdwnres.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Shdwnres.h"


// This is an example of an exported variable
SHDWNRES_API int nShdwnres=0;

// This is an example of an exported function.
SHDWNRES_API int fnShdwnres(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see Shdwnres.h for the class definition
CShdwnres::CShdwnres()
{
	return;
}
