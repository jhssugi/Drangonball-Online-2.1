// TreePCPane.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool2.h"
#include "NtlMTXml.h"
#include "BBoxPane.h"
#include "PropertyListXML.h"

#include "TreePCPane.h"

CTreePCPane* CTreePCPane::m_pInstance = NULL;

// CTreePCPane

IMPLEMENT_DYNCREATE(CTreePCPane, CXTTreeView)

CTreePCPane::CTreePCPane()
{
    m_pInstance = this;

    m_pImageList = NULL;
    m_pCharacter = NULL;
    m_bDrag      = FALSE;

    m_strRootName = "PC";        
}

CTreePCPane::~CTreePCPane()
{


    if(m_pImageList)
    {
        delete m_pImageList;
        m_pImageList = NULL;
    }
}

BEGIN_MESSAGE_MAP(CTreePCPane, CXTTreeView)
    ON_NOTIFY_REFLECT(NM_RCLICK, &CTreePCPane::OnNMRclick)
    ON_COMMAND(ID_MENU_ADD_FOLDER, &CTreePCPane::OnMenuAddFolder)
    ON_COMMAND(ID_MENU_RENAME_FOLDER, &CTreePCPane::OnMenuRenameFolder)
    ON_COMMAND(ID_MENU_DELETE_FOLDER, &CTreePCPane::OnMenuDeleteFolder)
    ON_COMMAND(ID_MENU_NEW_SCRIPT, &CTreePCPane::OnMenuNewScript)
    ON_COMMAND(ID_MENU_LOAD_SCRIPT, &CTreePCPane::OnMenuLoadScript)
    ON_COMMAND(ID_SCRIPT_SETCLUMP, &CTreePCPane::OnScriptSetclump)
    ON_COMMAND(ID_MENU_SCRIPT_SAVE, &CTreePCPane::OnMenuScriptSave)
    ON_COMMAND(ID_MENU_SCRIPT_SAVEAS, &CTreePCPane::OnMenuScriptSaveas)
    ON_COMMAND(ID_MENU_SCRIPT_RENAME, &CTreePCPane::OnMenuScriptRename)
    ON_COMMAND(ID_MENU_DELETE_SCRIPT, &CTreePCPane::OnMenuDeleteScript)
    ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CTreePCPane::OnTvnEndlabeledit)
    ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CTreePCPane::OnTvnBeginlabeledit)
    ON_WM_DESTROY()
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CTreePCPane::OnTvnSelchanged)
    ON_COMMAND(ID_SCRIPT_SAVEALL, &CTreePCPane::OnScriptSaveall)
    ON_WM_SETFOCUS()
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CTreePCPane::OnTvnBegindrag)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CTreePCPane diagnostics

#ifdef _DEBUG
void CTreePCPane::AssertValid() const
{
	CXTTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CTreePCPane::Dump(CDumpContext& dc) const
{
	CXTTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTreePCPane message handlers

void CTreePCPane::OnInitialUpdate()
{
    CXTTreeView::OnInitialUpdate();

    ModifyStyle(0, TVS_SHOWSELALWAYS | TVS_EDITLABELS  | TVS_HASBUTTONS  | TVS_HASLINES);

    // ?????? ?????? ????
    m_pImageList = new CImageList();
    m_pImageList->Create(16, 16, ILC_COLOR32, 4, 4);
    m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_CLOSE));
    m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_OPEN));
    m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_SCRIPT));        
    GetTreeCtrl().SetImageList(m_pImageList, TVSIL_NORMAL);    

    // ???? ?????? ??????
    TV_INSERTSTRUCT tvStruct;
    tvStruct.hParent      = NULL;
    tvStruct.hInsertAfter = TVI_LAST;
    tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvStruct.item.pszText = (LPSTR)(LPCSTR)m_strRootName;
    tvStruct.item.iImage  = 0;
    tvStruct.item.iSelectedImage = 1;
    GetTreeCtrl().InsertItem(&tvStruct);
    
    InitClass();
    LoadTreeXML(m_strTreeFileName);
}

void CTreePCPane::InitClass()
{
    USES_CONVERSION;

    CString sWorkFolder = (CModelToolApplication::GetInstance()->GetWorkDir());
    m_sSaveFolderName = sWorkFolder;
    m_sSaveFolderName += MT_PATH_CHARACTER;
    
    m_strTreeFileName = sWorkFolder;
    m_strTreeFileName += "\\Tool\\ModelTool\\TreePC.xml";

    m_strPropertyListFileName = m_sSaveFolderName;
    m_strPropertyListFileName += "CharacterPropertyList.xml";
}

void CTreePCPane::SetModel(CMTCharacter* pCharacter)
{
    if(m_pCharacter == pCharacter)
        return;

    if(m_pCharacter)
    {
        m_pCharacter->SetVisible(FALSE);
    }

    m_pCharacter = pCharacter;
}

RwBool CTreePCPane::LoadTreeXML(const CHAR* szXmlFileName)
{
    if(!szXmlFileName)
        return FALSE;

    CNtlMTXml xmlLoad;
    xmlLoad.LoadTreeXML((CHAR*)szXmlFileName, &m_itemNode);    

    GetTreeCtrl().DeleteAllItems();
	InsertItemRecursive(&m_itemNode, NULL);

    GetTreeCtrl().Expand(GetTreeCtrl().GetRootItem(), TVE_EXPAND);

    return TRUE;
}

void CTreePCPane::GetAllChildNode(HTREEITEM hItem, TREEITEM_VECTOR& vTreeItem)
{
    while (hItem)
    {
        vTreeItem.push_back(hItem);

        if (GetTreeCtrl().ItemHasChildren(hItem))
        {
            GetAllChildNode(GetTreeCtrl().GetChildItem(hItem), vTreeItem);  //????????
        } 

        hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
    }   
}

RwBool CTreePCPane::SaveTreeXML(const CHAR* szXmlFileName)
{
    if(!szXmlFileName)
        return FALSE;

    CString strRootName = GetTreeCtrl().GetItemText(GetTreeCtrl().GetRootItem());

    CNtlMTXml xmlSave;
    xmlSave.CreateXML((LPSTR)(LPCSTR)strRootName);

    TREEITEM_VECTOR vTreeItem;
    GetAllChildNode(GetTreeCtrl().GetRootItem(), vTreeItem);
    for(size_t i = 0; i < vTreeItem.size(); ++i)
    {
        HTREEITEM pItem = GetTreeCtrl().GetParentItem(vTreeItem[i]);
        if(!pItem)
            continue;
        int nImage = 0, nSelectedImage = 0;
        GetTreeCtrl().GetItemImage(vTreeItem[i], nImage, nSelectedImage);
        CString sParentName = GetTreeCtrl().GetItemText(pItem);
        CString sItemName = GetTreeCtrl().GetItemText(vTreeItem[i]);
        
        sItemName.Replace("*", "");

        if(sItemName.Find("*") == -1)   // '*' ?????? ???? ?????? ????????.
        {
            // XML?? ????????.
            if(nImage == 0)    // ????
            {
                xmlSave.AddFolderNode((LPSTR)(LPCSTR)sParentName, (LPSTR)(LPCSTR)sItemName);
            }
            else if(nImage == 2)  // ????????
            {
                xmlSave.AddScriptNode((LPSTR)(LPCSTR)sParentName, (LPSTR)(LPCSTR)sItemName);
            }        
        }
    }
    xmlSave.Save((CHAR*)szXmlFileName);  

    // ???????? ?????? ????
    CPropertyListXML xmlPropertyList;
    xmlPropertyList.Save(m_strPropertyListFileName, CPropertyListXML::PROPERTY_CHARACTER, &GetTreeCtrl(), vTreeItem);
    
    return TRUE;
}


void CTreePCPane::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
    CPoint point;
    ::GetCursorPos(&point);

    CPoint tempPoint = point;
    ScreenToClient(&tempPoint);
    HTREEITEM hItem     = GetTreeCtrl().HitTest(tempPoint);
    if(hItem)
    {
        GetTreeCtrl().SelectItem(hItem);

        int nImage = 0;
        int nSelectedImage = 0;
        GetTreeCtrl().GetItemImage(hItem, nImage, nSelectedImage);

        CMenu  menuPopup;         
        CMenu* subMenu = NULL;
        menuPopup.LoadMenu(IDR_MENU_POPUP);

        if(nImage == 0)         // ????
        {
            // ?????? ????.
            subMenu = menuPopup.GetSubMenu(0);
            subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);        
        }
        else if(nImage == 2)    // ???????? 
        {
            subMenu = menuPopup.GetSubMenu(1);            
            subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);        
        }               
    }    

    *pResult = 0;
}

void CTreePCPane::OnMenuAddFolder()
{
    // ?? ?????? ????????.
    HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();;

    if(!hSelectedItem)
        return;

    TV_INSERTSTRUCT tvStruct;
    tvStruct.hParent      = hSelectedItem;    
    tvStruct.hInsertAfter = TVI_LAST;
    tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvStruct.item.pszText = "New Folder";
    tvStruct.item.iImage  = 0;
    tvStruct.item.iSelectedImage = 1;
    HTREEITEM hAddItem = GetTreeCtrl().InsertItem(&tvStruct);

    GetTreeCtrl().SelectItem(hAddItem);
    GetTreeCtrl().EditLabel(hAddItem);
}

void CTreePCPane::OnMenuRenameFolder()
{
    HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
    GetTreeCtrl().EditLabel(hSelectedItem);
}

void CTreePCPane::OnMenuDeleteFolder()
{
    if(MessageBox("Sure Delete Folder?", NULL, MB_OKCANCEL) == IDOK)
    {
        HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();;
        GetTreeCtrl().DeleteItem(hSelectedItem);
    }   
}

void CTreePCPane::OnMenuNewScript()
{
    USES_CONVERSION;

    // ?????? ?????????? ?????? ????????.   

    // ???????? ???? ????
    TV_INSERTSTRUCT tvStruct;
    tvStruct.hParent      = GetTreeCtrl().GetSelectedItem();;    
    tvStruct.hInsertAfter = TVI_LAST;
    tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvStruct.item.pszText = "New Script";
    tvStruct.item.iImage  = 2;    
    tvStruct.item.iSelectedImage = 2;
    HTREEITEM hAddItem = GetTreeCtrl().InsertItem(&tvStruct);

    GetTreeCtrl().SelectItem(hAddItem);

    if(!OnSetClump())
    {
        MessageBox("Clump Load Fail!");
    }
    else
    {
        // ???????? ???? DFF ???????? ??????.
        // ???????? ???? ?????????? ???? ???? '*'?? ??????.        
        CMTCharacter* pCharacter = (CMTCharacter*)GetTreeCtrl().GetItemData(hAddItem);
        if(pCharacter)
        {
            CString strScriptName = "*";
            strScriptName  += (pCharacter->GetProperty()->GetBaseMeshFileName().c_str());
            strScriptName = strScriptName.MakeUpper();
            strScriptName .Replace(".DFF", ".XML");
                        
            GetTreeCtrl().SetItemText(hAddItem, strScriptName);        
        }        
    }
}

void CTreePCPane::OnMenuLoadScript()
{
    USES_CONVERSION;
    // Load the script file. (The script file must be in the. \ Character \ folder.)
    CHAR sLoadFilter[] = "Character Script File (*.XML)|*.XML||";
    CString workDir = (CModelToolApplication::GetInstance()->GetWorkDir());
    workDir += "\\Character\\*.XML";    
    CFileDialog fileDlg(TRUE, NULL, workDir, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sLoadFilter);
    if(fileDlg.DoModal() == IDOK)
    {
        CString sFilePath = fileDlg.GetPathName();
        CString sLoadFileName = fileDlg.GetFileName();        

        // ?????? ?????? ????????.
        // ???? ????
        TV_INSERTSTRUCT sTvStruct;
        sTvStruct.hParent = GetTreeCtrl().GetSelectedItem();
        sTvStruct.hInsertAfter = TVI_SORT;
        sTvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        sTvStruct.item.iImage = 2;
        sTvStruct.item.iSelectedImage = 2;
        sTvStruct.item.pszText = (LPSTR)(LPCSTR)sLoadFileName;
        HTREEITEM hItem = GetTreeCtrl().InsertItem(&sTvStruct);
        GetTreeCtrl().SelectItem(hItem);
    }       
}


void CTreePCPane::OnScriptSetclump()
{
    if(!OnSetClump())
    {
        MessageBox("Clump Load Fail!");
    }
}

void CTreePCPane::OnMenuScriptSave()
{
    // ???? ?????? ?????????? ????????.
    HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
    if(!hSelectedItem)
        return;

    CString sScriptName = GetTreeCtrl().GetItemText(hSelectedItem);
    CString strMsg;
    
    // ???? '*'???? ?????? ???????????? '*'?? ???? ????????.
    int nIndex = sScriptName.Replace("*", "");

    if(OnSaveScript(hSelectedItem, sScriptName, FALSE))
    {
        if(nIndex)
        {
            GetTreeCtrl().SetItemText(hSelectedItem, sScriptName);
        }
    }    
}

void CTreePCPane::OnMenuScriptSaveas()
{
    // ?????? ???????? ?????????? ????????.
    HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    if(hItem)
    {
        OnSaveScript(hItem, "", TRUE);
    }    
}

void CTreePCPane::OnMenuScriptRename()
{
    HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
    if(!hSelectedItem)
        return;

    GetTreeCtrl().EditLabel(hSelectedItem);
}

void CTreePCPane::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    // ???????? ?????? ???????????? ??????????.
    if(pTVDispInfo->item.iImage == 2)
    {
        m_strPrevScriptName = pTVDispInfo->item.pszText;
    }
    
    *pResult = 0;
}

void CTreePCPane::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    USES_CONVERSION;

    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    
    // ?????? ????????????, ???????? ?????? ???????????? ???????? ??????????.
    CString strChangedName = pTVDispInfo->item.pszText;
    if(strChangedName != "")
    {
        HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();

        GetTreeCtrl().SetItemText(hSelectedItem, strChangedName);

        if(pTVDispInfo->item.iImage == 2)
        {
            CMTCharacter* pCharacter = (CMTCharacter*)GetTreeCtrl().GetItemData(hSelectedItem);
            if(pCharacter && pCharacter->GetProperty())
            {
                CString strPropName = strChangedName;
                strPropName.Replace(".XML", "");
                pCharacter->GetProperty()->SetName((strChangedName));

                CString sOrgScriptName = m_sSaveFolderName + m_strPrevScriptName;
                CString sNewScriptName = m_sSaveFolderName + strChangedName;
                CFile::Rename(sOrgScriptName, sNewScriptName);
            }
        }
    }
    
    *pResult = 0;
}

void CTreePCPane::OnMenuDeleteScript()
{
    HTREEITEM hSeledtedItem = GetTreeCtrl().GetSelectedItem();
    if(!hSeledtedItem)
        return;

    CMTCharacter* pCharacter = (CMTCharacter*)GetTreeCtrl().GetItemData(hSeledtedItem);
    if(pCharacter)
    {
        // ?????????? ?????? ?????? ????.
        pCharacter->SetVisible(FALSE);        
    }

    GetTreeCtrl().DeleteItem(hSeledtedItem);
}


RwBool CTreePCPane::OnSetClump()
{
    // Clump ?????? ????????.
    HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();;
    if(!hSelectedItem)
        return FALSE;

    USES_CONVERSION;

    CHAR szOpenFilter[] = "Clump File (*.dff)|*.dff||";
    CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szOpenFilter);    
    if(fileDlg.DoModal() == IDOK)
    {
        CString sLoadFilePath = ".";
        sLoadFilePath += fileDlg.GetPathName();	
        CString sLoadFileName = fileDlg.GetFileName();

        CMTCharacter* pCharacter = NULL;
        pCharacter = (CMTCharacter*)GetTreeCtrl().GetItemData(hSelectedItem);
        if(!pCharacter)
        {
            // ?????? ??????????.
            pCharacter = new CMTCharacter;
            pCharacter->Create();
            GetTreeCtrl().SetItemData(hSelectedItem, (DWORD_PTR)pCharacter);
            m_vCharacter.push_back(pCharacter);
        }

        RwBool retBool = pCharacter->LoadClump((LPSTR)(LPCSTR)(sLoadFilePath));
        if(retBool == (RwBool)TRUE)
        {
            ////  ?????? ???????? ????????.                        
            //CString sWorkPath = (CModelToolApplication::GetInstance()->GetWorkDir());

            //sWorkPath = sWorkPath.MakeUpper();                
            //sLoadFilePath = sLoadFilePath.MakeUpper();
            //sLoadFileName = sLoadFileName.MakeUpper();

            //sLoadFilePath.Replace(sWorkPath, "");                
            //pCharacter->GetProperty()->SetBaseMeshFileName(W2A(sLoadFilePath));

            m_pCharacter = pCharacter;
            
            CModelToolApplication::GetInstance()->SetEditChar(pCharacter);            

            return TRUE;
        }        
    }	

    return FALSE;
}

RwBool CTreePCPane::OnSaveScript(HTREEITEM hItem, const CHAR* szScriptName, BOOL bVisible)
{
    if(!szScriptName || !hItem)
        return FALSE;

    USES_CONVERSION;

    CString strMsg;
    strMsg.Format("[%s] ", szScriptName);
    RwBool bReturn = FALSE;

    CMTCharacter* pCharacter = (CMTCharacter*)GetTreeCtrl().GetItemData(hItem);
    if(!pCharacter)
    {
        strMsg += "Not Exist Character Data";
        MessageBox(strMsg, NULL, MB_ICONERROR);
        return FALSE;
    }

    CString sCurrentScriptFileName = szScriptName;
    if(bVisible == FALSE)
    {
        // ???? ???????? ???????? ???? ????????.
        sCurrentScriptFileName.Replace(".XML", "");
        pCharacter->GetProperty()->SetName((sCurrentScriptFileName));        

        CString sSaveFileName = m_sSaveFolderName;        
        sSaveFileName += szScriptName;
        
        bReturn = pCharacter->GetProperty()->SaveScript((LPSTR)(LPCSTR)(sSaveFileName));
    }
    else
    {
        // ???????????? ?????? ???? ????????.
        CString sSavePath = m_sSaveFolderName;
        sSavePath += GetTreeCtrl().GetItemText(hItem);
        sSavePath.Replace("*", "");

        CHAR szSaveFilter[] = "Character Script File (*.XML)|*.XML||";
        CFileDialog fileDlg(FALSE, "XML", sSavePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szSaveFilter);
        if(fileDlg.DoModal() == IDOK)
        {
            CString sSaveFilePath = fileDlg.GetPathName();
            CString sSaveFileName = fileDlg.GetFileName();

            sSaveFileName.Replace(".XML", ""); // ?????????? ?????????? ???????? ????????.
            pCharacter->GetProperty()->SetName((sSaveFileName));                
            bReturn = pCharacter->GetProperty()->SaveScript((LPSTR)(LPCSTR)(sSaveFilePath));
            if(bReturn)
            {
                sSaveFileName += ".XML";   // ???? ???????? ????????.                
                GetTreeCtrl().SetItemText(hItem, sSaveFileName);    
            }            
        }
    }  

    if(!bReturn)
    {
        strMsg += "Script File Save Fail!";
        MessageBox(strMsg, NULL, MB_ICONERROR);
    }    

    return bReturn;
}


void CTreePCPane::OnDestroy()
{   
    // ???? ?????? ???? XML?? ?????? ????????.
    SaveTreeXML(m_strTreeFileName);

    for(size_t i = 0; i < m_vCharacter.size(); ++i)
    {
        CMTCharacter* pCharacter = m_vCharacter[i];
        if(pCharacter)
        {
            pCharacter->Destroy();
            delete pCharacter;
            pCharacter = NULL;
        }
    }
    m_vCharacter.clear();


    CXTTreeView::OnDestroy();
    
}

void CTreePCPane::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    USES_CONVERSION;

    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    HTREEITEM hItem =  pNMTreeView->itemOld.hItem;   
    HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
    int nImage = 0, nSelectedImage = 0;
    GetTreeCtrl().GetItemImage(hSelectedItem, nImage, nSelectedImage);

    if(hItem)
    {
        // ?????? ???????? ?????? ???????? ????????.
        CMTCharacter* pCharacter = (CMTCharacter*)GetTreeCtrl().GetItemData(hItem);
        if(pCharacter)
        {
            pCharacter->SetVisible(FALSE);            
        }

        GetTreeCtrl().SetItemState(hItem, NULL, TVIS_BOLD);

    }

    if(nImage == 0)    // ?????? ??????????.
    {
        CModelToolApplication::GetInstance()->SetEditChar(NULL);
    }
    else if(nImage == 2)   // ?????????? ??????????.
    {
        GetTreeCtrl().SetItemState(hSelectedItem, TVIS_BOLD, TVIS_BOLD);

        // ???????? ?????? ???????????? ?????????? ????????.
        CString sScriptFileName = GetTreeCtrl().GetItemText(hSelectedItem);
        if(sScriptFileName != "" && sScriptFileName != "New Script")
        {

            CMTCharacter* pCharacter = (CMTCharacter*)GetTreeCtrl().GetItemData(hSelectedItem);
            if(pCharacter)
            {
                pCharacter->SetVisible(TRUE);                
            }
            else
            {
                // ???????? ???? ???????? ?????????? ???????? ????????.
                pCharacter = new CMTCharacter();
                pCharacter->Create();
                
                std::string strXmlFileName = (m_sSaveFolderName);
                strXmlFileName += (sScriptFileName);
                    
                if(pCharacter->GetProperty()->LoadScript(strXmlFileName))
                {
                    std::string strClumpFileName = CModelToolApplication::GetInstance()->GetWorkDir();
                    strClumpFileName += pCharacter->GetProperty()->GetBaseMeshFilePath().c_str();
                    strClumpFileName += pCharacter->GetProperty()->GetBaseMeshFileName().c_str();

                    if(pCharacter->LoadClump((RwChar*)strClumpFileName.c_str()))
                    {
                        GetTreeCtrl().SetItemData(hSelectedItem, (DWORD_PTR)pCharacter);
                        m_vCharacter.push_back(pCharacter);
                    }
                    else
                    {
                        CString msg;
                        msg.Format("[%S] Clump File Load Fail!", pCharacter->GetProperty()->GetBaseMeshFileName().c_str());
                        MessageBox(msg, NULL, MB_ICONERROR);

                        pCharacter->Destroy();
                        delete pCharacter;
                        pCharacter = NULL;
                    }
                }
                else
                {
                    CString msg;
                    msg.Format("[%s] Script File Load Fail!", sScriptFileName);
                    MessageBox(msg, NULL, MB_ICONERROR);

                    pCharacter->Destroy();
                    delete pCharacter;
                    pCharacter = NULL;
                }
            }

            m_pCharacter = pCharacter;
            CModelToolApplication::GetInstance()->SetEditChar(pCharacter);
        }
    }
    *pResult = 0;
}

void CTreePCPane::OnDataChanged()
{
    // ???????? ???????? ?????????????? '*'?? ???????? ????????.
    HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
    if(hSelectedItem)
    {
        CString strItemName = GetTreeCtrl().GetItemText(hSelectedItem);
        if(strItemName.Find("*") < 0)
        {
            strItemName.Insert(0, "*");
            GetTreeCtrl().SetItemText(hSelectedItem, strItemName);
        }        
    }
}
void CTreePCPane::OnScriptSaveall()
{
    // ???? ?????????? ????????.
    TREEITEM_VECTOR vTreeItem;
    GetAllChildNode(GetTreeCtrl().GetRootItem(), vTreeItem);
    for(UINT i = 0; i < vTreeItem.size(); ++i)
    {
        HTREEITEM hItem = vTreeItem[i];
        if(hItem && GetTreeCtrl().GetItemData(hItem))
        {
            CString strScriptName = GetTreeCtrl().GetItemText(hItem);
            int nIndex = strScriptName.Replace("*", "");
            if(OnSaveScript(hItem, strScriptName, FALSE))
            {
                if(nIndex)
                {
                    GetTreeCtrl().SetItemText(hItem, strScriptName);
                }
            }
        }
    }
}

void CTreePCPane::OnShowSaveForChanges()
{
    TREEITEM_VECTOR vTreeItem;
    TREEITEM_VECTOR vChangeItem;
    GetAllChildNode(GetTreeCtrl().GetRootItem(), vTreeItem);

    // *?????? ???? ?????????? ???????? ??????.
    for(UINT i = 0; i < vTreeItem.size(); ++i)
    {
        HTREEITEM hItem = vTreeItem[i];
        if(GetTreeCtrl().GetItemData(hItem))
        {
            CString strScriptName = GetTreeCtrl().GetItemText(hItem);
            if(strScriptName.Find("*") >= 0)
            {
                strScriptName.Replace("*", "");
                GetTreeCtrl().SetItemText(hItem, strScriptName);
                vChangeItem.push_back(hItem);
            }
        }
    }


    if(vChangeItem.size())
    {
        CString strTitle, strScriptList, strMsg;
        strTitle.Format("< %s > Save Changes to following Items?\n\n", m_strRootName);
        for(UINT i = 0; i < vChangeItem.size(); ++i)
        {
            HTREEITEM hItem = vChangeItem[i];
            strScriptList += GetTreeCtrl().GetItemText(hItem);
            strScriptList += "\n";
        }

        strScriptList += "\n";
        strMsg = strTitle + strScriptList;
        if(MessageBox(strMsg, m_strRootName, MB_YESNO) == IDYES)
        {
            for(UINT i = 0; i < vChangeItem.size(); ++i)
            {
                CString strScriptName = GetTreeCtrl().GetItemText(vChangeItem[i]);
                OnSaveScript(vChangeItem[i], strScriptName, FALSE);
            }
        }
    }    
}


void CTreePCPane::OnSetFocus(CWnd* pOldWnd)
{
    CXTTreeView::OnSetFocus(pOldWnd);

    CModelToolApplication::GetInstance()->SetAppMode(MT_MODE_PC);

    //HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    //if(hItem)
    //{
    //    GetTreeCtrl().SelectItem(GetTreeCtrl().GetRootItem());
    //    GetTreeCtrl().SelectItem(hItem);
    //}    
}

void CTreePCPane::InsertItemRecursive( SItemNode* pItemNode, HTREEITEM hParentItem ) 
{
	// ?????? ????????.
	TV_INSERTSTRUCT sTvStruct;
	sTvStruct.hParent = hParentItem;
	sTvStruct.hInsertAfter = TVI_SORT;
	sTvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	sTvStruct.item.pszText = (LPSTR)(LPCSTR)pItemNode->strNodeName;

	if(pItemNode->eNodeType == NODE_SCIRPT)
	{
		// ???????????? ?????? ???????? ??????.
		sTvStruct.item.iImage = 2;
		sTvStruct.item.iSelectedImage = 2;
		GetTreeCtrl().InsertItem(&sTvStruct);
	}
	else	// ???????? ???????? ?????? ????.
	{
		sTvStruct.item.iImage = 0;
		sTvStruct.item.iSelectedImage = 1;
		HTREEITEM hItem = GetTreeCtrl().InsertItem(&sTvStruct);

		for each(SItemNode* pChildNode in pItemNode->vecChildList)
		{
			if(pChildNode)
			{
				InsertItemRecursive(pChildNode, hItem);
			}
		}
	}
}
void CTreePCPane::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    if(pNMTreeView->itemNew.hItem)
    {
        SetClassLong(this->m_hWnd, GCL_HCURSOR, (long)AfxGetApp()->LoadCursor(IDC_CURSOR1));
        m_bDrag = TRUE;
        SetCapture();
        m_hDragItem = pNMTreeView->itemNew.hItem;
    }
    
    *pResult = 0;
}

void CTreePCPane::OnMouseMove(UINT nFlags, CPoint point)
{
    if(m_bDrag)
    {
        if(point.y <= 0 && m_hCurItem)
        {
            //m_hCurItem = GetTreeCtrl().GetPrevItem(m_hCurItem);            
            
        }        
        else
        {
            m_hCurItem = GetTreeCtrl().HitTest(point);            
        }   

        if(m_hCurItem)
        {
            GetTreeCtrl().SelectDropTarget(m_hCurItem);  
            GetTreeCtrl().EnsureVisible(m_hCurItem);
        }
    }

    CXTTreeView::OnMouseMove(nFlags, point);
}

void CTreePCPane::OnLButtonUp(UINT nFlags, CPoint point)
{
    if(m_bDrag && m_hCurItem)
    {
        ReleaseCapture();

        //?????? ???????? ????
        SetClassLong(this->m_hWnd, GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
        m_bDrag = FALSE;

        int nImageIndex = 0, nSelectImageIndex = 0;
        GetTreeCtrl().GetItemImage(m_hCurItem, nImageIndex, nSelectImageIndex);
        if(nImageIndex == 0) // ???? ?????? ?????? ????????
        {
            OnMoveItem(m_hDragItem, m_hCurItem);                
        }

        GetTreeCtrl().SelectDropTarget(NULL);
    }

    CXTTreeView::OnLButtonUp(nFlags, point);
}

void CTreePCPane::OnMoveItem( HTREEITEM hItem, HTREEITEM hParentItem, BOOL bRecrusive /*= FALSE*/ ) 
{
    while(hItem)
    {
        if(hItem == hParentItem)
        {
            //hItem = GetTreeCtrl().GetNextSelectedItem(hItem);
            continue;
        }

        int nImageIndex = 0, nSelectedImage = 0;
        GetTreeCtrl().GetItemImage(hItem, nImageIndex, nSelectedImage);
        CString strItemName = GetTreeCtrl().GetItemText(hItem);

        TV_INSERTSTRUCT sTvStruct;
        sTvStruct.hParent = hParentItem;
        sTvStruct.hInsertAfter = TVI_SORT;
        sTvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        sTvStruct.item.iImage = nImageIndex;
        sTvStruct.item.iSelectedImage = nSelectedImage;
        sTvStruct.item.pszText = (LPSTR)(LPCSTR)strItemName;    
        HTREEITEM hNewItem = GetTreeCtrl().InsertItem(&sTvStruct);        

        HTREEITEM hDeleteItem = hItem;

        if(GetTreeCtrl().ItemHasChildren(hItem))
        {
            OnMoveItem(GetTreeCtrl().GetChildItem(hItem), hNewItem, TRUE);
        }

        if(bRecrusive)
        {
            hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
        }
        else
        {
            //hItem = GetTreeCtrl().GetNextSelectedItem(hItem);
            hItem = NULL;
        }

        GetTreeCtrl().SetItemState(hDeleteItem, 0, TVIS_FOCUSED);
        GetTreeCtrl().DeleteItem(hDeleteItem);
        GetTreeCtrl().SelectItem(hNewItem);
    }

}