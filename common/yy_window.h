#ifndef _YUYU_WINDOW_H_
#define _YUYU_WINDOW_H_

#ifdef YY_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "math/vec.h"

using yyWindow_callback = void(*)(yyWindow*);
using yyWindow_callbackMouse = void(*)(yyWindow*, s32 wheel, s32 x, s32 y, u32 click);
using yyWindow_callbackKeyboard = void(*)(yyWindow*, bool isPress, u32 key, char16_t character);

enum yyWindow_mouseClickMask
{
	yyWindow_mouseClickMask_LMB_DOWN = BIT(0),
	yyWindow_mouseClickMask_RMB_DOWN = BIT(1),
	yyWindow_mouseClickMask_MMB_DOWN = BIT(2),
	yyWindow_mouseClickMask_X1MB_DOWN = BIT(3),
	yyWindow_mouseClickMask_X2MB_DOWN = BIT(4),
	yyWindow_mouseClickMask_X1MB_UP = BIT(5),
	yyWindow_mouseClickMask_X2MB_UP = BIT(6),
	yyWindow_mouseClickMask_X1MB_DOUBLE = BIT(7),
	yyWindow_mouseClickMask_X2MB_DOUBLE = BIT(8),
	yyWindow_mouseClickMask_LMB_UP = BIT(9),
	yyWindow_mouseClickMask_RMB_UP = BIT(10),
	yyWindow_mouseClickMask_MMB_UP = BIT(11),
	yyWindow_mouseClickMask_LMB_DOUBLE = BIT(12),
	yyWindow_mouseClickMask_RMB_DOUBLE = BIT(13),
	yyWindow_mouseClickMask_MMB_DOUBLE = BIT(14),
};


// implementation must located in game/Win32
// put files from this location into your project
class yyWindow
{
public:
	yyWindow();
	~yyWindow();

	bool init(int size_x, int size_y);

	// use it if you want ask user something before closing window
	yyWindow_callback m_onClose = nullptr;
	
	yyWindow_callback m_onShow = nullptr;
	yyWindow_callback m_onMove = nullptr;
	yyWindow_callback m_onFocusLost = nullptr;
	yyWindow_callback m_onFocusSet = nullptr;
	yyWindow_callback m_onActivate = nullptr;
	yyWindow_callback m_onPaint = nullptr;
	yyWindow_callback m_onSize = nullptr;
	yyWindow_callback m_onMinimize = nullptr;
	yyWindow_callback m_onRestore = nullptr;

	yyWindow_callbackMouse m_onMouseWheel = nullptr;
	yyWindow_callbackMouse m_onMouseMove  = nullptr;

	yyWindow_callbackKeyboard m_onKeyboard = nullptr;

	v2i m_size;
	v2i m_clientSize;

#ifdef YY_PLATFORM_WINDOWS
	HWND m_hWnd = nullptr;
	HDC  m_dc = nullptr;
	wchar_t m_class_name[32];
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

#endif