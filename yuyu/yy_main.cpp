#include "yy.h"

#include <cassert>

class Engine
{
public:
	Engine(){}
	~Engine(){}

	yySystemState m_state = yySystemState::Run;
	dl_handle m_videoDriverLib = nullptr;
	
	using videoGetApi = yyVideoDriverAPI*(*)();
	videoGetApi m_videoDriverGetApi = nullptr;
	yyVideoDriverAPI* n_videoAPI = nullptr;
};

Engine * g_engine = nullptr;

class EngineDestroyer
{
public:
	EngineDestroyer(){}
	~EngineDestroyer()
	{
		if(g_engine) // Visual Studio want to delete g_engine more than 1 time
		{
			delete g_engine;
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
	g_engine = new Engine;
	return &g_engine->m_state;
}

YY_API void YY_C_DECL yyStop()
{
	if(g_engine)
	{
		if(g_engine->m_videoDriverLib)
		{
			g_engine->n_videoAPI->Destroy();

			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
		}

		delete g_engine;
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

YY_API yyVideoDriverAPI* YY_C_DECL yyGetVideoDriverAPI()
{
	return g_engine->n_videoAPI;
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

		g_engine->n_videoAPI = g_engine->m_videoDriverGetApi();
		if(!g_engine->n_videoAPI)
		{
			yyLogWriteWarning("Can't get video driver API\n");
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		if(g_engine->n_videoAPI->GetAPIVersion() != yyVideoDriverAPIVersion)
		{
			yyLogWriteWarning("Wrong Video Driver API version; %u, need %u\n", g_engine->n_videoAPI->GetAPIVersion(), yyVideoDriverAPIVersion);
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		return g_engine->n_videoAPI->Init(window);
	}
	return false;
}

}