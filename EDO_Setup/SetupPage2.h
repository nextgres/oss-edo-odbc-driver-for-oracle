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
// Contents:  Page2 of the setup dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "Attributes.h"

// SetupPage2 dialog

class SetupPage2 : public CDialog
{
	DECLARE_DYNAMIC(SetupPage2)

public:
	SetupPage2(CWnd* p_parent,Attributes* p_attrib);
	virtual ~SetupPage2();
  BOOL OnInitDialog();
  void SetAttributes();


// Dialog Data
	enum { IDD = IDD_TABOORACLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  void ReadAttributes();
  void UsePrefetch(BOOL p_use);
  void UseFailover(BOOL p_use);

  CString     m_prefetchCount;
  CString     m_prefetchMemory;
  CString     m_failoverCount;
  CString     m_failoverTime;
  Attributes* m_attrib;
  CButton     m_prefetch;
  CButton     m_failover;
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnEnChangePrecount();
  afx_msg void OnEnChangePrememory();
  afx_msg void OnBnClickedPrefetch();
  afx_msg void OnBnClickedFailover();
  afx_msg void OnEnChangeFoRetrycount();
  afx_msg void OnEnChangeFoRetrytime();
};
