#ifndef __KK_KRGUI_GROUP_H__
#define __KK_KRGUI_GROUP_H__

namespace Kr
{
	namespace Gui
	{
		struct Group
		{
			Group()
			{
				m_scrollValue = 0.f;
				m_style = nullptr;
				m_contentHeight = 0;
				m_inRect = false;
			}

			float   m_scrollValue;
			Vec2f m_size;
			Style* m_style;
		

			// read only
			// не использовать. для внутреннего пользования
			Vec4f m_clipRect; // область для scissor. 
			float   m_contentHeight; // высота содержимого. должен учитываться во всех контролах которые будут внутри группы
			bool    m_inRect;
		};
	}
}

#endif