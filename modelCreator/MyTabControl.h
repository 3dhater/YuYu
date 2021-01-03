#pragma once


// MyTabControl

class MyTabControl : public CTabCtrl
{
	DECLARE_DYNAMIC(MyTabControl)

public:
	MyTabControl();
	virtual ~MyTabControl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


