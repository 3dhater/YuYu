#ifndef _YUYU_H_
#define _YUYU_H_

#include "yy_config.h"
#include "yy_info.h"
#include "yy_types.h"
#include "yy_forward.h"
#include "yy_log.h"

enum class yyVideoDriverType
{
	OpenGLLast
};

enum class yyAudioDriverType
{
	None,
	XAudio2
};

enum class yySystemState : u32 
{
	Run,
	Quit
};

extern "C"
{
	YY_API yySystemState* YY_C_DECL yyStart();
	YY_API void YY_C_DECL yyStop();
	YY_API void YY_C_DECL yyQuit();
}

#endif