// CropperView.h : interface of the CCropperView class
//
#pragma once

const int nNONE = 0;
const int nMOVE = 1;
const int nDRAW = 2;
const int nTOP = 3;
const int nBOTTOM = 4;
const int nLEFT = 5;
const int nRIGHT = 6;
const int nTOPLEFT = 7;
const int nTOPRIGHT = 8;
const int nBOTTOMLEFT = 9;
const int nBOTTOMRIGHT = 10;

class CCropperView : public CScrollView
{
protected: // create from serialization only
	CCropperView();
	DECLARE_DYNCREATE(CCropperView)

// Attributes
public:
    void InvertCropArea(CRect oRect);
    int CursorType();
	CCropperDoc* GetDocument() const;

// Operations
private:
    int m_nOperation;
    CPoint m_oMoveTo;

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CCropperView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

public:
    void OnUpdateStatusBarPos(CCmdUI *pCmdUI);
    void OnUpdateStatusBarCrop(CCmdUI *pCmdUI);
    bool IsCropping(void);

    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
    // // Is the current position inside the image?
    BOOL InPicture(CPoint& point);
    bool InitViewArea(CDC* pDC, int nState);

public:
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnViewNext();
    afx_msg void OnViewPrevious();
    afx_msg void OnRestorePicture();
};

#ifndef _DEBUG  // debug version in CropperView.cpp
inline CCropperDoc* CCropperView::GetDocument() const
   { return reinterpret_cast<CCropperDoc*>(m_pDocument); }
#endif

