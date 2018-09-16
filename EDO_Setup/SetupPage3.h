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
// Contents:  Page3 of the setup dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//

#pragma once
#include "Attributes.h"

// SetupPage3 dialog

class SetupPage3 : public CDialog
{
	DECLARE_DYNAMIC(SetupPage3)

public:
	SetupPage3(CWnd* p_parent,Attributes* p_attrib);
	virtual ~SetupPage3();
  BOOL OnInitDialog();
  void SetAttributes();

// Dialog Data
	enum { IDD = IDD_TABTRACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

  Attributes* m_attrib;
  CString     m_traceFile;
  CString     m_transOption;
  CString     m_transLibrary;
  CButton     m_traceLevel;
  CButton     m_choose;
  CButton     m_chooseLib;
public:
  afx_msg void OnBnClickedTrace();
  afx_msg void OnEnChangeTracefile();
  afx_msg void OnBnClickedChoose();
  afx_msg void OnEnChangeTransoption();
  afx_msg void OnEnChangeTranslib();
  afx_msg void OnBnClickedChooselib();
};
