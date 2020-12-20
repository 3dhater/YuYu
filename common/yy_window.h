#ifndef _YUYU_WINDOW_H_
#define _YUYU_WINDOW_H_

#ifdef YY_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "math/vec.h"

typedef void(*yyWindow_callback)(yyWindow*);
typedef void(*yyWindow_callbackMouse)(yyWindow*, s32 wheel, s32 x, s32 y, u32 click);
typedef void(*yyWindow_callbackKeyboard)(yyWindow*, bool isPress, u32 key, char16_t character);

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

enum yyWindowFlags
{
	yyWindowFlag_noMinimizeButton = BIT(0),
	yyWindowFlag_hide = BIT(1),
};

// implementation must located in game/Win32
// put files from this location into your project
class yyWindow
{
public:
	yyWindow();
	~yyWindow();

	bool init(int size_x, int size_y, u32 flags, yyWindow* parent = 0);

	// use it if you want ask user something before closing window
	yyWindow_callback m_onClose;
	
	yyWindow_callback m_onShow;
	yyWindow_callback m_onMove;
	yyWindow_callback m_onFocusLost;
	yyWindow_callback m_onFocusSet;
	yyWindow_callback m_onActivate;
	yyWindow_callback m_onPaint;
	yyWindow_callback m_onSize;
	yyWindow_callback m_onMinimize;
	yyWindow_callback m_onRestore;

	yyWindow_callbackMouse m_onMouseWheel;
	yyWindow_callbackMouse m_onMouseButton;

	yyWindow_callbackKeyboard m_onKeyboard;

	v2i m_size;
	v2i m_clientSize;

#ifdef YY_PLATFORM_WINDOWS
	HWND m_hWnd;
	HDC  m_dc;
	wchar_t m_class_name[32];
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

	void Show()
	{
#ifdef YY_PLATFORM_WINDOWS
		ShowWindow(m_hWnd, SW_SHOWNORMAL);
#else
#error Need implementation
#endif
		m_visible = true;
	}

	void Hide()
	{
#ifdef YY_PLATFORM_WINDOWS
		ShowWindow(m_hWnd, SW_HIDE);
#else
#error Need implementation
#endif
		m_visible = false;
	}

	bool m_visible;

	void SetFocus()
	{
#ifdef YY_PLATFORM_WINDOWS
		SetForegroundWindow(m_hWnd);
		::SetFocus(m_hWnd);
#else
#error Need implementation
#endif
	}

	void SetTitle(const char* title)
	{
#ifdef YY_PLATFORM_WINDOWS
		SetWindowTextA(m_hWnd, title);
#else
#error Need implementation
#endif
	}

	void* GetHandle()
	{
#ifdef YY_PLATFORM_WINDOWS
		return m_hWnd;
#else
#error Need implementation
#endif
	}
};

#endif