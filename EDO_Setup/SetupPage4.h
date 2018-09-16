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
// Contents:  Page4 of the setup dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "Attributes.h"

// SetupPage4 dialog

class SetupPage4 : public CDialog
{
	DECLARE_DYNAMIC(SetupPage4)

public:
	SetupPage4(CWnd* p_parent,Attributes* p_attrib);
	virtual ~SetupPage4();
  BOOL OnInitDialog();
  void SetAttributes();

// Dialog Data
	enum { IDD = IDD_TABNLS };

protected:
	void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  void SetNLS();

	DECLARE_MESSAGE_MAP()

  Attributes* m_attrib;
  CComboBox   m_comboNLS;
  int         m_NLS_TYPE;
  bool        m_flushed;
  CString     m_NLSDate;
  CString     m_NLSTime;
  CString     m_NLSTimeStamp;
  CString     m_NLSTimeStampTZ;
public:
  afx_msg void OnCbnSelchangeNlstype();
  afx_msg void OnEnChangeNlsdate();
  afx_msg void OnEnChangeNlstime();
  afx_msg void OnEnChangeNlstimestamp();
  afx_msg void OnEnChangeNlstimestamptz();
  afx_msg void OnBnClickedSession();
};
