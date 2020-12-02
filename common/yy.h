#ifndef _YUYU_H_
#define _YUYU_H_

#include "yy_config.h"
#include "yy_info.h"
#include "yy_types.h"
#include "yy_forward.h"
#include "yy_memory.h"
#include "yy_log.h"
#include "yy_resource.h"
#include "yy_video.h"

enum class yySystemState : u32 
{
	Run,
	Quit
};


extern "C"
{
	YY_API yySystemState* YY_C_DECL yyStart(yyInputContext* input);
	YY_API void YY_C_DECL yyStop();
	YY_API void YY_C_DECL yyQuit();
	
	YY_API bool YY_C_DECL yyInitVideoDriver(const char* dl, yyWindow*);
	
	YY_API yyVideoDriverAPI* YY_C_DECL yyGetVideoDriverAPI();
	
	YY_API yyImage* YY_C_DECL yyLoadImage(const char*); // after loading, you must call yyDestroyImage
	YY_API void YY_C_DECL yyLoadImageAsync(const char*, s32 id); // after loading, you must call yyDestroyImage
	YY_API void YY_C_DECL yyDeleteImage(yyImage*);
	
	YY_API yyModel* YY_C_DECL yyLoadModel(const char*); // after loading, you must call yyDeleteModel
	YY_API void YY_C_DECL yyDeleteModel(yyModel*);

	YY_API void YY_C_DECL yyUpdateAsyncLoader();

	using dl_handle = void*;
	using dl_function = void*;
	YY_API dl_handle YY_C_DECL yyLoadLybrary(const char* libraryName);
	YY_API void YY_C_DECL yyFreeLybrary(dl_handle);
	YY_API dl_function YY_C_DECL yyGetProcAddress(dl_handle,const char* functionName);
	
	YY_API yySprite* YY_C_DECL yyCreateSprite(const v4f& rect, yyResource* texture);

}


#endif