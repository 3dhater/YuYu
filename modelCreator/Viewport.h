#pragma once



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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* pDC);
};


