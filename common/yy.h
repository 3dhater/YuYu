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

	// extensions = "obj 3ds fbx"
	// extensionTitle = "Supported 3d files"
	// return - path to selected file or nullptr
	//  call yyDestroy(path) after using
	// Before using call yySetMainWindow(yourWindow);
	YY_API yyString* YY_C_DECL yyOpenFileDialog(const char* title, const char* okButtonLabel, 
		const char* extensions, const char* extensionTitle);
	YY_API yyString* YY_C_DECL yySaveFileDialog(const char* title, const char* okButtonLabel,
		const char* extension);

	YY_API const wchar_t* YY_C_DECL yyGetWorkingDir();
	YY_API yyString* YY_C_DECL yyGetRelativePath(const wchar_t*);


	YY_API u8* YY_C_DECL yyCompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct );
	YY_API u8* YY_C_DECL yyDecompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct );

	YY_API bool YY_C_DECL yyInitVideoDriver(const char* dl, yyWindow*);
	
	YY_API yyVideoDriverAPI* YY_C_DECL yyGetVideoDriverAPI();
	
	YY_API yyImage* YY_C_DECL yyLoadImage(const char*); // after loading, you must call yyDestroyImage
	YY_API void YY_C_DECL yyLoadImageAsync(const char*, s32 id); // after loading, you must call yyDestroyImage
	YY_API void YY_C_DECL yyDeleteImage(yyImage*);
	
	// after loading, you must call yyDeleteModel
	YY_API yyMDL* YY_C_DECL yyLoadModel(const char*, bool useLinearFilterForTextures = true, bool loadTextures = true);
	// загрузить модель и поместить её в кеш. следующий вызов - получить из кеша
	YY_API yyMDL* YY_C_DECL yyGetModel(const char*, bool useLinearFilterForTextures = true, bool loadTextures = true);
	YY_API void YY_C_DECL yyDeleteModel(yyMDL*);

	YY_API void YY_C_DECL yyUpdateAsyncLoader();

	typedef void* dl_handle;
	typedef void* dl_function;
	YY_API dl_handle YY_C_DECL yyLoadLybrary(const char* libraryName);
	YY_API void YY_C_DECL yyFreeLybrary(dl_handle);
	YY_API dl_function YY_C_DECL yyGetProcAddress(dl_handle,const char* functionName);
	
	/* pivotPosition
		0--1--2
		|  |  |
		7--8--3
		|  |  |
		6--5--4
	*/
	YY_API yySprite* YY_C_DECL yyCreateSprite(const v4f& rect, yyResource* texture, u8 pivotPosition = 0);
	YY_API yySprite2* YY_C_DECL yyCreateSprite2(yyResource* texture);

	// get from cache. if not found, create GPU resource, add to cache.
	// ++m_refCount;
	// call videoDriverAPI->UnloadTexture for --m_refCount;
	YY_API yyResource* YY_C_DECL yyGetTextureResource(const char*, bool useFilter, bool useComparisonFilter, bool load);
	//YY_API yyResource* YY_C_DECL yyGetModelResource(const char*, bool load);

	YY_API void YY_C_DECL yyGetTextureSize(yyResource*, v2i*);

	YY_API void YY_C_DECL yySetMainWindow(yyWindow*);
	YY_API yyWindow* YY_C_DECL yyGetMainWindow();

	// загрузить аудио но без инициализации m_implementation
	//YY_API yyAudioSource* YY_C_DECL	yyLoadAudioRaw(const char*);
}

// for auto create\delete
struct yyEngineContext
{
	yyEngineContext()
	{
		m_state = nullptr;
	}
	~yyEngineContext()
	{
		yyStop(); // destroy main class, free memory
	}

	void init(yyInputContext* input)
	{
		m_state = yyStart(input); // allocate memory for main class inside yuyu.dll
	}

	yySystemState * m_state;
};
#endif