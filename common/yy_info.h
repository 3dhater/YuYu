#ifndef _YUYU_INFO_H_
#define _YUYU_INFO_H_

#if defined _WIN32 || defined __CYGWIN__
#define YY_PLATFORM_WINDOWS
#else
#error Only for Windows
#endif

// define YY_EXPORTS in yuyu.dll project
#ifdef YY_PLATFORM_WINDOWS
#ifdef _MSC_VER
#define YY_C_DECL _cdecl
#ifdef YY_EXPORTS
#define YY_API _declspec(dllexport)
#else
#define YY_API _declspec(dllimport)
#endif
#else
#define YY_C_DECL
#define YY_API
#endif
#else
#define YY_C_DECL
#define YY_API
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define YY_DEBUG
#endif

#ifdef YY_DEBUG
#define YY_DEBUGBREAK __debugbreak()
#else
#define YY_DEBUGBREAK
#endif

#ifdef YY_PLATFORM_WINDOWS
#define YY_FILE __FILE__
#define YY_FUNCTION __FUNCTION__
#define YY_LINE __LINE__

#define YY_DL_LOADLIBRARY
#define YY_DL_FREELIBRARY
#define YY_DL_GETPROCADDRESS

#ifdef _MSC_VER
#define YY_FORCE_INLINE __forceinline
#else
#define YY_FORCE_INLINE inline
#endif
#endif

#ifdef YY_DEBUG
#define YY_DEBUG_PRINT_FUNC yyLogWriteInfo("%s\n", YY_FUNCTION)
#else
#define YY_DEBUG_PRINT_FUNC
#endif

#endif