// MainFrm.h : interface of the CMainFrame class
//
#pragma once

class CCropperView;
class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
public:

// Operations
private:
    // Check registration key
    bool Initialize(void);

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

// Implementation
public:
	virtual ~CMainFrame();
	CCropperView* GetRightPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CDialogBar      m_wndDlgBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    afx_msg void OnViewNext();
    afx_msg void OnViewPrevious();
    afx_msg void OnPositionTop();
    afx_msg void OnUpdateStatusBarPos(CCmdUI *pCmdUI);
    afx_msg void OnUpdateStatusBarCrop(CCmdUI *pCmdUI);
    afx_msg void OnUpdateDlgBar();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnViewOriginals();
    afx_msg void OnViewProcessed();
    afx_msg void OnUpdateTrueSize(CCmdUI *pCmdUI);
    afx_msg void OnUpdateClientSize(CCmdUI *pCmdUI);
    afx_msg void OnUpdateToolBar(CCmdUI *pCmdUI);
    afx_msg void OnUpdateNextPrev(CCmdUI *pCmdUI);
    afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    virtual BOOL DestroyWindow();
    afx_msg void OnFileNew();
};


