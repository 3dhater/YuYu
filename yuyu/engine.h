#ifndef _YUYU_ENGINE_H_
#define _YUYU_ENGINE_H_

#include <zstd.h>

#include "yy_async.h"
#include "containers/fixed_fifo.h"
#include "containers/list.h"
#include "containers/array.h"
#include "yy_fs.h"
#include "yyResourceImpl.h"

void yyBackgroundWorkerFunction();

struct BackgroundWorkerResults
{
	enum type
	{
		None,
		LoadImage,
	};

	BackgroundWorkerResults()
	:
		m_type(type::None),
		m_id(-1),
		m_data(nullptr)
	{}
	BackgroundWorkerResults(type t)
		:
		m_type(t),
		m_id(-1),
		m_data(nullptr)
	{}

	~BackgroundWorkerResults(){}
	
	type m_type;
	s32 m_id;
	void* m_data;
};

struct BackgroundWorkerCommands
{
	enum type
	{
		None,
		ExitThread,
		LoadImage,
	};

	BackgroundWorkerCommands()
	:
		m_type(type::None),
		m_fileName(nullptr),
		m_id(-1)
	{}
	BackgroundWorkerCommands(type t, const char* fn, s32 id)
	:
		m_type(t),
		m_fileName(fn),
		m_id(id)
	{}
	~BackgroundWorkerCommands(){}


	type m_type;
	const char* m_fileName ;
	s32 m_id;
};

template<typename T>
struct CacheNode
{
	CacheNode()
		:
		m_resource(nullptr)
	{}
	yy_fs::path m_path;
	T* m_resource;
};

struct IFileSaveDialog;
struct IFileOpenDialog;

#define YY_EVENT_MAX 30

class Engine
{
	void GUIRebuildElement(yyGUIElement*);
public:
	Engine(yyPoolSetup*);
	~Engine();

	yyPoolSetup m_poolSetup;
	yyPoolAllocator<yyResourceImpl> m_poolResource;
	yyPoolAllocator<yyMaterial> m_poolMaterial;

	yyInputContext* m_inputContext;

	yySystemState m_state;
	dll_handle m_videoDriverLib;
	
	typedef yyVideoDriverAPI*(*videoGetApi)();
	videoGetApi m_videoDriverGetApi;
	yyVideoDriverAPI* m_videoAPI;

	void AddEvent(const yyEvent&, bool unique);
	bool PollEvent(yyEvent&);
	yyEvent m_events[YY_EVENT_MAX];
	u32 m_events_num;
	u32 m_events_current;

	yyArraySmall<yyImageLoader> m_imageLoaders;

	s32 m_textureAnisotropicLevel;
	yyTextureAddressMode m_textureAddressMode;
	yyTextureComparisonFunc m_textureComparisonFunc;
	yyTextureFilter m_textureFilter;
	bool m_useMipmaps;

	yyAsyncLoadEventHandler m_asyncEventHandler;
	std::thread* m_backgroundWorker;
	yyFixedFIFOThread<BackgroundWorkerCommands, 20> m_workerCommands;
	
	yyFixedFIFOThread<BackgroundWorkerResults, 20> m_workerResults;

	//void addGuiElement(yyGUIElement*, yyGUIDrawGroup*);
	//yyList<yyGUIElement*> m_guiElements;
	yyGUIDrawGroup* m_mainGUIDrawGroup;
	yyList<yyGUIDrawGroup*> m_GUIDrawGroups;
	void GUIUpdateDrawGroup(yyGUIDrawGroup*, f32 dt);
	void GUIDrawDrawGroup(yyGUIDrawGroup*);
	void GUIRebuildDrawGroup(yyGUIDrawGroup*);
	bool m_cursorInGUI;
	// input only elements on top
	bool m_guiIgnoreUpdateInput;
	yyGUIElement* m_guiElementInMouseFocus;

	ZSTD_CCtx* m_cctx;
	u8* compressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size);
	u8* decompressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size);

	//std::vector<CacheNode<yyResource>> m_textureCache;
	yyList<CacheNode<yyResource>> m_textureCache;
	yyList<CacheNode<yyMDL>> m_modelCache;

	yyStringW m_workingDir;

	yyResource* m_defaultTexture;

#ifdef YY_PLATFORM_WINDOWS
	IFileSaveDialog * m_fileSaveDialog;
	IFileOpenDialog * m_fileOpenDialog;
#endif
};

#endif