// LeftView.h : interface of the CSelectionView class
//


#pragma once

class CCropperDoc;

class CSelectionView : public CTreeView
{
protected: // create from serialization only
	CSelectionView();
	DECLARE_DYNCREATE(CSelectionView)
	CImageList m_Images;
	CTreeCtrl* m_pTreeCtrl;

// Attributes
public:
	CCropperDoc* GetDocument();

// Operations
public:
    void OnViewNext();
    void OnViewPrevious();

// Overrides
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:

// Implementation
public:
	virtual ~CSelectionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
protected:
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
public:
    virtual void OnInitialUpdate();
    // Add a new entry to the process folder
    bool AddNewItem(LPCSTR sFile, HTREEITEM hParent);
private:
    void PopulateTree(void);
public:
    afx_msg void OnFileOpen();
    afx_msg void OnFileProcessed();
    afx_msg void OnPopupRefresh();
    afx_msg void OnPositionTop();
    HTREEITEM GetParent(void);
    int m_nProcessedCount;
    int m_nOriginalCount;
    afx_msg void OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
    // Set the file counts on our tree view listings
    void UpdateCounts(void);
    // Select the first photo in the list. If none, select the root item.
    bool SelectFirstPhoto(void);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CCropperDoc* CSelectionView::GetDocument()
   { return reinterpret_cast<CCropperDoc*>(m_pDocument); }
#endif

