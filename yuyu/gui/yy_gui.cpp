#include "yy.h"
#include "yy_gui.h"
#include "io/file_io.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

bool pointInRect( float x, float y, const v4f& rect )
{
	if(x>rect.x){if(x<rect.z){if(y>rect.y){if(y<rect.w){return true;}}}}
	return false;
}

YY_API void YY_C_DECL yyGUIDeleteElement(yyGUIElement* elem)
{
	g_engine->m_guiElements.erase_first(elem);
	yyDestroy(elem);
	/*for( auto & e : g_engine->m_guiElements )
	{
		if (e.m_data == elem)
		{
		}
	}*/
}
YY_API void YY_C_DECL yyGUIUpdate(f32 deltaTime)
{
	auto guiElement = g_engine->m_guiElements.head();
	for(size_t i = 0, sz = g_engine->m_guiElements.size(); i < sz; ++i)
	{
		switch (guiElement->m_data->m_type)
		{
		default:
			break;
		case yyGUIElementType::PictureBox:
		{
			yyGUIPictureBox* pictureBox = (yyGUIPictureBox*)guiElement->m_data;
			bool inRect = pointInRect(
				g_engine->m_inputContext->m_cursorCoords.x, 
				g_engine->m_inputContext->m_cursorCoords.y, 
				pictureBox->m_rect);

			if(pictureBox->m_onClick && g_engine->m_inputContext->m_isLMBDown)
			{
				if(inRect)
					pictureBox->m_onClick(guiElement->m_data, guiElement->m_data->m_id);
			}
		}break;
		}
		guiElement = guiElement->m_right;
	}
}

YY_API void YY_C_DECL yyGUIDrawAll()
{
	g_engine->m_videoAPI->BeginDrawGUI();
	
	auto guiElement = g_engine->m_guiElements.head();
	for(size_t i = 0, sz = g_engine->m_guiElements.size(); i < sz; ++i)
	{
		g_engine->m_videoAPI->SetGUIShaderData(guiElement->m_data);

		if (guiElement->m_data->m_visible)
		{

			switch (guiElement->m_data->m_type)
			{
			default:
				break;
			case yyGUIElementType::PictureBox:
			{
				yyGUIPictureBox* pictureBox = (yyGUIPictureBox*)guiElement->m_data;
				if (pictureBox->m_texture)
					g_engine->m_videoAPI->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, pictureBox->m_texture);
				if (pictureBox->m_pictureBoxModel)
					g_engine->m_videoAPI->SetModel(pictureBox->m_pictureBoxModel);
				g_engine->m_videoAPI->Draw();
			}break;
			case yyGUIElementType::Text:
			{
				yyGUIText* text = (yyGUIText*)guiElement->m_data;
				for (u16 k = 0, ksz = text->m_drawNodes.m_size; k < ksz; ++k)
				{
					auto & dn = text->m_drawNodes.m_data[k];
					if (dn.m_texture)
						g_engine->m_videoAPI->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, dn.m_texture);
					if (dn.m_model)
						g_engine->m_videoAPI->SetModel(dn.m_model);
					g_engine->m_videoAPI->Draw();
				}
			}break;
			}
		}
		guiElement = guiElement->m_right;
	}

	g_engine->m_videoAPI->EndDrawGUI();
}

