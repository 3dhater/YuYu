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

	BackgroundWorkerResults(){}
	BackgroundWorkerResults(type t)
	{
		m_type = t;
	}
	~BackgroundWorkerResults(){}
	
	type m_type = type::None;
	s32 m_id = 0;
	void* m_rawData = nullptr;
};

struct BackgroundWorkerCommands
{
	enum type
	{
		None,
		ExitThread,
		LoadImage,
	};

	BackgroundWorkerCommands(){}
	BackgroundWorkerCommands(type t, const char* fn, s32 id)
	{
		m_type = t;
		m_fileName = fn;
		m_id = id;
	}
	~BackgroundWorkerCommands(){}


	type m_type = type::None;
	
	const char* m_fileName = nullptr;
	s32 m_id = 0;
};

class Engine
{
public:
	Engine();
	~Engine();

	yySceneObjectBase* m_sceneRootObject = nullptr;
	yyCamera* m_sceneActiveCamera = nullptr;

	yyInputContext* m_inputContext = nullptr;

	yySystemState m_state = yySystemState::Run;
	dl_handle m_videoDriverLib = nullptr;
	
	using videoGetApi = yyVideoDriverAPI*(*)();
	videoGetApi m_videoDriverGetApi = nullptr;
	yyVideoDriverAPI* m_videoAPI = nullptr;

	yyArraySmall<yyImageLoader> m_imageLoaders;
	//yyResourceManager* m_resourceManager = nullptr;

	yyAsyncLoadEventHandler m_asyncEventHandler = nullptr;
	std::thread* m_backgroundWorker = nullptr;
	yyFixedFIFOThread<BackgroundWorkerCommands, 20> m_workerCommands;
	
	yyFixedFIFOThread<BackgroundWorkerResults, 20> m_workerResults;

	void addGuiElement(yyGUIElement*);
	yyList<yyGUIElement*> m_guiElements;

	ZSTD_CCtx* m_cctx = nullptr;
	u8* compressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size);
	u8* decompressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size);
};

#endif