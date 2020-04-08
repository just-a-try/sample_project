// RandomIntegerDLL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "RandomIntegerDLL.h"


// This is an example of an exported variable
RANDOMINTEGERDLL_API int nRandomIntegerDLL=0;

// This is an example of an exported function.
RANDOMINTEGERDLL_API int fnRandomIntegerDLL(void)
{
    return rand();
}

// This is the constructor of a class that has been exported.
CRandomIntegerDLL::CRandomIntegerDLL()
{
    return;
}
