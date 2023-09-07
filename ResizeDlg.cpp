// ResizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "ResizeDlg.h"
#include ".\resizedlg.h"


// CResizeDlg dialog

IMPLEMENT_DYNAMIC(CResizeDlg, CDialog)
CResizeDlg::CResizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResizeDlg::IDD, pParent)
{
}

CResizeDlg::~CResizeDlg()
{
}

void CResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResizeDlg, CDialog)
END_MESSAGE_MAP()


// CResizeDlg message handlers

void CResizeDlg::OnOK()
{
    // Get the size of the dialog
    this->GetWindowRect(&m_Resize);
    m_Resize.MoveToXY(0, 0);

    CDialog::OnOK();
}
