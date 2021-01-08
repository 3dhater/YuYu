// EditFloat.cpp : implementation file
//

#include "stdafx.h"
#include "modelCreator.h"
#include "EditFloat.h"


// CEditFloat

IMPLEMENT_DYNAMIC(CEditFloat, CEdit)

CEditFloat::CEditFloat()
{

}

CEditFloat::~CEditFloat()
{
}


BEGIN_MESSAGE_MAP(CEditFloat, CEdit)
	ON_WM_CHAR()
	ON_WM_KEYUP()
END_MESSAGE_MAP()



afx_msg void CEditFloat::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_onCharEnter) m_onCharEnter(this);
}
afx_msg void CEditFloat::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case L'0':
	case L'1':
	case L'2':
	case L'3':
	case L'4':
	case L'5':
	case L'6':
	case L'7':
	case L'8':
	case L'9':
	case L'-':
	case L'.': 
	{
	}break;
	default:
		return;
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

