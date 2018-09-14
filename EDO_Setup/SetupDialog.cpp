//////////////////////////////////////////////////////////////////////////
//
// EDO = Edo's Driver for Oracle
// ORACLE ODBC DRIVER for ODBC 3.51
//
// Copyright (C) 2008 ir. Wicher Edo Huisman
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
//
////////////////////////////////////////////////////////////////////////
//
// Contents:  The setup dialog for the ODBC manager
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "EDO_Setup.h"
#include "SetupDialog.h"
#include "Errors.h"

// SetupDialog dialog

IMPLEMENT_DYNAMIC(SetupDialog, CDialog)

SetupDialog::SetupDialog(CWnd*   p_parent
                        ,CString p_mode
                        ,CString p_dataSource
                        ,Attributes* p_attrib)
            :CDialog(SetupDialog::IDD, p_parent)
            ,m_dataSource(p_dataSource)
            ,m_attrib(p_attrib)
            ,m_mode(p_mode)
{
  m_tab1Dlg = new SetupPage1(this,p_attrib);
  m_tab2Dlg = new SetupPage2(this,p_attrib);
  m_tab3Dlg = new SetupPage3(this,p_attrib);
  m_tab4Dlg = new SetupPage4(this,p_attrib);
}

SetupDialog::~SetupDialog()
{
  delete m_tab1Dlg;
  delete m_tab2Dlg;
  delete m_tab3Dlg;
  delete m_tab4Dlg;
}

void 
SetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  DDX_Text(pDX,IDC_MODE,          m_mode);
  DDX_Text(pDX,IDC_DATASOURCE,    m_dataSource);
  DDX_Text(pDX,IDC_DESCRIPTION,   m_description);
  DDX_Text(pDX,IDC_TNS,           m_tnsService);
  DDX_Control(pDX,IDC_TAB,        m_tab);
}

BEGIN_MESSAGE_MAP(SetupDialog, CDialog)
  ON_EN_KILLFOCUS(IDC_DATASOURCE,   &SetupDialog::OnEnChangeDatasource)
  ON_EN_KILLFOCUS(IDC_DESCRIPTION,  &SetupDialog::OnEnChangeDescription)
  ON_EN_KILLFOCUS(IDC_TNS,          &SetupDialog::OnEnChangeTns)
  ON_BN_CLICKED(IDOK,               &SetupDialog::OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL,           &SetupDialog::OnBnClickedCancel)
  ON_BN_CLICKED(IDHELP,             &SetupDialog::OnBnClickedHelp)
  ON_BN_CLICKED(IDC_TEST,           &SetupDialog::OnBnClickedTest)
  ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &SetupDialog::OnTcnSelchangeTab)
END_MESSAGE_MAP()

BOOL
SetupDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_tab1Dlg->Create(IDD_TABODBC,   this);
  m_tab2Dlg->Create(IDD_TABOORACLE,this);
  m_tab3Dlg->Create(IDD_TABTRACE,  this);
  m_tab4Dlg->Create(IDD_TABNLS,    this);
  CString s;
  m_tab1Dlg->GetWindowText(s);
  m_tab.InsertItem(0,s);
  m_tab2Dlg->GetWindowText(s);
  m_tab.InsertItem(1,s);
  m_tab3Dlg->GetWindowText(s);
  m_tab.InsertItem(2,s);
  m_tab4Dlg->GetWindowText(s);
  m_tab.InsertItem(3,s);

  CRect rect;
  m_tab.GetWindowRect(&rect);
  ScreenToClient(&rect);
  rect.top    += 24;   // 22
  rect.bottom -= 2;
  rect.right  -= 2;
  rect.left   += 2;    // 6

  m_tab1Dlg->MoveWindow(rect,true);
  m_tab2Dlg->MoveWindow(rect,false);
  m_tab3Dlg->MoveWindow(rect,false);
  m_tab4Dlg->MoveWindow(rect,false);

  m_tab1Dlg->ShowWindow(SW_SHOW);
  m_tab2Dlg->ShowWindow(SW_HIDE);
  m_tab3Dlg->ShowWindow(SW_HIDE);
  m_tab4Dlg->ShowWindow(SW_HIDE);
  TabCtrl_SetCurSel(m_tab.GetSafeHwnd(),0);

  m_description = m_attrib->GetAttribute(ATTRIB_DESCRIPTION);
  m_tnsService  = m_attrib->GetAttribute(ATTRIB_SERVERNAME);
  UpdateData(FALSE);

  return TRUE;
}

void
SetupDialog::Synchronise()
{
  UpdateData();
  m_attrib->SetAttribute(ATTRIB_DSN,        m_dataSource);
  m_attrib->SetAttribute(ATTRIB_DESCRIPTION,m_description);
  m_attrib->SetAttribute(ATTRIB_SERVERNAME, m_tnsService);
  // Synchronise all the pages
  m_tab1Dlg->SetAttributes();
  m_tab2Dlg->SetAttributes();
  m_tab3Dlg->SetAttributes();
  m_tab4Dlg->SetAttributes();
}

// SetupDialog message handlers

void 
SetupDialog::OnEnChangeDatasource()
{
  UpdateData();
}

void 
SetupDialog::OnEnChangeDescription()
{
  UpdateData();
}

void 
SetupDialog::OnEnChangeTns()
{
  UpdateData();
}

void 
SetupDialog::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
  int num = TabCtrl_GetCurSel(pNMHDR->hwndFrom);
  switch(num)
  {
    case 0: m_tab1Dlg->ShowWindow(TRUE);
            m_tab2Dlg->ShowWindow(FALSE);
            m_tab3Dlg->ShowWindow(FALSE);
            m_tab4Dlg->ShowWindow(FALSE);
            break;
    case 1: m_tab2Dlg->ShowWindow(TRUE);
            m_tab1Dlg->ShowWindow(FALSE);
            m_tab3Dlg->ShowWindow(FALSE);
            m_tab4Dlg->ShowWindow(FALSE);
            break;
    case 2: m_tab3Dlg->ShowWindow(TRUE);
            m_tab1Dlg->ShowWindow(FALSE);
            m_tab2Dlg->ShowWindow(FALSE);
            m_tab4Dlg->ShowWindow(FALSE);
            break;
    case 3: m_tab4Dlg->ShowWindow(TRUE);
            m_tab1Dlg->ShowWindow(FALSE);
            m_tab2Dlg->ShowWindow(FALSE);
            m_tab3Dlg->ShowWindow(FALSE);
  }
  *pResult = 0;
}

void 
SetupDialog::OnBnClickedOk()
{
  Synchronise();
  OnOK();
}

void 
SetupDialog::OnBnClickedCancel()
{
  OnCancel();
}

void 
SetupDialog::OnBnClickedHelp()
{
  CString path;
  CString driver = m_attrib->GetAttribute("Driver");
  int pos = driver.ReverseFind('\\');
  if(pos > 0)
  {
    path = driver.Left(pos);
  }
  ShellExecute(GetSafeHwnd(),"open","EDO_Documentation.chm",NULL,path.GetString(),SW_SHOW);
}

void 
SetupDialog::OnBnClickedTest()
{
  Synchronise();
  CString connect = m_attrib->MakeConnectionString();
  if(connect.IsEmpty())
  {
    ::MessageBox(GetSafeHwnd(),"No connection string","EDO ODBC",MB_OK|MB_ICONERROR);
  }
  else
  {
    SQLHANDLE env = NULL;
    SQLHANDLE dbc = NULL;
    if(SQLAllocHandle(SQL_HANDLE_ENV,NULL,&env) == SQL_SUCCESS)
    {
      SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0);
      if(SQLAllocHandle(SQL_HANDLE_DBC,env, &dbc) == SQL_SUCCESS)
      {
        if(SQLDriverConnect(dbc,m_hWnd,(SQLCHAR*)connect.GetString(),connect.GetLength(),NULL,0,NULL,SQL_DRIVER_COMPLETE) == SQL_SUCCESS)
        {
          CString msg;
          msg.Format("Connection to [%s] tests out OK!",m_dataSource);
          ::MessageBox(GetSafeHwnd(),msg,"EDO ODBC",MB_OK|MB_ICONEXCLAMATION);
          SQLDisconnect(dbc);
        }
        else
        {
          ODBCError(GetSafeHwnd(),dbc);
        }
        SQLFreeHandle(SQL_HANDLE_DBC,dbc);
      }
      SQLFreeHandle(SQL_HANDLE_ENV,env);
    }
  }
}
