#include "yy.h"

#ifdef YY_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#error For Windows
#endif

extern "C"
{

	YY_API dl_handle YY_C_DECL yyLoadLibrary(const char* libraryName){
#ifdef YY_PLATFORM_WINDOWS
		return (dl_handle)LoadLibraryA(libraryName);
#else
#error For Windows
#endif
		//return nullptr;
	}

	YY_API void YY_C_DECL yyFreeLibrary(dl_handle lib){
#ifdef YY_PLATFORM_WINDOWS
		FreeLibrary((HMODULE)lib);
#else
#error For Windows
#endif
	}

	YY_API dl_function YY_C_DECL yyGetProcAddress(dl_handle lib,const char* functionName){
#ifdef YY_PLATFORM_WINDOWS
		return (dl_function)GetProcAddress((HMODULE)lib, functionName);
#else
#error For Windows
#endif
		//return nullptr;
	}

}
