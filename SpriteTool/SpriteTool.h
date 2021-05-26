#ifndef _SPRITETOOL_H_
#define _SPRITETOOL_H_

#include "yy.h"
#include "yy_input.h"
#include "yy_window.h"
#include "yy_gui.h"
#include "math\math.h"

class SpriteTool
{
	yyInputContext * m_inputContext;
	yyEngineContext* m_engineContext;
	yyWindow*        m_window;
	yyVideoDriverAPI* m_gpu;
	
	v2f m_gpuDepthRange;
	f32 m_dt;
	bool m_isCursorMove;
	bool m_isCursorInWindow;
	bool m_isCursorInGUI;
	bool m_isGUIInputFocus;
public:
	SpriteTool();
	~SpriteTool();

	bool Init(const char*);
	void MainLoop();
};

#endif