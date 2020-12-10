#ifndef _YUYU_PTR_H_
#define _YUYU_PTR_H_

template<typename type>
class yyPtr
{
public:
	yyPtr()
		:
		m_data(nullptr)
	{
	}

	yyPtr(type* ptr)
		:
		m_data(ptr)
	{
	}

	~yyPtr()
	{
		if(m_data) 
			yyDestroy(m_data);
	}

	type * m_data;

};

#endif