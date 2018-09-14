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
// Contents:  Page1 of the setup dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "Attributes.h" 

// SetupPage1 dialog

class SetupPage1 : public CDialog
{
	DECLARE_DYNAMIC(SetupPage1)

public:
	SetupPage1(CWnd* p_parent,Attributes* p_attrib);
	virtual ~SetupPage1();
  BOOL OnInitDialog();
  void SetAttributes();

  // Dialog Data
	enum { IDD = IDD_TABODBC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

  CButton     m_readonly;
  CButton     m_ensure;
  CButton     m_metadataID;
  CButton     m_intSpeedup;
  CString     m_maxLen;
  Attributes* m_attrib;
public:
  afx_msg void OnBnClickedReadonly();
  afx_msg void OnBnClickedEnsure();
  afx_msg void OnBnClickedMetadataid();
  afx_msg void OnEnChangeMaxlen();
  afx_msg void OnBnClickedIntspeedup();
};
