// MyTabControl.cpp : implementation file
//

#include "stdafx.h"
#include "modelCreator.h"
#include "MyTabControl.h"


// MyTabControl

IMPLEMENT_DYNAMIC(MyTabControl, CTabCtrl)

MyTabControl::MyTabControl()
	:
	CTabCtrl()
{

}

MyTabControl::~MyTabControl()
{
}


BEGIN_MESSAGE_MAP(MyTabControl, CTabCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// MyTabControl message handlers
afx_msg int MyTabControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	TCITEM newItem;
	newItem.mask = TCIF_TEXT;
	newItem.pszText = L"Layers";
	InsertItem(0, &newItem);

	return 0;
}

