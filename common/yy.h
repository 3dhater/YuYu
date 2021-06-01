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
#include "yy_events.h"
#include "yy_ptr.h"
#include "yy_cursor.h"

enum class yyKeyboardModifier : u32 {
	None,
	Ctrl,
	Alt,
	Shift,
	ShiftAlt,
	ShiftCtrl,
	ShiftCtrlAlt,
	CtrlAlt
};

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

enum class yyMatrixType : u32
{
	World,
	View,
	Projection,
	ViewProjection, //For 3d line
	WorldViewProjection,
	ViewInvert,

	_count
	//LightView,
	//LightProjection,
};


extern "C"
{
	YY_API yySystemState* YY_C_DECL yyStart(yyInputContext* input, yyPoolSetup* poolSetup = 0);
	YY_API void YY_C_DECL yyStop();
	YY_API void YY_C_DECL yyQuit();
	YY_API bool YY_C_DECL yyRun(f32* deltaTime);

	YY_API void YY_C_DECL yySetDefaultMaterial(const yyMaterial& mat);
	YY_API yyMaterial* YY_C_DECL yyGetDefaultMaterial();
	YY_API void YY_C_DECL yySetMaterial(yyMaterial* mat);
	YY_API yyMaterial* YY_C_DECL yyGetMaterial();

	YY_API void YY_C_DECL yySetDefaultEyePosition(const v3f&);
	YY_API v3f* YY_C_DECL yyGetDefaultEyePosition();
	YY_API void YY_C_DECL yySetEyePosition(v3f*);
	YY_API v3f* YY_C_DECL yyGetEyePosition();

	YY_API void YY_C_DECL yySetMatrix(yyMatrixType, Mat4*);
	YY_API Mat4* YY_C_DECL yyGetMatrix(yyMatrixType);
	YY_API void YY_C_DECL yySetDefaultMatrix(yyMatrixType, const Mat4&);
	YY_API Mat4* YY_C_DECL yyGetDefaultMatrix(yyMatrixType);

	YY_API void YY_C_DECL yySetBoneMatrix(u32 boneIndex, const Mat4&);
	YY_API Mat4* YY_C_DECL yyGetBoneMatrix(u32 boneIndex);

	YY_API u64 YY_C_DECL yyGetTime();
	
	YY_API yyCursor* YY_C_DECL yyGetCursor(yyCursorType);
	YY_API void YY_C_DECL yyResetCursor(yyCursorType);
	YY_API void YY_C_DECL yyShowCursor(bool);
	YY_API void YY_C_DECL yySetCursor(yyCursorType, yyCursor*);
	YY_API void YY_C_DECL yySetCursorDisableAutoChange(bool);
	YY_API bool YY_C_DECL yyGetCursorDisableAutoChange();

	YY_API yyInputContext* YY_C_DECL yyGetInputContext();
	
	YY_API void YY_C_DECL yyCopyTextToClipboard(yyStringW*);
	YY_API void YY_C_DECL yyGetTextFromClipboard(yyStringW*);

	// extensions = "obj 3ds fbx"
	// extensionTitle = "Supported 3d files"
	// return - path to selected file or nullptr
	//  call yyDestroy(path) after using
	// Before using call yySetMainWindow(yourWindow);
	YY_API yyString* YY_C_DECL yyOpenFileDialog(const char* title, const char* okButtonLabel, 
		const char* extensions, const char* extensionTitle);
	YY_API yyString* YY_C_DECL yySaveFileDialog(const char* title, const char* okButtonLabel,
		const char* extension);

	// true if dll file is video driver
	YY_API bool YY_C_DECL yyIsValidVideoDriver( const char* dll_file_name);

	YY_API const wchar_t* YY_C_DECL yyGetWorkingDir();
	YY_API yyString* YY_C_DECL yyGetRelativePath(const wchar_t*);

	YY_API u8* YY_C_DECL yyCompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct );
	YY_API u8* YY_C_DECL yyDecompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct );

	YY_API bool YY_C_DECL yyInitVideoDriver(const char* dl, yyWindow*);
	
	YY_API yyVideoDriverAPI* YY_C_DECL yyGetVideoDriverAPI();
	
	YY_API void YY_C_DECL yyUpdateAsyncLoader();

	typedef void* dll_handle;
	typedef void* dll_function;
	YY_API dll_handle YY_C_DECL yyLoadLibrary(const char* libraryName);
	YY_API void YY_C_DECL yyFreeLibrary(dll_handle);
	YY_API dll_function YY_C_DECL yyGetProcAddress(dll_handle,const char* functionName);
	
	/* pivotPosition
		0--1--2
		|  |  |
		7--8--3
		|  |  |
		6--5--4
	*/
	YY_API yySprite* YY_C_DECL yyCreateSprite(const v4f& rect, yyResource* texture, u8 pivotPosition = 0);
	YY_API yySprite2* YY_C_DECL yyCreateSprite2(yyResource* texture);

	YY_API void YY_C_DECL yySetMainWindow(yyWindow*);
	YY_API yyWindow* YY_C_DECL yyGetMainWindow();

	YY_API void YY_C_DECL yyAddEvent(const yyEvent&, bool unique);
	YY_API bool YY_C_DECL yyPollEvent(yyEvent&);

	// using in:
	//  - buttons without texture
	YY_API void YY_C_DECL yySetDefaultTexture(yyResource*);
	YY_API yyResource* YY_C_DECL yyGetDefaultTexture();

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