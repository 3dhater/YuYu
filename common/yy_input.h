#ifndef _YUYU_INPUT_H_
#define _YUYU_INPUT_H_

struct yyInputContext
{
	bool m_isLMBDown = false;
	bool m_isLMBHold = false;
	v2f  m_cursorCoords;
};

extern "C"
{
	//YY_API yyInputContext* YY_C_DECL yyGetInputContext();
}

#endif