#include "yy.h"
#include "yy_window.h"

#include "yy_ptr.h"

struct EngineContext
{
	EngineContext()
	{
		m_state = yyStart();
	}
	~EngineContext()
	{
		yyStop();
	}

	yySystemState * m_state = nullptr;
};

void window_onCLose(yyWindow* window)
{
	yyQuit();
}

int main()
{
	yyPtr<EngineContext> engineContext = new EngineContext;
	auto p_engineContext = engineContext.m_data;

	// use yyPtr if you want auto destroy window
	yyPtr<yyWindow> window = new yyWindow;
	auto p_window = window.m_data;

	if(!p_window->init(800,600))
	{
		YY_PRINT_FAILED;
		return 1;
	}

	p_window->m_onClose = window_onCLose;
	
	bool run = true;
	while( run )
	{
#ifdef YY_PLATFORM_WINDOWS
		MSG msg;
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			GetMessage( &msg, NULL, 0, 0 );
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
#else
#error For windows
#endif


		switch(*p_engineContext->m_state)
		{
		default:
			run = false;
			break;
		case yySystemState::Run:
			break;
		}
	}

	return 0;
}