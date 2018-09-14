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
// Contents:  Page4 of the setup dialog
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
//
#include "stdafx.h"
#include "EDO_Setup.h"
#include "SetupDialog.h"
#include "Errors.h"

// SetupPage4 dialog

IMPLEMENT_DYNAMIC(SetupPage4, CDialog)

SetupPage4::SetupPage4(CWnd* p_parent,Attributes* p_attrib)
           :CDialog(SetupPage4::IDD, p_parent)
           ,m_attrib(p_attrib)
           ,m_flushed(true)
{
}

SetupPage4::~SetupPage4()
{
}

void SetupPage4::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  DDX_Control(pDX,IDC_NLSTYPE,        m_comboNLS);
  DDX_Text   (pDX,IDC_NLSDATE,        m_NLSDate);
  DDX_Text   (pDX,IDC_NLSTIME,        m_NLSTime);
  DDX_Text   (pDX,IDC_NLSTIMESTAMP,   m_NLSTimeStamp);
  DDX_Text   (pDX,IDC_NLSTIMESTAMPTZ, m_NLSTimeStampTZ);
}


BEGIN_MESSAGE_MAP(SetupPage4, CDialog)
  ON_CBN_SELCHANGE(IDC_NLSTYPE,       &SetupPage4::OnCbnSelchangeNlstype)
  ON_EN_KILLFOCUS (IDC_NLSDATE,       &SetupPage4::OnEnChangeNlsdate)
  ON_EN_KILLFOCUS (IDC_NLSTIME,       &SetupPage4::OnEnChangeNlstime)
  ON_EN_KILLFOCUS (IDC_NLSTIMESTAMP,  &SetupPage4::OnEnChangeNlstimestamp)
  ON_EN_KILLFOCUS (IDC_NLSTIMESTAMPTZ,&SetupPage4::OnEnChangeNlstimestamptz)
  ON_BN_CLICKED(IDC_SESSION, &SetupPage4::OnBnClickedSession)
END_MESSAGE_MAP()

BOOL
SetupPage4::OnInitDialog()
{
  CDialog::OnInitDialog();
  m_NLS_TYPE = atoi(m_attrib->GetAttribute(ATTRIB_NLSTYPE));
  if(m_NLS_TYPE <1 || m_NLS_TYPE > 3)
  {
    // Defaults to ORACLE NLS
    m_NLS_TYPE = 1;
  }
  if(m_NLS_TYPE > 1)
  {
    m_NLSDate        = m_attrib->GetAttribute(ATTRIB_NLSDATE);
    m_NLSTime        = m_attrib->GetAttribute(ATTRIB_NLSTIME);
    m_NLSTimeStamp   = m_attrib->GetAttribute(ATTRIB_NLSTIMESTAMP);
    m_NLSTimeStampTZ = m_attrib->GetAttribute(ATTRIB_NLSTIMESTAMPTZ);
  }
  SetNLS();
  m_comboNLS.SetCurSel(m_NLS_TYPE - 1);
  UpdateData(FALSE);
  return TRUE;
}

void
SetupPage4::SetAttributes()
{
  CString type;
  type.Format("%d",m_NLS_TYPE);
  m_attrib->SetAttribute(ATTRIB_NLSTYPE,type);
  if(m_NLS_TYPE > 1)
  {
    // Do for ODBC and specific settings
    m_attrib->SetAttribute(ATTRIB_NLSDATE,        m_NLSDate);
    m_attrib->SetAttribute(ATTRIB_NLSTIME,        m_NLSTime);
    m_attrib->SetAttribute(ATTRIB_NLSTIMESTAMP,   m_NLSTimeStamp);
    m_attrib->SetAttribute(ATTRIB_NLSTIMESTAMPTZ, m_NLSTimeStampTZ);
  }
  else
  {
    m_attrib->SetAttribute(ATTRIB_NLSDATE,        "");
    m_attrib->SetAttribute(ATTRIB_NLSTIME,        "");
    m_attrib->SetAttribute(ATTRIB_NLSTIMESTAMP,   "");
    m_attrib->SetAttribute(ATTRIB_NLSTIMESTAMPTZ, "");
  }
}

void
SetupPage4::SetNLS()
{
  if(m_flushed && m_NLS_TYPE == 1)
  {
    // USE Standard from Oracle settings (registry/env)
    m_NLSDate = 
    m_NLSTime =
    m_NLSTimeStamp = 
    m_NLSTimeStampTZ = "Login to see me!";
    m_flushed = false;
  }
  if(m_NLS_TYPE == 2)
  {
    m_NLSDate        = "YYYY-MM-DD";
    m_NLSTime        = "HH24:MI:SS";
    m_NLSTimeStamp   = "YYYY-MM-DD HH24:MI:SSXFF";
    m_NLSTimeStampTZ = "YYYY-MM-DD HH24:MI:SSXFF TZR";
    m_flushed = true;
  }
  if(m_NLS_TYPE == 3)
  {
    CString NLSDate        = m_attrib->GetAttribute(ATTRIB_NLSDATE);
    CString NLSTime        = m_attrib->GetAttribute(ATTRIB_NLSTIME);
    CString NLSTimeStamp   = m_attrib->GetAttribute(ATTRIB_NLSTIMESTAMP);
    CString NLSTimeStampTZ = m_attrib->GetAttribute(ATTRIB_NLSTIMESTAMPTZ);
    if(!NLSDate.IsEmpty() || !NLSTime.IsEmpty() || !NLSTimeStamp.IsEmpty() || !NLSTimeStampTZ.IsEmpty())
    {
      m_NLSDate        = NLSDate;
      m_NLSTime        = NLSTime;
      m_NLSTimeStamp   = NLSTimeStamp;
      m_NLSTimeStampTZ = NLSTimeStampTZ;
    }
  }
  CWnd* w;
  w = GetDlgItem(IDC_NLSDATE);       w->EnableWindow(m_NLS_TYPE == 3);
  w = GetDlgItem(IDC_NLSTIME);       w->EnableWindow(m_NLS_TYPE == 3);
  w = GetDlgItem(IDC_NLSTIMESTAMP);  w->EnableWindow(m_NLS_TYPE == 3);
  w = GetDlgItem(IDC_NLSTIMESTAMPTZ);w->EnableWindow(m_NLS_TYPE == 3);

  UpdateData(FALSE);
}

// SetupPage4 message handlers

void 
SetupPage4::OnCbnSelchangeNlstype()
{
  int ind = m_comboNLS.GetCurSel();
  if(ind >= 0)
  {
    m_NLS_TYPE = 1 + ind;
    SetNLS();
  }
}

void 
SetupPage4::OnEnChangeNlsdate()
{
  m_flushed = true;
  UpdateData();
}

void 
SetupPage4::OnEnChangeNlstime()
{
  m_flushed = true;
  UpdateData();
}

void 
SetupPage4::OnEnChangeNlstimestamp()
{
  m_flushed = true;
  UpdateData();
}

void 
SetupPage4::OnEnChangeNlstimestamptz()
{
  m_flushed = true;
  UpdateData();
}

void 
SetupPage4::OnBnClickedSession()
{
  SetupDialog* dialog = (SetupDialog*) GetParent();
  dialog->Synchronise();
  CString connect = m_attrib->MakeConnectionString();
  if(connect.IsEmpty())
  {
    ::MessageBox(GetSafeHwnd(),"No connection string","EDO ODBC",MB_OK|MB_ICONERROR);
  }
  else
  {
    SQLHANDLE env = NULL;
    SQLHANDLE dbc = NULL;
    if(SQLAllocHandle(SQL_HANDLE_ENV,NULL,&env) == SQL_SUCCESS)
    {
      SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0);
      if(SQLAllocHandle(SQL_HANDLE_DBC,env, &dbc) == SQL_SUCCESS)
      {
        if(SQLDriverConnect(dbc,m_hWnd,(SQLCHAR*)connect.GetString(),connect.GetLength(),NULL,0,NULL,SQL_DRIVER_COMPLETE) == SQL_SUCCESS)
        {

          char* query = "SELECT parameter\n"
                        "      ,value\n"
                        "  FROM sys.nls_session_parameters";
          SQLHSTMT statement;
          if(SQLAllocHandle(SQL_HANDLE_STMT,dbc,&statement) == SQL_SUCCESS)
          {
            if(SQLPrepare(statement,(SQLCHAR*)query,SQL_NTS) == SQL_SUCCESS)
            {
              if(SQLExecute(statement) == SQL_SUCCESS)
              {
                char parameter[31];
                char value[41];
                SQLLEN parLen,valLen;
                while(SQLFetch(statement) == SQL_SUCCESS)
                {
                  SQLGetData(statement,1,SQL_CHAR,parameter,31,&parLen);
                  SQLGetData(statement,2,SQL_CHAR,value,    41,&valLen);
                  parameter[parLen] = 0;
                  value    [valLen] = 0;
                       if (strcmp(parameter,"NLS_DATE_FORMAT")         == 0) m_NLSDate = value;
                  else if (strcmp(parameter,"NLS_TIME_FORMAT")         == 0) m_NLSTime = value;
                  else if (strcmp(parameter,"NLS_TIMESTAMP_FORMAT")    == 0) m_NLSTimeStamp   = value;
                  else if (strcmp(parameter,"NLS_TIMESTAMP_TZ_FORMAT") == 0) m_NLSTimeStampTZ = value;
                }
              }
            }
            SQLFreeStmt(statement,SQL_DROP);
          }
          SQLDisconnect(dbc);
        }
        else
        {
          ODBCError(GetSafeHwnd(),dbc);
        }
        SQLFreeHandle(SQL_HANDLE_DBC,dbc);
      }
      SQLFreeHandle(SQL_HANDLE_ENV,env);
    }
  }
  m_flushed = false;
  UpdateData(FALSE);
}
