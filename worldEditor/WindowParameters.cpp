#include "yy.h"
#include "yy_window.h"
#include "WindowParameters.h"

WindowParameters::WindowParameters()
{
	m_window = 0;
}

WindowParameters::~WindowParameters()
{
	if (m_window)
	{
		yyDestroy(m_window);
	}
}
