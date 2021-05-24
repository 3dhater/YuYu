#ifndef _YUYU_GUI_RANGESLIDER_H_
#define _YUYU_GUI_RANGESLIDER_H_

enum class yyGUIRangeSliderType : u32 {
	IntLimits,
	FloatLimits,
	Int,
	Float
};

class yyGUIRangeSlider : public yyGUIElement
{
	yyGUITextInput* m_text;
	v4f m_limitRectangle;
	void _calculate_limit_rectangle();
	void _checkLimits();

	friend void yyGUIRangeSlider_text_onRebuildSetRects(yyGUIElement* elem, s32 m_id);
	friend void yyGUIRangeSlider_text_onEnter(yyGUIElement* elem, s32 m_id);
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
	union {
		f32 m_old_f;
		s32 m_old_i;
	};

	                             // default:
	f32 m_valueMultiplerNormal;  // 1.f
	f32 m_valueMultiplerShift;   // 10.f
	f32 m_valueMultiplerAlt;     // 0.f
	
	yyGUIRangeSliderType m_sliderType;
	
	yyColor m_bgColorCurrent;
	yyColor m_bgColor;
	yyColor m_bgColorHover;
	yyColor m_limitRectColor;
	bool m_vertical;

	// enable text
	virtual void UseText(yyGUIFont*);

	// if you set m_ptr_f/m_ptr_i by yourself, call this function
	virtual void UpdateText();

	void(*m_onValueChanged)(yyGUIRangeSlider*);
};

#endif