#include "yy.h"

#include <cassert>

class Engine
{
public:
	Engine(){}
	~Engine(){}

	yySystemState m_state = yySystemState::Run;
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

}