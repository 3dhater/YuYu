#ifndef _YUYU_GUI_RANGESLIDER_H_
#define _YUYU_GUI_RANGESLIDER_H_

enum class yyGUIRangeSliderType : u32 {
	IntLimits
};

class yyGUIRangeSlider : public yyGUIElement
{

public:
	yyGUIRangeSlider();
	virtual ~yyGUIRangeSlider();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw();
	virtual void Rebuild();

	union {
		f32 m_minMax_f[2];
		s32 m_minMax_i[2];
	};
	union {
		f32 * m_ptr_f;
		s32 * m_ptr_i;
	};
	
	yyGUIRangeSliderType m_sliderType;
};

#endif