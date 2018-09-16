//////////////////////////////////////////////////////////////////////////
//
// EDO = Edo's Driver for Oracle
// ORACLE ODBC DRIVER for ODBC 3.51
//
// Copyright (C) 2008-2015 ir. Wicher Edo Huisman
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
// Contents:  Page1 of the setup dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "EDO_Setup.h"
#include "SetupPage1.h"

// SetupPage1 dialog

IMPLEMENT_DYNAMIC(SetupPage1, CDialog)

SetupPage1::SetupPage1(CWnd* p_parent,Attributes* p_attrib)
           :CDialog(SetupPage1::IDD,p_parent)
           ,m_attrib(p_attrib)
{
}

SetupPage1::~SetupPage1()
{
}

void SetupPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  DDX_Control(pDX,IDC_READONLY,   m_readonly);
  DDX_Control(pDX,IDC_ENSURE,     m_ensure);
  DDX_Control(pDX,IDC_METADATAID, m_metadataID);
  DDX_Control(pDX,IDC_INTSPEEDUP, m_intSpeedup);
  DDX_Text   (pDX,IDC_MAXLEN,     m_maxLen);
  DDV_MinMaxInt(pDX,atoi(m_maxLen),0,0x7FFFFFFF);
}

BEGIN_MESSAGE_MAP(SetupPage1, CDialog)
  ON_BN_CLICKED  (IDC_READONLY,   &SetupPage1::OnBnClickedReadonly)
  ON_BN_CLICKED  (IDC_ENSURE,     &SetupPage1::OnBnClickedEnsure)
  ON_BN_CLICKED  (IDC_METADATAID, &SetupPage1::OnBnClickedMetadataid)
  ON_BN_CLICKED  (IDC_INTSPEEDUP, &SetupPage1::OnBnClickedIntspeedup)
  ON_EN_KILLFOCUS(IDC_MAXLEN,     &SetupPage1::OnEnChangeMaxlen)
END_MESSAGE_MAP()

BOOL
SetupPage1::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_ensure    .SetCheck(atoi(m_attrib->GetAttribute(ATTRIB_STATISTICS)));
  m_readonly  .SetCheck(atoi(m_attrib->GetAttribute(ATTRIB_READONLY)));
  m_metadataID.SetCheck(atoi(m_attrib->GetAttribute(ATTRIB_METADATAID)));
  m_intSpeedup.SetCheck(atoi(m_attrib->GetAttribute(ATTRIB_INTSPEEDUP)));
  m_maxLen = m_attrib->GetAttribute(ATTRIB_COLUMNMAXLEN);
  UpdateData(FALSE);
  return TRUE;
}

void
SetupPage1::SetAttributes()
{
  UpdateData();
  CString readonly = m_readonly  .GetCheck() ? "1" : "0";
  CString ensure   = m_ensure    .GetCheck() ? "1" : "0";
  CString metaID   = m_metadataID.GetCheck() ? "1" : "0";
  CString intspeed = m_intSpeedup.GetCheck() ? "1" : "0";
  m_attrib->SetAttribute(ATTRIB_STATISTICS,  ensure);
  m_attrib->SetAttribute(ATTRIB_READONLY,    readonly);
  m_attrib->SetAttribute(ATTRIB_METADATAID,  metaID);
  m_attrib->SetAttribute(ATTRIB_COLUMNMAXLEN,m_maxLen);
  m_attrib->SetAttribute(ATTRIB_INTSPEEDUP,  intspeed);
}

// SetupPage1 message handlers

void 
SetupPage1::OnBnClickedReadonly()
{
  UpdateData();
}

void 
SetupPage1::OnBnClickedEnsure()
{
  UpdateData();
  if(m_ensure.GetCheck())
  {
    ::MessageBox(GetSafeHwnd()
                ,"Statistics SQL_ENSURE comes with a heavy performance hit!\n"
                 "Are you sure to turn on this option?"
                ,"Warning"
                ,MB_OK|MB_ICONQUESTION);
  }
}

void 
SetupPage1::OnBnClickedMetadataid()
{
  UpdateData();
}

void 
SetupPage1::OnEnChangeMaxlen()
{
  UpdateData();
}

void 
SetupPage1::OnBnClickedIntspeedup()
{
  UpdateData();
}
