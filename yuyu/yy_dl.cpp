#include "yy.h"

#ifdef YY_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#error For Windows
#endif

extern "C"
{

	YY_API dll_handle YY_C_DECL yyLoadLibrary(const char* libraryName){
		YY_DEBUG_PRINT_FUNC;
#ifdef YY_PLATFORM_WINDOWS
		return (dll_handle)LoadLibraryA(libraryName);
#else
#error For Windows
#endif
		//return nullptr;
	}

	YY_API void YY_C_DECL yyFreeLibrary(dll_handle lib){
		YY_DEBUG_PRINT_FUNC;
#ifdef YY_PLATFORM_WINDOWS
		FreeLibrary((HMODULE)lib);
#else
#error For Windows
#endif
	}

	YY_API dll_function YY_C_DECL yyGetProcAddress(dll_handle lib,const char* functionName){
		YY_DEBUG_PRINT_FUNC;
#ifdef YY_PLATFORM_WINDOWS
		return (dll_function)GetProcAddress((HMODULE)lib, functionName);
#else
#error For Windows
#endif
		//return nullptr;
	}

}
