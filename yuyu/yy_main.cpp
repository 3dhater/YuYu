#include "yy.h"

#include <cassert>

#include "yy_resource.h"
#include "yy_async.h"
#include "yy_gui.h"

#include "engine.h"

#include <thread>

Engine * g_engine = nullptr;

Engine::Engine()
{
	yyImageLoader loader;
	loader.ext = ".dds";
	loader.image_loader_callback = ImageLoader_DDS;
	m_imageLoaders.push_back(loader);

	loader.ext = ".png";
	loader.image_loader_callback = ImageLoader_PNG;
	m_imageLoaders.push_back(loader);
}
Engine::~Engine()
{
	auto guiNode = m_guiElements.head();
	if(guiNode)
	{
		for(size_t i = 0, sz = m_guiElements.size(); i < sz; ++i)
		{
			yyDestroy( guiNode->m_data );
			guiNode = guiNode->m_right;
		}
	}

	if(m_videoDriverLib)
	{
		m_videoAPI->Destroy();

		yyFreeLybrary(m_videoDriverLib);
		m_videoDriverLib = nullptr;
	}
}

void Engine::addGuiElement(yyGUIElement* el)
{
	m_guiElements.push_back(el);
}

class EngineDestroyer
{
public:
	EngineDestroyer(){}
	~EngineDestroyer()
	{
		if(g_engine) // Visual Studio want to delete g_engine more than 1 time
		{
			yyDestroy( g_engine );
			g_engine = nullptr;
		}
	}
};
EngineDestroyer g_engineDestroyer;

extern "C"
{

YY_API yySystemState* YY_C_DECL yyStart()
{
	assert(!g_engine);
	g_engine = yyCreate<Engine>();
	//g_engine->m_resourceManager = new yyResourceManager;
	g_engine->m_backgroundWorker = new std::thread(yyBackgroundWorkerFunction);
	return &g_engine->m_state;
}

YY_API void YY_C_DECL yyStop()
{
	if(g_engine)	
	{
		if(g_engine->m_backgroundWorker)
		{
			g_engine->m_workerCommands.put(BackgroundWorkerCommands(BackgroundWorkerCommands::type::ExitThread,0,0));
			g_engine->m_backgroundWorker->join();
			delete g_engine->m_backgroundWorker;
		}

		/*if(g_engine->m_resourceManager)
		{
			delete g_engine->m_resourceManager;
			g_engine->m_resourceManager = nullptr;
		}*/

		

		yyDestroy( g_engine );
		g_engine = nullptr;
	}
}

YY_API void YY_C_DECL yyQuit()
{
	if(g_engine)
	{
		g_engine->m_state = yySystemState::Quit;
	}
}

YY_API void YY_C_DECL yySetAsyncLoadEventHandler(yyAsyncLoadEventHandler f)
{
	g_engine->m_asyncEventHandler = f;
}
YY_API void YY_C_DECL yyUpdateAsyncLoader()
{
	if(!g_engine->m_asyncEventHandler)
		return;

	for(int i = 0, sz = g_engine->m_workerResults.m_size; i < sz; ++i)
	{
		auto & obj = g_engine->m_workerResults.get();
		switch (obj.m_type)
		{
		default:
			break;
		case BackgroundWorkerResults::type::None:
			break;
		case BackgroundWorkerResults::type::LoadImage:
			g_engine->m_asyncEventHandler( obj.m_id, yyResource(), obj.m_rawData );
			break;
		}
	}
}

YY_API yyVideoDriverAPI* YY_C_DECL yyGetVideoDriverAPI()
{
	return g_engine->m_videoAPI;
}

YY_API bool YY_C_DECL yyInitVideoDriver(const char* dl, yyWindow* window)
{
	if(g_engine)
	{
		if(g_engine->m_videoDriverLib)
		{
			yyLogWriteWarning("Video driver already initialized\n");
			return false;
		}

		g_engine->m_videoDriverLib = yyLoadLybrary(dl);
		if(!g_engine->m_videoDriverLib)
		{
			yyLogWriteWarning("Can't load video driver library\n");
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		g_engine->m_videoDriverGetApi = (Engine::videoGetApi)yyGetProcAddress(g_engine->m_videoDriverLib, "GetAPI");
		if(!g_engine->m_videoDriverGetApi)
		{
			yyLogWriteWarning("Can't get GetApi function from video driver library\n");
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		g_engine->m_videoAPI = g_engine->m_videoDriverGetApi();
		if(!g_engine->m_videoAPI)
		{
			yyLogWriteWarning("Can't get video driver API\n");
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		if(g_engine->m_videoAPI->GetAPIVersion() != yyVideoDriverAPIVersion)
		{
			yyLogWriteWarning("Wrong Video Driver API version; %u, need %u\n", g_engine->m_videoAPI->GetAPIVersion(), yyVideoDriverAPIVersion);
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		//g_engine->m_resourceManager->m_videoDriverAPI = g_engine->m_videoAPI;

		return g_engine->m_videoAPI->Init(window);
	}
	return false;
}

}