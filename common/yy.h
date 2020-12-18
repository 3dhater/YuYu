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
#include "yy_ptr.h"

enum class yySystemState : u32 
{
	Run,
	Quit
};

enum class yyCompressType : u32
{
	WithoutCompress,
	ZStd
};



extern "C"
{
	YY_API yySystemState* YY_C_DECL yyStart(yyInputContext* input);
	YY_API void YY_C_DECL yyStop();
	YY_API void YY_C_DECL yyQuit();
	
	YY_API u64 YY_C_DECL yyGetTime();


	YY_API u8* YY_C_DECL yyCompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct );
	YY_API u8* YY_C_DECL yyDecompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct );

	YY_API bool YY_C_DECL yyInitVideoDriver(const char* dl, yyWindow*);
	
	YY_API yyVideoDriverAPI* YY_C_DECL yyGetVideoDriverAPI();
	
	YY_API yyImage* YY_C_DECL yyLoadImage(const char*); // after loading, you must call yyDestroyImage
	YY_API void YY_C_DECL yyLoadImageAsync(const char*, s32 id); // after loading, you must call yyDestroyImage
	YY_API void YY_C_DECL yyDeleteImage(yyImage*);
	
	YY_API yyModel* YY_C_DECL yyLoadModel(const char*); // after loading, you must call yyDeleteModel
	YY_API void YY_C_DECL yyDeleteModel(yyModel*);

	YY_API void YY_C_DECL yyUpdateAsyncLoader();

	typedef void* dl_handle;
	typedef void* dl_function;
	YY_API dl_handle YY_C_DECL yyLoadLybrary(const char* libraryName);
	YY_API void YY_C_DECL yyFreeLybrary(dl_handle);
	YY_API dl_function YY_C_DECL yyGetProcAddress(dl_handle,const char* functionName);
	
	YY_API yySprite* YY_C_DECL yyCreateSprite(const v4f& rect, yyResource* texture, bool pivotOnCenter);

	// get from cache. if not found, create GPU resource, add to cache
	YY_API yyResource* YY_C_DECL yyGetTexture(const char*, bool useFilter, bool load);
	YY_API yyResource* YY_C_DECL yyGetModel(const char*, bool load);

	YY_API void YY_C_DECL yyGetTextureSize(yyResource*, v2i*);

	YY_API void YY_C_DECL yySetMainWindow(yyWindow*);
	YY_API yyWindow* YY_C_DECL yyGetMainWindow();
}


#endif