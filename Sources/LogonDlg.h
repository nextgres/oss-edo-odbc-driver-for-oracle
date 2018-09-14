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
// Contents:  Interface of the Logon dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once

// LogonDlg dialog

class LogonDlg : public CDialog
{
	DECLARE_DYNAMIC(LogonDlg)

public:
	LogonDlg(CWnd*   p_parent
          ,CString p_datasource
          ,CString p_user
          ,CString p_password);
	virtual ~LogonDlg();
  BOOL OnInitDialog();

  CString GetUser();
  CString GetPassword();

// Dialog Data
	enum { IDD = IDD_LOGON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

  CString m_datasource;
  CString m_user;
  CString m_password;
public:
  afx_msg void OnEnChangeUser();
  afx_msg void OnEnChangePassword();
};

inline CString 
LogonDlg::GetUser()
{
  return m_user;
}

inline CString 
LogonDlg::GetPassword()
{
  return m_password;
}
