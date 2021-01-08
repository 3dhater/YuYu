#pragma once


// CEditFloat

class CEditFloat : public CEdit
{
	DECLARE_DYNAMIC(CEditFloat)

public:
	CEditFloat();
	virtual ~CEditFloat();

	void(*m_onCharEnter)(CEditFloat*);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};


