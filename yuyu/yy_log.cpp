#include "yy.h"

#include <cstdarg>
#include <cstring>
#include <cstdio>

class Log
{
public:
	Log();
	~Log();

	enum E_MessageType
	{
		EMT_INFO,
		EMT_ERROR,
		EMT_WARNING
	};

	void print(E_MessageType mt)
	{
		char buffer[1054];
		switch (mt)
		{
		case Log::EMT_INFO:
		{
			sprintf(buffer, "Info: %s", m_buffer);
			if(m_onInfo)
				m_onInfo(buffer);
		}break;
		case Log::EMT_ERROR:
		{
			sprintf(buffer, "Error: %s", m_buffer);
			if(m_onError)
				m_onError(buffer);
		}break;
		case Log::EMT_WARNING:
		{
			sprintf(buffer, "Warning: %s", m_buffer);
			if(m_onWarning)
				m_onWarning(buffer);
		}break;
		default:
			break;
		}
	}

	char m_buffer[1024];

	bool m_isInit;

	void(*m_onError)(const char* message) ;
	void(*m_onInfo)(const char* message) ;
	void(*m_onWarning)(const char* message) ;
};

Log::Log()
	:
	m_isInit(false),
	m_onError(nullptr),
	m_onInfo(nullptr),
	m_onWarning(nullptr)
{
}

Log::~Log()
{
	yyLogWriteInfo("Destroy log\n");
}

Log g_loger;

extern "C"
{

YY_API void YY_C_DECL yyLogSetErrorOutput(void(*f)(const char* message))
{
	g_loger.m_onError = f;
}

YY_API void YY_C_DECL yyLogSetInfoOutput(void(*f)(const char* message))
{
	g_loger.m_onInfo = f;
}

YY_API void YY_C_DECL yyLogSetWarningOutput(void(*f)(const char* message))
{
	g_loger.m_onWarning = f;
}

YY_API void YY_C_DECL yyLogWriteInfo(const char* format,...)
{
	va_list arg;
	va_start(arg, format);
	vsnprintf(g_loger.m_buffer,1024,format,arg);
	va_end (arg);
	g_loger.print(Log::EMT_INFO);
}

YY_API void YY_C_DECL yyLogWriteError(const char* format,...)
{
	va_list arg;
	va_start(arg, format);
	vsnprintf(g_loger.m_buffer,1024,format,arg);
	va_end (arg);
	g_loger.print(Log::EMT_ERROR);
}

YY_API void YY_C_DECL yyLogWriteWarning(const char* format,...)
{
	va_list arg;
	va_start(arg, format);
	vsnprintf(g_loger.m_buffer,1024,format,arg);
	va_end (arg);
	g_loger.print(Log::EMT_WARNING);
}

}
