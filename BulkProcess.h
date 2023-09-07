#pragma once

#include "CropperDoc.h"

// CBulkProcess dialog

class CBulkProcess : public CDialog
{
	DECLARE_DYNAMIC(CBulkProcess)

public:
	CBulkProcess(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBulkProcess();

// Dialog Data
	enum { IDD = IDD_BULKSAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnBnClickedBrowse();

	DECLARE_MESSAGE_MAP()
    virtual BOOL OnInitDialog();

public:
    BOOL m_bCheckOverwrite;
    int m_nSourceCnt;
    CCropperDoc* m_pDoc;

protected:
    virtual void OnOK();
    int PopulateFolderList(int nDefault);

public:
    afx_msg void OnBnClickedSetsize();
    void SetLabel(void);
    CString m_sProcessFolder;
    CString m_sSourceFolder;
    int m_nFolderID;
};
