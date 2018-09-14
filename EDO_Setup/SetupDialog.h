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
// Contents:  Interface for the setup dialog for the ODBC manager
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "SetupPage1.h"
#include "SetupPage2.h"
#include "SetupPage3.h"
#include "SetupPage4.h"
#include "Attributes.h"

// SetupDialog dialog

class SetupDialog : public CDialog
{
	DECLARE_DYNAMIC(SetupDialog)

public:
	SetupDialog(CWnd*   p_parent
             ,CString p_mode
             ,CString p_dataSource
             ,Attributes* p_attrib);
	virtual ~SetupDialog();
  BOOL OnInitDialog();
  void Synchronise();

  // Dialog Data
	enum { IDD = IDD_SETUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

  CString     m_mode;
  CString     m_dataSource;
  CString     m_description;
  CString     m_tnsService;
  CTabCtrl    m_tab;
  SetupPage1* m_tab1Dlg;
  SetupPage2* m_tab2Dlg;
  SetupPage3* m_tab3Dlg;
  SetupPage4* m_tab4Dlg;
  Attributes* m_attrib;
public:
  afx_msg void OnEnChangeDatasource();
  afx_msg void OnEnChangeDescription();
  afx_msg void OnEnChangeTns();
  afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedHelp();
  afx_msg void OnBnClickedTest();
};
