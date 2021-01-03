
// modelCreator.h : main header file for the modelCreator application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CmodelCreatorApp:
// See modelCreator.cpp for the implementation of this class
//

class CmodelCreatorApp : public CWinApp
{
public:
	CmodelCreatorApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CmodelCreatorApp theApp;
