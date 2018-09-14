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
// Contents:  Implementation of the Logon dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "LogonDlg.h"

// LogonDlg dialog

IMPLEMENT_DYNAMIC(LogonDlg, CDialog)

LogonDlg::LogonDlg(CWnd*   p_parent
                  ,CString p_datasource
                  ,CString p_user
                  ,CString p_password)
         :CDialog(LogonDlg::IDD, p_parent)
         ,m_datasource(p_datasource)
         ,m_user(p_user)
         ,m_password(p_password)
{
}

LogonDlg::~LogonDlg()
{
}

void 
LogonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  DDX_Text(pDX,IDC_DATASOURCE,m_datasource);
  DDX_Text(pDX,IDC_USER,      m_user);
  DDX_Text(pDX,IDC_PASSWORD,  m_password);
}


BEGIN_MESSAGE_MAP(LogonDlg, CDialog)
  ON_EN_CHANGE(IDC_USER,     &LogonDlg::OnEnChangeUser)
  ON_EN_CHANGE(IDC_PASSWORD, &LogonDlg::OnEnChangePassword)
END_MESSAGE_MAP()

BOOL
LogonDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  return TRUE;
};

// LogonDlg message handlers

void 
LogonDlg::OnEnChangeUser()
{
}

void 
LogonDlg::OnEnChangePassword()
{
}
