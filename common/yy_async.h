#ifndef _YUYU_ASYNC_H_
#define _YUYU_ASYNC_H_

#include "yy_resource.h"

typedef void(*yyAsyncLoadEventHandler)(u32 userIndex, yyResource*);

extern "C"
{
	YY_API void YY_C_DECL yySetAsyncLoadEventHandler(yyAsyncLoadEventHandler);
}


#endif