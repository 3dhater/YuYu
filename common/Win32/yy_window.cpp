#include "yy.h"

#ifdef YY_PLATFORM_WINDOWS

#include "yy_window.h"
#include "yy_keys.h"

s32 g_window_counter = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef IMGUI_API
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

v2i ClientResize(HWND hWnd, int nWidth, int nHeight);

yyWindow::yyWindow()
	:
	m_onClose(nullptr),
	m_onShow(nullptr),
	m_onMove(nullptr),
	m_onFocusLost(nullptr),
	m_onFocusSet(nullptr),
	m_onActivate(nullptr),
	m_onPaint(nullptr),
	m_onSize(nullptr),
	m_onMinimize(nullptr),
	m_onRestore(nullptr),
	m_onMouseWheel(nullptr),
	m_onMouseButton(nullptr),
	m_onKeyboard(nullptr)
{
	m_onRawInput = 0;
	m_onMaximize = 0;

	m_GPUData = 0;
	m_isFullscreen = false;
	m_hWnd = nullptr;
	m_dc = nullptr;
	m_oldStyle = 0;
	wsprintfW(m_class_name, L"Window%i", g_window_counter++);
	m_visible = false;
	m_title = "Window";

	
}

yyWindow::~yyWindow()
{
	yyLogWriteInfo("Destroy main window...\n");
	if(m_hWnd)
	{
		if( m_hWnd )
		{
			ReleaseDC(m_hWnd,m_dc);
			DestroyWindow( m_hWnd );
		}
		UnregisterClass( m_class_name, GetModuleHandle( 0 ) );
	}
}

void yyWindow::ToFullscreenMode()
{
	if (m_isFullscreen) return;
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	m_oldStyle = dwStyle;
	MONITORINFO mi = { sizeof(mi) };
	if (GetWindowPlacement(m_hWnd, &m_wndPlcmnt) &&
		GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &mi)) 
	{
		m_currentSize.x = mi.rcMonitor.right - mi.rcMonitor.left;
		m_currentSize.y = mi.rcMonitor.bottom - mi.rcMonitor.top;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hWnd, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		m_isFullscreen = true;
	}
}
void yyWindow::ToWindowMode()
{
	if (!m_isFullscreen) return;
	SetWindowLong(m_hWnd, GWL_STYLE, m_oldStyle);
	m_currentSize = m_creationSize;
	SetWindowPlacement(m_hWnd, &m_wndPlcmnt);
	SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	m_isFullscreen = false;
}

bool yyWindow::init(int size_x, int size_y, u32 flags, yyWindow* parent)
{
	yyLogWriteInfo("Init window...\n");

	m_creationSize.x = size_x;
	m_creationSize.y = size_y;

	//m_clientSize = m_creationSize;
	m_currentSize = m_creationSize;

	DWORD style = WS_OVERLAPPEDWINDOW;
	
	if (flags & yyWindowFlag_popup)
	{
		style = WS_POPUP;
	}
	else
	{
		if(flags & yyWindowFlag_noMinimizeButton)
			style ^= WS_MINIMIZEBOX;
		if (flags & yyWindowFlag_noMaximizeButton)
			style ^= WS_MAXIMIZEBOX;

		if (flags & yyWindowFlag_noResize)
		{
			style ^= WS_SIZEBOX;
			if (style & WS_MINIMIZEBOX)
				style ^= WS_MINIMIZEBOX;
			if (style & WS_MAXIMIZEBOX)
				style ^= WS_MAXIMIZEBOX;
		}
	}

	WNDCLASSEX wc;
	ZeroMemory( &wc, sizeof( wc ) ); // memset Winows style
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc		= WndProc;
	wc.hInstance		= GetModuleHandle( 0 );
	wc.hIcon			= nullptr;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_MENU+1);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= m_class_name;
	wc.hIconSm			= nullptr;
	if( !RegisterClassEx(&wc) )
	{
		YY_PRINT_FAILED;
		return false;
	}

	m_hWnd = CreateWindowExW( 0, m_class_name,
		(wchar_t*)m_title.to_string().c_str(),
		style,
		0,
		0,
		size_x,
		size_y,
		parent ? parent->m_hWnd : nullptr,
		nullptr,
		wc.hInstance,
		this );
	if( !m_hWnd )
	{
		YY_PRINT_FAILED;
		return false;
	}

	if ((flags & yyWindowFlag_hide) == 0)
	{
		this->Show();
	}

	MoveWindow(m_hWnd, 0, 0, size_x, size_y, FALSE);

	SetForegroundWindow( m_hWnd );
	::SetFocus( m_hWnd );
	UpdateWindow( m_hWnd );
	m_dc = GetDC(m_hWnd);

	RAWINPUTDEVICE device;
	device.usUsagePage = 0x01;
	device.usUsage = 0x02;
	device.dwFlags = 0;
	device.hwndTarget = 0;
	RegisterRawInputDevices(&device, 1, sizeof device);
	
	ClientResize(m_hWnd, m_currentSize.x, m_currentSize.y);

	return true;
}

v2i ClientResize(HWND hWnd, int nWidth, int nHeight)
{
	v2i size;
	RECT rcClient, rcWind;
	POINT ptDiff;
	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWind);
	ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
	ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
	MoveWindow(hWnd, rcWind.left, rcWind.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
	size.x = nWidth + ptDiff.x;
	size.y = nHeight + ptDiff.y;
	return size;
}

static unsigned int LocaleIdToCodepage(unsigned int lcid);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef IMGUI_API
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
#endif

	yyWindow* pD = nullptr;
	s32 wmId    = LOWORD(wParam);
	static HKL KEYBOARD_INPUT_HKL = 0;
	static u32 KEYBOARD_INPUT_CODEPAGE = 1252;

	if( message == WM_NCCREATE )
	{
		pD = static_cast<yyWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
		if( !SetWindowLongPtr(hWnd, -21, reinterpret_cast<LONG_PTR>(pD)) )
		{
			if( GetLastError() != 0 )
				return FALSE;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	else
		pD = reinterpret_cast<yyWindow*>(GetWindowLongPtr(hWnd, -21));

	switch( message )
	{
	case WM_GETMINMAXINFO:
	{
		if(pD)
		{ 
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 800;
			lpMMI->ptMinTrackSize.y = 600;
		}
	}break;
	case WM_ERASEBKGND:
		return 1;
	case WM_SHOWWINDOW:
	{
		if(pD)
		{
			if(pD->m_onShow)
				pD->m_onShow(pD);
		}
	}
	break;
	case WM_INPUT:
		if (pD)
		{
			if (pD->m_onRawInput)
				pD->m_onRawInput(pD,
					GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT,
					(HRAWINPUT)lParam);
		}
		break;
	case WM_MOVE:
	{
		if(pD)
		{
			if(pD->m_onMove)
				pD->m_onMove(pD);
		}
	}
	break;
	case WM_KILLFOCUS:
	{
		if(pD)
		{
			if(pD->m_onFocusLost)
				pD->m_onFocusLost(pD);
		}
	}break;
	case WM_CAPTURECHANGED:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCMBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCMOUSEHOVER:
	case WM_NCMOUSELEAVE:
	{
		break;
	}
	case WM_SETFOCUS:
	{
		if(pD)
		{
			if(pD->m_onFocusSet)
				pD->m_onFocusSet(pD);
		}
		break;
	}
	case WM_ACTIVATEAPP:
	{
		if(pD)
		{
			if(pD->m_onActivate)
				pD->m_onActivate(pD);
		}
		break;
	}
	case WM_PAINT:
	{
		if(pD)
		{
			if(pD->m_onPaint)
				pD->m_onPaint(pD);
		}
		break;
	}
	case WM_SIZE:
	{
		if(pD)
		{
			if(pD->m_onSize)
				pD->m_onSize(pD);
		}
		switch( wmId )
		{
		case SIZE_MAXIMIZED:
			if (pD)
			{
				if (pD->m_onMaximize)
					pD->m_onMaximize(pD);
			}
			break;
		case SIZE_MINIMIZED:
			if(pD)
			{
				if(pD->m_onMinimize)
					pD->m_onMinimize(pD);
			}
			break;
		case SIZE_RESTORED:
			if(pD)
			{
				if(pD->m_onRestore)
					pD->m_onRestore(pD);
			}
			break;
		}
		if( pD )
		{
			RECT rc;
			GetClientRect( hWnd, &rc );
			pD->m_currentSize.x = rc.right - rc.left;
			pD->m_currentSize.y = rc.bottom - rc.top;
	//		ClientResize(hWnd, pD->m_currentSize.x, pD->m_currentSize.y);

			/*pD->m_client_rect.x = rc.left;
			pD->m_client_rect.y = rc.top;
			pD->m_client_rect.z = rc.right;
			pD->m_client_rect.w = rc.bottom;

			pD->m_client_size.x = rc.right - rc.left;
			pD->m_client_size.y = rc.bottom - rc.top;
			Game_AddEvent( ev );*/
			return 1;
		}
		return 0;
	}break;

	case WM_SIZING:
	{
		if(pD)
		{
			if(pD->m_onSize)
				pD->m_onSize(pD);

			RECT rc;
			GetClientRect(hWnd, &rc);
			pD->m_currentSize.x = rc.right - rc.left;
			pD->m_currentSize.y = rc.bottom - rc.top;
		}
		//if( pD )
		//{
		//	/*RECT rc;
		//	GetClientRect( hWnd, &rc );
		//	pD->m_clientSize.x = rc.right - rc.left;
		//	pD->m_clientSize.y = rc.bottom - rc.top;*/
		////	ClientResize(hWnd, pD->m_currentSize.x, pD->m_currentSize.y);
		//	/*GetClientRect( hWnd, &rc );
		//	pD->m_client_rect.x = rc.left;
		//	pD->m_client_rect.y = rc.top;
		//	pD->m_client_rect.z = rc.right;
		//	pD->m_client_rect.w = rc.bottom;

		//	pD->m_client_size.x = rc.right - rc.left;
		//	pD->m_client_size.y = rc.bottom - rc.top;*/
		//}
	}
	break;
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
		if(pD)
		{
			if( pD->m_onClose )
			{
				pD->m_onClose(pD);
			}
			else
			{
				PostQuitMessage(0);
			}
		}
		else
		{
			PostQuitMessage(0);
		}
	return 0;

	case WM_NCMOUSEMOVE:
	{
		/*Game_Event ev;
		ev.type = Game_EventType::Mouse;
		ev.mouseEvent.state = 0u;
		ev.mouseEvent.x = -1;
		ev.mouseEvent.y = -1;
		Game_AddEvent( ev );*/
		return 0;
	}
	case WM_MOUSEWHEEL:
		if(pD)
		{
			if(pD->m_onMouseWheel)
				pD->m_onMouseWheel(pD, int( (f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA ), LOWORD(lParam), HIWORD(lParam), 0);
		}
		return 0;
	case WM_MOUSEMOVE:
	{
		if(pD)
		{
			//POINT point;
			//GetCursorPos(&point);
			//ScreenToClient(hWnd,&point);
			if(pD->m_onMouseButton)
				pD->m_onMouseButton(pD, 0, LOWORD(lParam), HIWORD(lParam), 0);
		}
	}return 0;
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	{
		u32 click = 0;

		if( wParam & MK_LBUTTON )		click |= yyWindow_mouseClickMask_LMB_DOWN;
		if( wParam & MK_RBUTTON )		click |= yyWindow_mouseClickMask_RMB_DOWN;
		if( wParam & MK_MBUTTON )		click |= yyWindow_mouseClickMask_MMB_DOWN;
		if( wParam & MK_XBUTTON1 )
		{
			click |= yyWindow_mouseClickMask_X1MB_DOWN;
			switch( message )
			{
				case WM_XBUTTONUP: click |= yyWindow_mouseClickMask_X1MB_UP; break;
				case WM_XBUTTONDBLCLK: click |= yyWindow_mouseClickMask_X1MB_DOUBLE; break;
			}
		}
		if( wParam & MK_XBUTTON2 )
		{
			click |= yyWindow_mouseClickMask_X2MB_DOWN;
			switch( message )
			{
				case WM_XBUTTONUP: click |= yyWindow_mouseClickMask_X2MB_UP; break;
				case WM_XBUTTONDBLCLK: click |= yyWindow_mouseClickMask_X2MB_DOUBLE; break;
			}
		}

		switch( message )
		{
		case WM_LBUTTONUP:{	
			click  |= yyWindow_mouseClickMask_LMB_UP; 
		}break;
			case WM_RBUTTONUP:	click |= yyWindow_mouseClickMask_RMB_UP; break;
			case WM_MBUTTONUP:	click |= yyWindow_mouseClickMask_MMB_UP; break;
			case WM_LBUTTONDBLCLK: click |= yyWindow_mouseClickMask_LMB_DOUBLE; break;
			case WM_RBUTTONDBLCLK: click |= yyWindow_mouseClickMask_RMB_DOUBLE; break;
			case WM_MBUTTONDBLCLK: click |= yyWindow_mouseClickMask_MMB_DOUBLE; break;
		}
		
		if(pD->m_onMouseButton)
			pD->m_onMouseButton(pD, 0, LOWORD(lParam), HIWORD(lParam), click);

		return 0;
	}break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool isPress = true;

		yyKey key = (yyKey)wParam;
		//ev.keyboardEvent.key = static_cast<Game_Key>( wParam );

		//if(message == WM_SYSKEYDOWN) ;
		//if(message == WM_KEYDOWN) ;
		if(message == WM_SYSKEYUP) isPress = false;
		if(message == WM_KEYUP) isPress = false;

		const UINT MY_MAPVK_VSC_TO_VK_EX = 3;

		if( key == yyKey::K_SHIFT )
		{ // shift -> lshift rshift
			key = static_cast<yyKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255u ), MY_MAPVK_VSC_TO_VK_EX ) );
		}
		if( key == yyKey::K_CTRL )
		{ // ctrl -> lctrl rctrl
			key = static_cast<yyKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255 ), MY_MAPVK_VSC_TO_VK_EX ) );
			if( lParam & 0x1000000 )
				key = static_cast<yyKey>( 163 );
		}

		if( key == yyKey::K_ALT )
		{ // alt -> lalt ralt
			key = static_cast<yyKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255 ), MY_MAPVK_VSC_TO_VK_EX ) );
			if( lParam & 0x1000000 )
				key = static_cast<yyKey>(165);
			//printf("alt = %i\n",(int)ev.keyboardEvent.key);
		}

		u8 keys[ 256u ];
		GetKeyboardState( keys );
		WORD chars[ 2 ];

		char16_t character = 0;

		if( ToAsciiEx( (UINT)wParam, HIWORD(lParam), keys, chars, 0, KEYBOARD_INPUT_HKL ) == 1 )
		{
			MultiByteToWideChar( KEYBOARD_INPUT_CODEPAGE, MB_PRECOMPOSED, (LPCSTR)chars,
				sizeof(chars), (WCHAR*)&character, 1 );
		}

		if(pD->m_onKeyboard)
			pD->m_onKeyboard(pD, isPress, (u32)key, character);

		if( message == WM_SYSKEYDOWN || message == WM_SYSKEYUP )
		{
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
		else
		{
			return 0;
		}
	}break;

	case WM_INPUTLANGCHANGE:
		KEYBOARD_INPUT_HKL = GetKeyboardLayout( 0 );
		KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage( LOWORD(KEYBOARD_INPUT_HKL) );
	return 0;

	case WM_SYSCOMMAND:
		if( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
			( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
			( wParam & 0xFFF0 ) == SC_KEYMENU
		)
		{
			return 0;
		}
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

// IrrLicht
// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
static unsigned int LocaleIdToCodepage(unsigned int lcid)
{
	switch ( lcid )
	{
		case 1098:  // Telugu
		case 1095:  // Gujarati
		case 1094:  // Punjabi
		case 1103:  // Sanskrit
		case 1111:  // Konkani
		case 1114:  // Syriac
		case 1099:  // Kannada
		case 1102:  // Marathi
		case 1125:  // Divehi
		case 1067:  // Armenian
		case 1081:  // Hindi
		case 1079:  // Georgian
		case 1097:  // Tamil
			return 0;
		case 1054:  // Thai
			return 874;
		case 1041:  // Japanese
			return 932;
		case 2052:  // Chinese (PRC)
		case 4100:  // Chinese (Singapore)
			return 936;
		case 1042:  // Korean
			return 949;
		case 5124:  // Chinese (Macau S.A.R.)
		case 3076:  // Chinese (Hong Kong S.A.R.)
		case 1028:  // Chinese (Taiwan)
			return 950;
		case 1048:  // Romanian
		case 1060:  // Slovenian
		case 1038:  // Hungarian
		case 1051:  // Slovak
		case 1045:  // Polish
		case 1052:  // Albanian
		case 2074:  // Serbian (Latin)
		case 1050:  // Croatian
		case 1029:  // Czech
			return 1250;
		case 1104:  // Mongolian (Cyrillic)
		case 1071:  // FYRO Macedonian
		case 2115:  // Uzbek (Cyrillic)
		case 1058:  // Ukrainian
		case 2092:  // Azeri (Cyrillic)
		case 1092:  // Tatar
		case 1087:  // Kazakh
		case 1059:  // Belarusian
		case 1088:  // Kyrgyz (Cyrillic)
		case 1026:  // Bulgarian
		case 3098:  // Serbian (Cyrillic)
		case 1049:  // Russian
			return 1251;
		case 8201:  // English (Jamaica)
		case 3084:  // French (Canada)
		case 1036:  // French (France)
		case 5132:  // French (Luxembourg)
		case 5129:  // English (New Zealand)
		case 6153:  // English (Ireland)
		case 1043:  // Dutch (Netherlands)
		case 9225:  // English (Caribbean)
		case 4108:  // French (Switzerland)
		case 4105:  // English (Canada)
		case 1110:  // Galician
		case 10249:  // English (Belize)
		case 3079:  // German (Austria)
		case 6156:  // French (Monaco)
		case 12297:  // English (Zimbabwe)
		case 1069:  // Basque
		case 2067:  // Dutch (Belgium)
		case 2060:  // French (Belgium)
		case 1035:  // Finnish
		case 1080:  // Faroese
		case 1031:  // German (Germany)
		case 3081:  // English (Australia)
		case 1033:  // English (United States)
		case 2057:  // English (United Kingdom)
		case 1027:  // Catalan
		case 11273:  // English (Trinidad)
		case 7177:  // English (South Africa)
		case 1030:  // Danish
		case 13321:  // English (Philippines)
		case 15370:  // Spanish (Paraguay)
		case 9226:  // Spanish (Colombia)
		case 5130:  // Spanish (Costa Rica)
		case 7178:  // Spanish (Dominican Republic)
		case 12298:  // Spanish (Ecuador)
		case 17418:  // Spanish (El Salvador)
		case 4106:  // Spanish (Guatemala)
		case 18442:  // Spanish (Honduras)
		case 3082:  // Spanish (International Sort)
		case 13322:  // Spanish (Chile)
		case 19466:  // Spanish (Nicaragua)
		case 2058:  // Spanish (Mexico)
		case 10250:  // Spanish (Peru)
		case 20490:  // Spanish (Puerto Rico)
		case 1034:  // Spanish (Traditional Sort)
		case 14346:  // Spanish (Uruguay)
		case 8202:  // Spanish (Venezuela)
		case 1089:  // Swahili
		case 1053:  // Swedish
		case 2077:  // Swedish (Finland)
		case 5127:  // German (Liechtenstein)
		case 1078:  // Afrikaans
		case 6154:  // Spanish (Panama)
		case 4103:  // German (Luxembourg)
		case 16394:  // Spanish (Bolivia)
		case 2055:  // German (Switzerland)
		case 1039:  // Icelandic
		case 1057:  // Indonesian
		case 1040:  // Italian (Italy)
		case 2064:  // Italian (Switzerland)
		case 2068:  // Norwegian (Nynorsk)
		case 11274:  // Spanish (Argentina)
		case 1046:  // Portuguese (Brazil)
		case 1044:  // Norwegian (Bokmal)
		case 1086:  // Malay (Malaysia)
		case 2110:  // Malay (Brunei Darussalam)
		case 2070:  // Portuguese (Portugal)
			return 1252;
		case 1032:  // Greek
			return 1253;
		case 1091:  // Uzbek (Latin)
		case 1068:  // Azeri (Latin)
		case 1055:  // Turkish
			return 1254;
		case 1037:  // Hebrew
			return 1255;
		case 5121:  // Arabic (Algeria)
		case 15361:  // Arabic (Bahrain)
		case 9217:  // Arabic (Yemen)
		case 3073:  // Arabic (Egypt)
		case 2049:  // Arabic (Iraq)
		case 11265:  // Arabic (Jordan)
		case 13313:  // Arabic (Kuwait)
		case 12289:  // Arabic (Lebanon)
		case 4097:  // Arabic (Libya)
		case 6145:  // Arabic (Morocco)
		case 8193:  // Arabic (Oman)
		case 16385:  // Arabic (Qatar)
		case 1025:  // Arabic (Saudi Arabia)
		case 10241:  // Arabic (Syria)
		case 14337:  // Arabic (U.A.E.)
		case 1065:  // Farsi
		case 1056:  // Urdu
		case 7169:  // Arabic (Tunisia)
			return 1256;
		case 1061:  // Estonian
		case 1062:  // Latvian
		case 1063:  // Lithuanian
			return 1257;
		case 1066:  // Vietnamese
			return 1258;
	}
	return 65001;   // utf-8
}

#endif