
#include<stdlib.h>

#ifdef RANDOMINTEGERDLL_EXPORTS
#define RANDOMINTEGERDLL_API __declspec(dllexport)
#else
#define RANDOMINTEGERDLL_API __declspec(dllimport)
#endif

// This class is exported from the dll
class RANDOMINTEGERDLL_API CRandomIntegerDLL {
public:
	CRandomIntegerDLL(void);
	// TODO: add your methods here.
};

extern RANDOMINTEGERDLL_API int nRandomIntegerDLL;

RANDOMINTEGERDLL_API int fnRandomIntegerDLL(void);
