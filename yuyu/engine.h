#ifndef _YUYU_ENGINE_H_
#define _YUYU_ENGINE_H_

#include <zstd.h>

#include "yy_async.h"
#include "containers/fixed_fifo.h"
#include "containers/list.h"
#include "containers/array.h"

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

struct CacheNode
{
	CacheNode()
		:
		m_resource(nullptr)
	{}
	//std::filesystem::path m_path;
	yyStringA m_path;
	yyResource* m_resource;
};

class Engine
{
public:
	Engine();
	~Engine();

	yySceneObjectBase* m_sceneRootObject;
	yyCamera* m_sceneActiveCamera;

	yyInputContext* m_inputContext;

	yySystemState m_state;
	dl_handle m_videoDriverLib;
	
	typedef yyVideoDriverAPI*(*videoGetApi)();
	videoGetApi m_videoDriverGetApi;
	yyVideoDriverAPI* m_videoAPI;

	yyArraySmall<yyImageLoader> m_imageLoaders;
	yyArraySmall<yyModelLoader> m_modelLoaders;

	yyAsyncLoadEventHandler m_asyncEventHandler;
	std::thread* m_backgroundWorker;
	yyFixedFIFOThread<BackgroundWorkerCommands, 20> m_workerCommands;
	
	yyFixedFIFOThread<BackgroundWorkerResults, 20> m_workerResults;

	void addGuiElement(yyGUIElement*);
	yyList<yyGUIElement*> m_guiElements;

	ZSTD_CCtx* m_cctx;
	u8* compressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size);
	u8* decompressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size);

	std::vector<CacheNode> m_modelCache;
	std::vector<CacheNode> m_textureCache;
};

#endif