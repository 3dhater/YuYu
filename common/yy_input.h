#ifndef _YUYU_INPUT_H_
#define _YUYU_INPUT_H_

#include "yy_keys.h"
#include "yy_window.h"

struct yyInputContext
{
	yyInputContext()
		:
		m_isLMBDown(false),
		m_isLMBHold(false),
		m_isRMBDown(false),
		m_isRMBHold(false)
		{
		memset(m_key_hold, 0, sizeof(u8) * 256);
		memset(m_key_pressed, 0, sizeof(u8) * 256);
		m_isLMBUp = false;
		m_isLMBDbl = false;
	}

	bool m_isLMBDown;
	bool m_isLMBDbl;
	bool m_isLMBHold;
	bool m_isLMBUp;
	bool m_isRMBDown;
	bool m_isRMBHold;
	v2f  m_cursorCoords;
	v2f  m_cursorCoordsOld;
	v2f  m_mouseDelta;

	u8 m_key_hold[256];
	bool isKeyHold(yyKey key)
	{
		return m_key_hold[(u32)key] != 0;
	}

	u8 m_key_pressed[256];
	bool isKeyPressed(yyKey key)
	{
		return m_key_pressed[(u32)key] != 0;
	}
};

YY_FORCE_INLINE void yySetCursorPosition(f32 x, f32 y, yyWindow* window)
{
#ifdef YY_PLATFORM_WINDOWS
	RECT clientRect;
	GetClientRect(window->m_hWnd, &clientRect);
	ClientToScreen(window->m_hWnd, (LPPOINT)&clientRect.left);
	ClientToScreen(window->m_hWnd, (LPPOINT)&clientRect.right);

	POINT point;
	point.x = (int)x + clientRect.left;
	point.y = (int)y + clientRect.top;

	ScreenToClient(window->m_hWnd, &point);

	SetCursorPos(clientRect.left + point.x, clientRect.top + point.y);
#else
#error Need implementation
#endif
}

extern "C"
{
	//YY_API yyInputContext* YY_C_DECL yyGetInputContext();
}

#endif