﻿#ifndef _YUYU_CURSOR_H_
#define _YUYU_CURSOR_H_

#ifdef YY_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#ifdef GetObject
#undef GetObject
#endif
#else
#error Need implementation
#endif

enum class yyCursorType : u32
{
	Arrow,
	Cross,
	Hand,
	Help,
	IBeam,
	No,
	Size,
	SizeNESW,
	SizeNS,
	SizeNWSE,
	SizeWE,
	UpArrow,
	Wait,

	_count
};

class yyCursor
{
	yyCursorType m_type;
public:
	yyCursor(yyCursorType ct) {
		m_type = ct;
#ifdef YY_PLATFORM_WINDOWS
		m_handle = 0;
#else
#error Need implementation
#endif
	}
	~yyCursor() {}

	void Activate() {
#ifdef YY_PLATFORM_WINDOWS
		::SetCursor(m_handle);
#else
#error Need implementation
#endif
	}

#ifdef YY_PLATFORM_WINDOWS
	HCURSOR m_handle;
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#else
#error Need implementation
#endif

	friend class yyWindow;
	friend class yyEngine;
};

#endif