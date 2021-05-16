#ifndef _YUYU_WINDOW_H_
#define _YUYU_WINDOW_H_

#ifdef YY_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#include "math/vec.h"

typedef void(*yyWindow_callback)(yyWindow*);
typedef void(*yyWindow_callbackMouse)(yyWindow*, s32 wheel, s32 x, s32 y, u32 click);
typedef void(*yyWindow_callbackKeyboard)(yyWindow*, bool isPress, u32 key, char16_t character);

enum yyWindowFlags
{
	yyWindowFlag_noMinimizeButton = BIT(0),
	yyWindowFlag_noMaximizeButton = BIT(1),
	yyWindowFlag_hide = BIT(2),
	yyWindowFlag_noResize = BIT(3),
	yyWindowFlag_popup = BIT(4),
};

// implementation must located in game/Win32
// put files from this location into your project
class yyWindow
{
	void _set_current_rect() {
		m_currentRect.z = (f32)m_currentSize.x;
		m_currentRect.w = (f32)m_currentSize.y;
	}
public:
	yyWindow();
	~yyWindow();

	bool init(int size_x, int size_y, u32 flags, yyWindow* parent = 0);

	// use it if you want ask user something before closing window
	yyWindow_callback m_onClose;
	
	void(*m_onCommand)(s32); // WM_COMMAND 

	yyWindow_callback m_onShow;
	yyWindow_callback m_onMove;
	//yyWindow_callback m_onFocusLost;
	//yyWindow_callback m_onFocusSet;
	yyWindow_callback m_onActivate;
	yyWindow_callback m_onDeactivate;
	yyWindow_callback m_onPaint;
	yyWindow_callback m_onSize;
	yyWindow_callback m_onMinimize;
	yyWindow_callback m_onMaximize;
	yyWindow_callback m_onRestore;

	//void(*m_onRawInput)(yyWindow*, bool inForeground, void* rawInput);

	//yyWindow_callbackMouse m_onMouseWheel;
	//yyWindow_callbackMouse m_onMouseButton;
	//yyWindow_callbackKeyboard m_onKeyboard;

	v2i m_currentSize;
	v4f m_currentRect;
	v2i m_oldSize;
	v2i m_creationSize;
	//v2i m_clientSize;

	// for example for swapChain in d3d11
	// delete it only in video driver
	void * m_GPUData;

	bool m_isFullscreen;
	void ToFullscreenMode();
	void ToWindowMode();

#ifdef YY_PLATFORM_WINDOWS
	HWND m_hWnd;
	WINDOWPLACEMENT m_wndPlcmnt;
	HDC  m_dc;
	DWORD m_oldStyle;
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

	bool IsInFocus()
	{
#ifdef YY_PLATFORM_WINDOWS
		return GetActiveWindow() == m_hWnd;
#else
#error Need implementation
#endif
	}

	void SetFocus()
	{
#ifdef YY_PLATFORM_WINDOWS
		SetForegroundWindow(m_hWnd);
		::SetFocus(m_hWnd);
#else
#error Need implementation
#endif
	}

	yyStringA m_title;
	void SetTitle(const char* title)
	{
		m_title = title;
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