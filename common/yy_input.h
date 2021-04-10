#ifndef _YUYU_INPUT_H_
#define _YUYU_INPUT_H_

#include "yy_keys.h"
#include "yy_window.h"

struct yyInputContext
{
	yyInputContext(){
		Reset();
		ResetHold();
		m_LMBClickCount = 0;
		m_RMBClickCount = 0;
		m_MMBClickCount = 0;
		m_X1MBClickCount = 0;
		m_X2MBClickCount = 0;
	}

	bool m_isLMBDown;
	bool m_isLMBHold;
	bool m_isLMBUp;
	bool m_isLMBDbl;

	bool m_isRMBDown;
	bool m_isRMBHold;
	bool m_isRMBUp;
	bool m_isRMBDbl;

	bool m_isMMBDown;
	bool m_isMMBHold;
	bool m_isMMBUp;
	bool m_isMMBDbl;

	bool m_isX1MBDown;
	bool m_isX1MBHold;
	bool m_isX1MBUp;
	bool m_isX1MBDbl;

	bool m_isX2MBDown;
	bool m_isX2MBHold;
	bool m_isX2MBUp;
	bool m_isX2MBDbl;

	v2f  m_cursorCoords;
	v2f  m_cursorCoordsForGUI;
//	v2f  m_cursorCoordsOld;
	v2f  m_mouseDelta;
	f32  m_wheelDelta;

	char16_t m_character;

	u8 m_key_hit[256];

	bool IsKeyHit(yyKey key){
		return m_key_hit[(u32)key] != 0;
	}

	u8 m_key_hold[256];

	bool IsKeyHold(yyKey key){
		return m_key_hold[(u32)key] != 0;
	}

	u8 m_key_pressed[256];

	bool IsKeyPressed(yyKey key){
		return m_key_pressed[(u32)key] != 0;
	}

	void ResetHold() {
		memset(m_key_hold, 0, sizeof(u8) * 256);
		m_isLMBHold = false;
		m_isRMBHold = false;
		m_isMMBHold = false;
		m_isX1MBHold = false;
		m_isX2MBHold = false;
	}
	void Reset() {
		m_character = 0;

		m_isLMBDown = false;
		m_isLMBUp = false;
		m_isLMBDbl = false;

		m_isRMBDown = false;
		m_isRMBUp = false;
		m_isRMBDbl = false;

		m_isMMBDown = false;
		m_isMMBUp = false;
		m_isMMBDbl = false;

		m_isX1MBDown = false;
		m_isX1MBUp = false;
		m_isX1MBDbl = false;

		m_isX2MBDown = false;
		m_isX2MBUp = false;
		m_isX2MBDbl = false;

		memset(m_key_pressed, 0, sizeof(u8) * 256);
		memset(m_key_hit, 0, sizeof(u8) * 256);
		//m_cursorCoords.set(0.f, 0.f);
		//m_cursorCoordsForGUI.set(0.f, 0.f);
		//m_cursorCoordsOld.set(0.f, 0.f);
		m_mouseDelta.set(0.f, 0.f);
		m_wheelDelta = 0.f;


		m_LMBMultipleClickTime = 0.3f;
		m_RMBMultipleClickTime = 0.3f;
		m_MMBMultipleClickTime = 0.3f;
		m_X1MBMultipleClickTime = 0.3f;
		m_X2MBMultipleClickTime = 0.3f;
	}

	f32 m_LMBMultipleClickTime;
	f32 m_RMBMultipleClickTime;
	f32 m_MMBMultipleClickTime;
	f32 m_X1MBMultipleClickTime;
	f32 m_X2MBMultipleClickTime;
	s32 m_LMBClickCount;
	s32 m_RMBClickCount;
	s32 m_MMBClickCount;
	s32 m_X1MBClickCount;
	s32 m_X2MBClickCount;
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

#endif