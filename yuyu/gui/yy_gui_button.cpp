#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"
#include "math/math.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

yyGUIButton::yyGUIButton()
	:
	m_basePB(nullptr),
	m_mouseHoverPB(0),
	m_mouseClickPB(0),
	m_onRelease(0),
	m_onClick(nullptr)
{
	m_type = yyGUIElementType::Button;
	m_mouseInRect = false;
	m_isClicked = false;
	m_isChecked = false;
	m_useAsCheckbox = false;
}

yyGUIButton::~yyGUIButton()
{
	auto vAPI = yyGetVideoDriverAPI();
}

void yyGUIButton::SetOffset(const v2f& o)
{
	m_offset = o;
	if (m_mouseClickPB) m_mouseClickPB->m_offset = o;
	if (m_mouseHoverPB) m_mouseHoverPB->m_offset = o;
	if (m_basePB) m_basePB->m_offset = o;
}

void yyGUIButton::OnUpdate()
{
	if (!m_visible) return;
	if (m_ignoreInput) return;

	m_mouseInRect = math::pointInRect(
		g_engine->m_inputContext->m_cursorCoordsForGUI.x,
		g_engine->m_inputContext->m_cursorCoordsForGUI.y,
		v4f(
			m_rect.x + m_offset.x,  
			m_rect.y + m_offset.y,
			m_rect.z + m_offset.x,
			m_rect.w + m_offset.y
		)
		);

	if (g_engine->m_inputContext->m_isLMBUp)
	{
		if (!m_useAsCheckbox)
		{
			m_isClicked = false;
		}


		if (m_onRelease && m_mouseInRect
			&& (g_engine->m_guiElementInMouseFocus == (yyGUIElement*)this))
			m_onRelease(this, m_id);
		
	}

	if (g_engine->m_inputContext->m_isLMBDown)
	{
		if (m_mouseInRect)
		{
			if (m_useAsCheckbox)
			{
				if (m_isChecked)
				{
					m_isChecked = false;
				}
				else
				{
					m_isChecked = true;
				}
			}
			else
			{
				m_isClicked = true;
			}

			if(m_onClick)
				m_onClick(this, m_id);
			g_engine->m_guiElementInMouseFocus = this;
		}
	}
}

void yyGUIButton::OnDraw()
{
	if (!m_visible) return;
	if (m_isClicked || m_isChecked)
	{
		if (m_mouseClickPB)
		{
			m_mouseClickPB->OnDraw();
		}
		else
		{
			m_basePB->OnDraw();
		}
	}
	else
	{
		if (m_mouseHoverPB)
		{
			if (m_mouseInRect)
			{
				m_mouseHoverPB->OnDraw();
			}
			else
			{
				m_basePB->OnDraw();
			}
		}
		else
		{
			m_basePB->OnDraw();
		}
	}
}

YY_API yyGUIButton* YY_C_DECL yyGUICreateButton(const v4f& rect, yyResource* baseTexture, s32 id)
{
	yyGUIButton* element = yyCreate<yyGUIButton>();
	element->m_rect = rect;
	element->m_id = id;
	element->m_basePB = yyGUICreatePictureBox(rect, baseTexture, -1);
	element->m_basePB->IgnoreInput(true);
	yyGUIRemoveElement(element->m_basePB);
	g_engine->addGuiElement(element);
	return element;
}

void yyGUIButton::SetVisible(bool v) 
{
	m_visible = v; 
	//if (m_basePB) m_basePB->SetVisible(v);
}

void yyGUIButton::SetMouseHoverTexture(yyResource* t)
{
	if (!m_mouseHoverPB)
	{
		m_mouseHoverPB = yyGUICreatePictureBox(m_rect, t, -1);
		m_mouseHoverPB->IgnoreInput(true);
		yyGUIRemoveElement(m_mouseHoverPB);
		//m_mouseHoverPB->SetVisible(false);
	}
	else
	{
		m_mouseHoverPB->m_texture = t;
	}
}

void yyGUIButton::SetMouseClickTexture(yyResource* t)
{
	if (!m_mouseClickPB)
	{
		m_mouseClickPB = yyGUICreatePictureBox(m_rect, t, -1);
		m_mouseClickPB->IgnoreInput(true);
		yyGUIRemoveElement(m_mouseClickPB);
		//m_mouseClickPB->SetVisible(false);
	}
	else
	{
		m_mouseClickPB->m_texture = t;
	}
}
