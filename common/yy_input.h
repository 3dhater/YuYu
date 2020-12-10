#ifndef _YUYU_INPUT_H_
#define _YUYU_INPUT_H_

#include "yy_keys.h"

struct yyInputContext
{
	yyInputContext()
		:
		m_isLMBDown(false),
		m_isLMBHold(false)
	{
		memset(m_key_hold, 0, sizeof(u8) * 256);
	}

	bool m_isLMBDown;
	bool m_isLMBHold;
	v2f  m_cursorCoords;

	u8 m_key_hold[256];
	bool isKeyHold(yyKey key)
	{
		return m_key_hold[(u32)key] != 0;
	}
};

extern "C"
{
	//YY_API yyInputContext* YY_C_DECL yyGetInputContext();
}

#endif