#pragma once
#include "atltypes.h"


// CRotateDlg dialog

class CRotateDlg : public CDialog
{
	DECLARE_DYNAMIC(CRotateDlg)

public:
	CRotateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRotateDlg();

// Dialog Data
	enum { IDD = IDD_ROTATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    int m_nAngle;
    afx_msg void OnPaint();
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
private:
    CRect m_rectBoundary;
    int m_nSliderPos;
protected:
    virtual void OnOK();
};
