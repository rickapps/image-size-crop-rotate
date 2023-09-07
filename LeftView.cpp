// LeftView.cpp : implementation of the CSelectionView class
//

#include "stdafx.h"
#include <direct.h>
#include "Cropper.h"
#include "Utils.h"
#include "CropperDoc.h"
#include "LeftView.h"
#include "Directory.h"
#include ".\leftview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSelectionView

IMPLEMENT_DYNCREATE(CSelectionView, CTreeView)

BEGIN_MESSAGE_MAP(CSelectionView, CTreeView)
//    ON_WM_CREATE()
ON_WM_CREATE()
ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnTvnItemexpanded)
ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
ON_COMMAND(ID_POPUP_OPEN, OnFileOpen)
ON_COMMAND(ID_POPUP_PROCESSED, OnFileProcessed)
ON_COMMAND(ID_POPUP_REFRESH, OnPopupRefresh)
ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnTvnSelchanging)
END_MESSAGE_MAP()


// CSelectionView construction/destruction

CSelectionView::CSelectionView()
: m_nProcessedCount(0)
, m_nOriginalCount(0)
{
}

CSelectionView::~CSelectionView()
{
}

BOOL CSelectionView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CTreeView::PreCreateWindow(cs))
		return false;
	cs.style |= TVS_SHOWSELALWAYS | TVS_NOHSCROLL | TVS_DISABLEDRAGDROP |
                TVS_HASBUTTONS;
	return true;
}

#ifdef _DEBUG
void CSelectionView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CSelectionView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CCropperDoc* CSelectionView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCropperDoc)));
	return (CCropperDoc*)m_pDocument;
}
#endif //_DEBUG


// CSelectionView message handlers
int CSelectionView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CTreeView::OnCreate(lpCreateStruct) == -1)
        return -1;

	// Get a pointer to our control
	m_pTreeCtrl = &(GetTreeCtrl()); 
	m_Images.Create(IDB_SELECTION, 16, 1, RGB(255, 0, 255));
	m_pTreeCtrl->SetImageList(&m_Images, TVSIL_NORMAL);

    return 0;
}

void CSelectionView::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
   	// Folder has expanded. If no children, it does not get this
	// message. We will change the image.
	int nImage;
	int nSelected;
	*pResult = 0;
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
    if (m_pTreeCtrl->GetItemData(hItem) > 0)
        return;  // Only change the image on the root level

	m_pTreeCtrl->GetItemImage(hItem, nImage, nSelected);
	if (pNMTreeView->action == TVE_EXPAND)
		m_pTreeCtrl->SetItemImage(hItem, nImage + 1, nImage + 1);   // Change icon to open folder
	else
		m_pTreeCtrl->SetItemImage(hItem, nImage - 1, nImage - 1);   // Change icon to closed folder

}

void CSelectionView::OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;

    if (hItem == NULL)
        return;
    int nView = (int)(m_pTreeCtrl->GetItemData(hItem));
    if (nView == nVIEWORIG)
    {
        if (pNMTreeView->action != TVC_UNKNOWN)
        {
            // Check if the folder exists
            if (_chdir(GetDocument()->m_sSourceDir) != 0)
                AfxMessageBox("The current Originals folder is not available.");
            else
                AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_VIEW_ORIGINALS);
        }
        *pResult = 1;
    }
    else if (nView == nVIEWPROC)
    {
         if (pNMTreeView->action != TVC_UNKNOWN)
         {
             // Check if processed folder exists
             if (_chdir(GetDocument()->m_sProcessDir) != 0)
                 AfxMessageBox("You must crop or resize a photo before you can view the Processed folder");
             else
                AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_VIEW_PROCESSED);
         }
         *pResult = 1;
    }
    else
        *pResult = 0;
}

void CSelectionView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    // A new image has been selected
    *pResult = 0;
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
    if (hItem == NULL)
        return;
    int nView = (int)(m_pTreeCtrl->GetItemData(hItem));
    CString sFileName = m_pTreeCtrl->GetItemText(hItem);
    GetDocument()->SetImage(nView, sFileName);
}

void CSelectionView::OnViewNext()
{
    // Select the next item in the list
    DWORD_PTR nView;
    HTREEITEM hCheck;
    HTREEITEM hItem = m_pTreeCtrl->GetSelectedItem();
    if (hItem == NULL)
        hItem = m_pTreeCtrl->GetRootItem();
    nView = m_pTreeCtrl->GetItemData(hItem);
    if ((nView != nORIGINAL) && (nView != nPROCESSED))
    {
        hItem = m_pTreeCtrl->GetRootItem();
        // Get the photos folder
        hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
        // Get the first photo
        hItem = m_pTreeCtrl->GetNextItem(hItem, TVGN_CHILD);
        
    }
    else
    {
        hCheck = m_pTreeCtrl->GetNextItem(hItem, TVGN_NEXT);
        if (hCheck == NULL) 
        {
            hCheck = m_pTreeCtrl->GetParentItem(hItem);
            hCheck = m_pTreeCtrl->GetNextItem(hCheck, TVGN_NEXT);
        }
        hItem = hCheck;
    }
    if (hItem != NULL)
    {
        nView = m_pTreeCtrl->GetItemData(hItem);
        if (nView == nORIGINAL || nView == nPROCESSED)
        {
            m_pTreeCtrl->SelectItem(hItem);
            m_pTreeCtrl->EnsureVisible(hItem);
        }
    }
}

void CSelectionView::OnViewPrevious()
{
   // TODO: Add error handling code. Perhaps display summary screen on folder node
    // Select the previous item in the list
    DWORD_PTR nView;
    HTREEITEM hCheck;
    HTREEITEM hItem = m_pTreeCtrl->GetSelectedItem();
    if (hItem == NULL) 
        hItem = m_pTreeCtrl->GetRootItem();
    // If they are not on a picture, put them on the first picture
    nView = m_pTreeCtrl->GetItemData(hItem);
    if ((nView != nORIGINAL) && (nView != nPROCESSED))
    {
        hItem = m_pTreeCtrl->GetRootItem();
        // Get the photos folder
        hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
        // Get the first photo
        hItem = m_pTreeCtrl->GetNextItem(hItem, TVGN_CHILD);
        
    }
    else
    {
        hCheck = m_pTreeCtrl->GetNextItem(hItem, TVGN_PREVIOUS);
        if (hCheck == NULL) 
            hCheck = m_pTreeCtrl->GetNextItem(hItem, TVGN_PARENT);
        hItem = hCheck;
    }
    if (hItem != NULL)
    {
        nView = m_pTreeCtrl->GetItemData(hItem);
        if (nView == nORIGINAL || nView == nPROCESSED)
        {
            m_pTreeCtrl->SelectItem(hItem);
            m_pTreeCtrl->EnsureVisible(hItem);
        }
    }
}

void CSelectionView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    switch (lHint) {
        case nORIGINAL: {   // New Original Item
            CString* sValue = (CString*) pHint;
            AddNewItem(*sValue, NULL);
            break;
                }
        case nPROCESSED: {   // New Processed Item
            CString* sValue = (CString*) pHint;
            AddNewItem(*sValue, GetParent());
            break;
                }
        case nDIRECTORY:   // Source or target dir change
            PopulateTree();
            SelectFirstPhoto();
            break;
        default:
            // Do nothing
            break;
    }
    return;
}

void CSelectionView::OnInitialUpdate()
{
    CTreeView::OnInitialUpdate();
    // Make sure the tree gets populated
    OnUpdate(this, 3, 0);
    SelectFirstPhoto();
}

bool CSelectionView::AddNewItem(LPCSTR sFile, HTREEITEM hParent)
{
    bool bInd = false;
    HTREEITEM hItem;
    CString sMatch;
    int nFolder = nORIGINAL;  // Assume it is an original item. 

    if (hParent == NULL)
    {
        hParent = m_pTreeCtrl->GetRootItem();  // The Originals folder
        hParent = m_pTreeCtrl->GetNextItem(hParent, TVGN_NEXT);
    }
    else
        nFolder = nPROCESSED;   // This is a processed item

    // Check for duplicates
    hItem = m_pTreeCtrl->GetNextItem(hParent, TVGN_CHILD); 
    while (hItem) {
        sMatch = m_pTreeCtrl->GetItemText(hItem);
        if (sMatch.CompareNoCase(FileName(sFile)) == 0)
            break;
        hItem = m_pTreeCtrl->GetNextItem(hItem, TVGN_NEXT);
    }

    // If there are no dups, add the image to the list
    if (hItem == NULL) {
        hItem = m_pTreeCtrl->InsertItem(
            FileName(sFile), 
            ID_NAV_NOSELECT, ID_NAV_SELECTED, hParent);
        // What images do we need?
        if (nFolder == nPROCESSED) {
	        m_pTreeCtrl->SetItemImage(hItem, ID_NAV_NONE, ID_NAV_NONESEL);
            m_pTreeCtrl->SetItemImage(hParent, ID_NAV_CHECKBLACK, ID_NAV_CHECKRED);
            m_nProcessedCount++;   // Increment our count of processed pictures
        }
        else
            m_nOriginalCount++;
        m_pTreeCtrl->SetItemData(hItem, nFolder);
    }

    return bInd;
}

void CSelectionView::PopulateTree(void)
{
	HTREEITEM hOriginals;
    CString sFileToProcess;

    // Clear the tree
	m_pTreeCtrl->DeleteAllItems();
    m_nProcessedCount = 0;
    m_nOriginalCount = 0;

    // At the root, tell people to drag an drop photos
    hOriginals = m_pTreeCtrl->InsertItem("Drag & Drop Photos Here", ID_NAV_DRAGDROP,
                                ID_NAV_DRAGDROP, TVI_ROOT);
    m_pTreeCtrl->SetItemImage(hOriginals, ID_NAV_DRAGDROP, ID_NAV_DRAGDROP);
    m_pTreeCtrl->SetItemData(hOriginals, nDRAGDROP);
    // Create a folder for our photos
    sFileToProcess.Format(IDS_TREE_PHOTOS, m_nOriginalCount);  // Still zero
 	hOriginals = m_pTreeCtrl->InsertItem(sFileToProcess, ID_NAV_FOLDERCLOSE, 
                                          ID_NAV_FOLDEROPEN, TVI_ROOT);
    m_pTreeCtrl->SetItemImage(hOriginals, 0, 0);
    m_pTreeCtrl->SetItemData(hOriginals, nLOGO);
    // Populate the folder
    CDirectory* pSrcDir = GetDocument()->m_pSourceFolder;
    pSrcDir->FirstFile("*.jpg;");
    while (pSrcDir->NextMatch(sFileToProcess)) {
        AddNewItem(sFileToProcess, NULL);
    }
    sFileToProcess.Format(IDS_TREE_ORIGS, m_nOriginalCount);
    hOriginals = m_pTreeCtrl->InsertItem(sFileToProcess, ID_NAV_PROC, ID_NAV_PROC, TVI_ROOT);
    m_pTreeCtrl->SetItemData(hOriginals, nVIEWORIG);
    sFileToProcess.Format(IDS_TREE_PROCS, m_nProcessedCount);
    hOriginals = m_pTreeCtrl->InsertItem(sFileToProcess, ID_NAV_ORIGS, ID_NAV_ORIGS, TVI_ROOT);
    m_pTreeCtrl->SetItemData(hOriginals, nVIEWPROC);
    OnViewNext();
}

void CSelectionView::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
    // If they are on a folder, allow them to edit
	UINT uFlags;
	HTREEITEM hItem;
	CPoint client;

	// Get the mouse position. Must be a better way to do this,
	// but don't know at this time.
	DWORD dwPos = ::GetMessagePos();
	CPoint point ((int) LOWORD(dwPos), (int) HIWORD(dwPos));
	client = point;
	m_pTreeCtrl->ScreenToClient(&client);
	hItem = m_pTreeCtrl->HitTest(client, &uFlags);
	// Display our context menu
    if ((hItem != NULL) && (uFlags & TVHT_ONITEM)) {

		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_SELTREE_MENU));
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN
						     | TPM_RIGHTBUTTON, 
						  point.x, point.y, this); 
        *pResult = 0;
    } else
        *pResult = 1;
}

void CSelectionView::OnFileOpen()
{
    // Make sure the app gets the message...
    AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_OPEN, 0);
}

void CSelectionView::OnFileProcessed()
{
    // Make sure the app gets the message...
    AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_OPENPROCESSED, 0);
}


void CSelectionView::OnPopupRefresh()
{
    // Refresh the treeview listing
    CWaitCursor oCursor;
    GetDocument()->SetSourceDir(GetDocument()->m_sSourceDir);
}

void CSelectionView::OnPositionTop()
{
    HTREEITEM hItem = m_pTreeCtrl->GetRootItem();
    if (hItem != NULL)
    {
        m_pTreeCtrl->SelectItem(hItem);
        OnViewNext();
    }
}

HTREEITEM CSelectionView::GetParent(void)
{
    // Return the handle of the current item. If the current item
    // is a processed item, return the handle of its parent.
    int nLevel;
    HTREEITEM hItem = m_pTreeCtrl->GetSelectedItem();

    if (hItem != NULL) {
        // What is the level of this item?
        nLevel = (int) (m_pTreeCtrl->GetItemData(hItem));
        if (nLevel == nPROCESSED)
            hItem = m_pTreeCtrl->GetParentItem(hItem);
    }
    return hItem;
}


// Set the file counts on our tree view listings
void CSelectionView::UpdateCounts(void)
{
    // Set the number of our Originals and our Processed
    DWORD_PTR nView;
    CString sCaption;
    HTREEITEM hItem = m_pTreeCtrl->GetRootItem();
    while (hItem != NULL)
    {
        nView = m_pTreeCtrl->GetItemData(hItem);
        switch (nView) {
            case nLOGO:
                sCaption.Format(IDS_TREE_PHOTOS, m_nOriginalCount);
                m_pTreeCtrl->SetItemText(hItem, sCaption);
                break;
            case nVIEWORIG:
                sCaption.Format(IDS_TREE_ORIGS, m_nOriginalCount);
                m_pTreeCtrl->SetItemText(hItem, sCaption);
               break;
            case nVIEWPROC:
                sCaption.Format(IDS_TREE_PROCS, m_nProcessedCount);
                m_pTreeCtrl->SetItemText(hItem, sCaption);
                break;
            default:
                break;
        }
        hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
    }
}

// Select the first photo in the list. If none, select the root item.
bool CSelectionView::SelectFirstPhoto(void)
{
    bool bInd;
    HTREEITEM hItem = m_pTreeCtrl->GetRootItem();
    // Do we have any photos?
    if (m_nOriginalCount > 0)
    {
        // Get the photos folder
        hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
        // Get the first photo
        hItem = m_pTreeCtrl->GetNextItem(hItem, TVGN_CHILD);
        bInd = true;
    }
    else
        bInd = false;

    // Select the item;
    if (hItem != NULL)
    {
       m_pTreeCtrl->EnsureVisible(hItem);
       m_pTreeCtrl->SelectItem(hItem);
    }

    return false;
}
