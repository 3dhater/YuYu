#ifndef _YUYU_LOG_H_
#define _YUYU_LOG_H_

extern "C"
{
YY_API void YY_C_DECL				yyLogWriteError(const char*fmt,...);
YY_API void YY_C_DECL				yyLogWriteInfo(const char*fmt,...);
YY_API void YY_C_DECL				yyLogWriteWarning(const char*fmt,...);
YY_API void YY_C_DECL				yyLogSetErrorOutput(void(*)(const char* message));
YY_API void YY_C_DECL				yyLogSetInfoOutput(void(*)(const char* message));
YY_API void YY_C_DECL				yyLogSetWarningOutput(void(*)(const char* message));
}

#define YY_PRINT_FAILED	yyLogWriteError( "Failed [%s][%s][%i]:(\n", YY_FILE, YY_FUNCTION, YY_LINE )
#define YY_CHECK_POINTER(ptr) if(!ptr) YY_PRINT_FAILED

#endif