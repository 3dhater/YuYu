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

void yyGUIButton::OnUpdate()
{
	m_mouseInRect = math::pointInRect(
		g_engine->m_inputContext->m_cursorCoords.x,
		g_engine->m_inputContext->m_cursorCoords.y,
		m_rect);

	if (g_engine->m_inputContext->m_isLMBUp)
	{
		if (m_useAsCheckbox)
		{
		}
		else
		{
			m_isClicked = false;
		}


		if (m_onRelease && m_mouseInRect
			&& (g_engine->m_guiElementInMouseFocus == this))
			m_onRelease(this, m_id);

		g_engine->m_guiElementInMouseFocus = 0;
	}

	if (m_onClick && g_engine->m_inputContext->m_isLMBDown)
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

			m_onClick(this, m_id);
			g_engine->m_guiElementInMouseFocus = this;
		}
	}
}

void yyGUIButton::OnDraw()
{
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
	g_engine->addGuiElement(element);
	return element;
}

void yyGUIButton::SetVisible(bool v) 
{
	m_visible = v; 
}

void yyGUIButton::SetMouseHoverTexture(yyResource* t)
{
	if (!m_mouseHoverPB)
	{
		m_mouseHoverPB = yyGUICreatePictureBox(m_rect, t, -1);
		m_mouseHoverPB->SetVisible(false);
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
		m_mouseClickPB->SetVisible(false);
	}
	else
	{
		m_mouseClickPB->m_texture = t;
	}
}
