#pragma once

#include "math\vec.h"

// CViewport form view
struct yyVideoDriverAPI;
struct yyCamera;
class CViewport : public CFormView
{
	DECLARE_DYNCREATE(CViewport)

protected:
	CViewport();           // protected constructor used by dynamic creation
	virtual ~CViewport();


	yyVideoDriverAPI * m_gpu;
	yyCamera* m_camera;
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEW1 };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	void InitForGPUDraw();
	void Draw();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseLeave();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	void _updateCamera();
	v4f m_cameraRotation;
	v4f m_cameraPosition;
	CPoint m_old_point;

};


