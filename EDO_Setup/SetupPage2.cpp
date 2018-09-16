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
#include "stdafx.h"
#include "EDO_Setup.h"
#include "SetupPage2.h"

// SetupPage2 dialog

IMPLEMENT_DYNAMIC(SetupPage2, CDialog)

SetupPage2::SetupPage2(CWnd* p_parent,Attributes* p_attrib)
           :CDialog(SetupPage2::IDD, p_parent)
           ,m_attrib(p_attrib)
{
}

SetupPage2::~SetupPage2()
{
}

void SetupPage2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  DDX_Control(pDX,IDC_PREFETCH,   m_prefetch);
  DDX_Control(pDX,IDC_FAILOVER,   m_failover);
  DDX_Text(pDX,IDC_PRECOUNT,      m_prefetchCount);
  DDX_Text(pDX,IDC_PREMEMORY,     m_prefetchMemory);
  DDX_Text(pDX,IDC_FO_RETRYCOUNT, m_failoverCount);
  DDX_Text(pDX,IDC_FO_RETRYTIME,  m_failoverTime);
  DDV_MinMaxInt(pDX,atoi(m_prefetchCount), 0,10000);
  DDV_MinMaxInt(pDX,atoi(m_prefetchMemory),0,100 * 1024 * 1024);
  DDV_MinMaxInt(pDX,atoi(m_failoverCount), 0,20);
  DDV_MinMaxInt(pDX,atoi(m_failoverTime),  0,20);
}

BEGIN_MESSAGE_MAP(SetupPage2, CDialog)
  ON_BN_CLICKED  (IDC_PREFETCH,     &SetupPage2::OnBnClickedPrefetch)
  ON_EN_KILLFOCUS(IDC_PRECOUNT,     &SetupPage2::OnEnChangePrecount)
  ON_EN_KILLFOCUS(IDC_PREMEMORY,    &SetupPage2::OnEnChangePrememory)
  ON_BN_CLICKED  (IDC_FAILOVER,     &SetupPage2::OnBnClickedFailover)
  ON_EN_KILLFOCUS(IDC_FO_RETRYCOUNT,&SetupPage2::OnEnChangeFoRetrycount)
  ON_EN_KILLFOCUS(IDC_FO_RETRYTIME, &SetupPage2::OnEnChangeFoRetrytime)
END_MESSAGE_MAP()

BOOL
SetupPage2::OnInitDialog()
{
  CDialog::OnInitDialog();
  ReadAttributes();
  UpdateData(FALSE);
  return TRUE;
}

void
SetupPage2::ReadAttributes()
{
  m_prefetchCount  = m_attrib->GetAttribute(ATTRIB_PRECOUNT);
  m_prefetchMemory = m_attrib->GetAttribute(ATTRIB_PREMEMORY);
  m_failoverCount  = m_attrib->GetAttribute(ATTRIB_FAILCOUNT);
  m_failoverTime   = m_attrib->GetAttribute(ATTRIB_FAILTIME);
  if(m_prefetchCount .IsEmpty()) m_prefetchCount  =   "100";
  if(m_prefetchMemory.IsEmpty()) m_prefetchMemory = "65536";
  if(m_failoverCount .IsEmpty()) m_failoverCount  =    "10";
  if(m_failoverTime  .IsEmpty()) m_failoverTime   =     "5";
  UpdateData(FALSE);
  if(m_prefetchCount  == "100" &&
     m_prefetchMemory == "65536")
  {
    UsePrefetch(FALSE);
  }
  else
  {
    m_prefetch.SetCheck(TRUE);
  }
  if(m_failoverCount == "0" &&
     m_failoverTime  == "0"  )
  {
    UseFailover(FALSE);
  }
  else
  {
    m_failover.SetCheck(TRUE);
  }
}

void
SetupPage2::SetAttributes()
{
  UpdateData();
  m_attrib->SetAttribute(ATTRIB_PRECOUNT, m_prefetchCount);
  m_attrib->SetAttribute(ATTRIB_PREMEMORY,m_prefetchMemory);
  m_attrib->SetAttribute(ATTRIB_FAILCOUNT,m_failoverCount);
  m_attrib->SetAttribute(ATTRIB_FAILTIME, m_failoverTime);
}

void
SetupPage2::UsePrefetch(BOOL p_use)
{
  CWnd* c = GetDlgItem(IDC_PRECOUNT);
  CWnd* m = GetDlgItem(IDC_PREMEMORY);
  c->EnableWindow(p_use);
  m->EnableWindow(p_use);
  if(p_use == FALSE)
  {
    m_prefetchCount  = "100";
    m_prefetchMemory = "65536";
    UpdateData(FALSE);
  }
}

void
SetupPage2::UseFailover(BOOL p_use)
{
  CWnd* c = GetDlgItem(IDC_FO_RETRYCOUNT);
  CWnd* t = GetDlgItem(IDC_FO_RETRYTIME);
  c->EnableWindow(p_use);
  t->EnableWindow(p_use);
  if(p_use == FALSE)
  {
    m_failoverCount = "0";
    m_failoverTime  = "0";
    UpdateData(FALSE);
  }
}

// SetupPage2 message handlers

void 
SetupPage2::OnBnClickedPrefetch()
{
  UsePrefetch(m_prefetch.GetCheck());
}

void 
SetupPage2::OnEnChangePrecount()
{
  UpdateData();
}

void 
SetupPage2::OnEnChangePrememory()
{
  UpdateData();
}


void 
SetupPage2::OnBnClickedFailover()
{
  UseFailover(m_failover.GetCheck());
}

void 
SetupPage2::OnEnChangeFoRetrycount()
{
  UpdateData();
}

void 
SetupPage2::OnEnChangeFoRetrytime()
{
  UpdateData();
}
