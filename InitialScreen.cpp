// ReminderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "InitialScreen.h"
#include ".\initialscreen.h"


// CReminderDlg dialog

IMPLEMENT_DYNCREATE(CReminderDlg, CDHtmlDialog)

CReminderDlg::CReminderDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CReminderDlg::IDD, CReminderDlg::IDH, pParent)
{
}
CReminderDlg::CReminderDlg(UINT nTemplate, CWnd* pParent)
    : CDHtmlDialog(CReminderDlg::IDD, nTemplate, pParent)
{
}

CReminderDlg::~CReminderDlg()
{
}

void CReminderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CReminderDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CReminderDlg, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CReminderDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CReminderDlg message handlers

HRESULT CReminderDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT CReminderDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

