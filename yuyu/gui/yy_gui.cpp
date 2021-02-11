#include "yy.h"
#include "yy_gui.h"
#include "io/file_io.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

YY_API void YY_C_DECL yyGUIDeleteElement(yyGUIElement* elem)
{
	g_engine->m_guiElements.erase_first(elem);
	yyDestroy(elem);
}
YY_API void YY_C_DECL yyGUIRemoveElement(yyGUIElement* elem)
{
	g_engine->m_guiElements.erase_first(elem);
}

YY_API void YY_C_DECL yyGUIUpdate(f32 deltaTime)
{
	auto guiElement = g_engine->m_guiElements.head();
	for(size_t i = 0, sz = g_engine->m_guiElements.size(); i < sz; ++i)
	{
		guiElement->m_data->OnUpdate();
		guiElement = guiElement->m_right;
	}

	if (g_engine->m_inputContext->m_isLMBUp)
	{
		g_engine->m_guiElementInMouseFocus = 0;
	}
}

YY_API void YY_C_DECL yyGUIDrawAll()
{
	g_engine->m_videoAPI->BeginDrawGUI();
	
	auto guiElement = g_engine->m_guiElements.head();
	for(size_t i = 0, sz = g_engine->m_guiElements.size(); i < sz; ++i)
	{
		g_engine->m_videoAPI->SetGUIShaderData(guiElement->m_data);
		guiElement->m_data->OnDraw();
		guiElement = guiElement->m_right;
	}

	g_engine->m_videoAPI->EndDrawGUI();
}

YY_API yyGUIElement* YY_C_DECL yyGUIGetElementInMouseFocus()
{
	return g_engine->m_guiElementInMouseFocus;
}