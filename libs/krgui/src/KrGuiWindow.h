#ifndef __KK_KRGUI_WINDOW_H__
#define __KK_KRGUI_WINDOW_H__

namespace Kr
{
	namespace Gui
	{
		struct Window
		{
			Window()
			{
				OSWindow = nullptr;
				m_currentItemId_inRect = 0;
				m_lastItemId_inRect = 0;
			}

			void * OSWindow;
			int m_currentItemId_inRect;
			int m_lastItemId_inRect;
		};
	}
}

#endif