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
// Contents:  Page3 of the setup dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//

#include "stdafx.h"
#include "EDO_Setup.h"
#include "SetupPage3.h"
#include "FileDialog.h"

// SetupPage3 dialog

IMPLEMENT_DYNAMIC(SetupPage3, CDialog)

SetupPage3::SetupPage3(CWnd* p_parent,Attributes* p_attrib)
           :CDialog(SetupPage3::IDD, p_parent)
           ,m_attrib(p_attrib)
{
}

SetupPage3::~SetupPage3()
{
}

void SetupPage3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  DDX_Text   (pDX,IDC_TRACEFILE,   m_traceFile);
  DDX_Control(pDX,IDC_TRACE,       m_traceLevel);
  DDX_Control(pDX,IDC_CHOOSE,      m_choose);
  DDX_Text   (pDX,IDC_TRANSOPTION, m_transOption);
  DDX_Text   (pDX,IDC_TRANSLIB,    m_transLibrary);
  DDX_Control(pDX,IDC_CHOOSELIB,   m_chooseLib);
}

BEGIN_MESSAGE_MAP(SetupPage3, CDialog)
  ON_BN_CLICKED  (IDC_TRACE,      &SetupPage3::OnBnClickedTrace)
  ON_EN_KILLFOCUS(IDC_TRACEFILE,  &SetupPage3::OnEnChangeTracefile)
  ON_BN_CLICKED  (IDC_CHOOSE,     &SetupPage3::OnBnClickedChoose)
  ON_EN_KILLFOCUS(IDC_TRANSOPTION,&SetupPage3::OnEnChangeTransoption)
  ON_EN_KILLFOCUS(IDC_TRANSLIB,   &SetupPage3::OnEnChangeTranslib)
  ON_BN_CLICKED  (IDC_CHOOSELIB,  &SetupPage3::OnBnClickedChooselib)
END_MESSAGE_MAP()

BOOL
SetupPage3::OnInitDialog()
{
  CDialog::OnInitDialog();
  m_traceFile    = m_attrib->GetAttribute(ATTRIB_TRACEFILE);
  m_transOption  = m_attrib->GetAttribute(ATTRIB_TRANSOPTION);
  m_transLibrary = m_attrib->GetAttribute(ATTRIB_TRANSLIB);
  m_traceLevel.SetCheck(atoi(m_attrib->GetAttribute(ATTRIB_TRACELEVEL)));
  if(m_traceLevel.GetCheck() == FALSE)
  {
     CWnd* w = GetDlgItem(IDC_TRACEFILE);
     w->EnableWindow(FALSE);
     m_choose.EnableWindow(FALSE);
  }
  UpdateData(FALSE);
  return TRUE;
}

void
SetupPage3::SetAttributes()
{
  CString level = m_traceLevel.GetCheck() ? "1" : "0";
  m_attrib->SetAttribute(ATTRIB_TRACEFILE,   m_traceFile);
  m_attrib->SetAttribute(ATTRIB_TRACELEVEL,  level);
  m_attrib->SetAttribute(ATTRIB_TRANSOPTION, m_transOption);
  m_attrib->SetAttribute(ATTRIB_TRANSLIB,    m_transLibrary);
}

// SetupPage3 message handlers

void 
SetupPage3::OnBnClickedTrace()
{
  BOOL use = m_traceLevel.GetCheck();
  CWnd* w = GetDlgItem(IDC_TRACEFILE);
  w->EnableWindow(use);
  m_choose.EnableWindow(use);
}

void 
SetupPage3::OnEnChangeTracefile()
{
  UpdateData();
}

void 
SetupPage3::OnBnClickedChoose()
{
  CString filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*|";
  DocFileDialog dlg(true,"Choose a trace file","txt",m_traceFile,0,filter);
  if(dlg.DoModal() == IDOK)
  {
    m_traceFile = dlg.GetChosenFile();
    UpdateData(FALSE);
  }
}

void SetupPage3::OnEnChangeTransoption()
{
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
}

void SetupPage3::OnEnChangeTranslib()
{
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
}

void 
SetupPage3::OnBnClickedChooselib()
{
  CString filter = "Translation library (*.dll)|*.dll|All files (*.*)|*.*|";
  DocFileDialog dlg(true,"Choose a translation library","dll",m_transLibrary,0,filter);
  if(dlg.DoModal() == IDOK)
  {
    m_transLibrary = dlg.GetChosenFile();
    UpdateData(FALSE);
  }
}
