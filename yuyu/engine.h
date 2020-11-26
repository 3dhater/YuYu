#ifndef _YUYU_ENGINE_H_
#define _YUYU_ENGINE_H_

#include "yy_async.h"
#include "containers/fixed_fifo.h"
#include "containers/list.h"

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
		if(fn)
			m_fileName = fn;
		m_id = id;
	}
	~BackgroundWorkerCommands(){}


	type m_type = type::None;
	
	std::string m_fileName;
	s32 m_id = 0;
};

class Engine
{
public:
	Engine();
	~Engine();

	yySystemState m_state = yySystemState::Run;
	dl_handle m_videoDriverLib = nullptr;
	
	using videoGetApi = yyVideoDriverAPI*(*)();
	videoGetApi m_videoDriverGetApi = nullptr;
	yyVideoDriverAPI* m_videoAPI = nullptr;

	std::vector<yyImageLoader> m_imageLoaders;
	//yyResourceManager* m_resourceManager = nullptr;

	yyAsyncLoadEventHandler m_asyncEventHandler = nullptr;
	std::thread* m_backgroundWorker = nullptr;
	yyFixedFIFOThread<BackgroundWorkerCommands, 20> m_workerCommands;
	
	yyFixedFIFOThread<BackgroundWorkerResults, 20> m_workerResults;

	void addGuiElement(yyGUIElement*);
	yyList<yyGUIElement*> m_guiElements;
};

#endif