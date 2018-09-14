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
// Contents:  Error routines for the setup DLL
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

void
InstallerError(HWND p_hwnd)
{
  WORD  len   = 0;
  DWORD error = 0;
  char  message[SQL_MAX_MESSAGE_LENGTH + 2];
  CString total;
  for(int i = 1; i <= 8; ++i)
  {
    int ret = SQLInstallerError(i,&error,message,SQL_MAX_MESSAGE_LENGTH,&len);
    if(ret == SQL_NO_DATA || ret == SQL_ERROR)
    {
      break;
    }
    if(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
      CString extra;
      message[len] = 0;
      extra.Format("ODBC Installer error: %d : %s\n",error,message);
      total += extra;
    }
  }
  if(!total.IsEmpty())
  {
    ::MessageBox(p_hwnd,total,"EDO ODBC",MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
  }
}

void ODBCError(HWND p_hwnd,HDBC handle)
{
  int         diagRecord = 0;  // Starts at 1 at first call
  CString     item;
  CString     errorString;
  CString     type;
  RETCODE     nRetCode;
  SQLCHAR     szSQLState[6];   // SQLStates are 5 char's plus zero
  SQLINTEGER  nativeError = 0;
  SQLSMALLINT cbMsgLength = 0;
  SQLCHAR     szErrorMsg [SQL_MAX_MESSAGE_LENGTH + 1];

  diagRecord = 0;
  while(true)
  {
    nRetCode = SQLGetDiagRec(SQL_HANDLE_DBC
                            ,handle
                            ,++diagRecord
                            ,szSQLState
                            ,&nativeError
                            ,szErrorMsg
                            ,SQL_MAX_MESSAGE_LENGTH
                            ,&cbMsgLength);
    if(nRetCode == SQL_NO_DATA)
    {
      // No (more) information records
      break;
    }
    if(nRetCode == SQL_ERROR || nRetCode == SQL_INVALID_HANDLE)
    {
      char buf[12];
      errorString += type + "Error while retrieving diagnostic record: ";
      sprintf_s(buf,10,"%d",diagRecord);
      errorString += buf;
      break;
    }
    // nRetCode = SQL_SUCCESS || SQL_SUCCESS_WITH_INFO
    if(cbMsgLength > 0 && cbMsgLength <= SQL_MAX_MESSAGE_LENGTH)
    {
      szErrorMsg[cbMsgLength] = 0; // Just to be sure
    }
    if(!errorString.IsEmpty() && errorString.Right(1) != '\n')
    {
      errorString += "\n";
    }
    errorString += type;
    // State error
    szSQLState[5] = 0;
    item.Format("[SQLSTATE=%s]",szSQLState);
    errorString += item;
    // Native error
    item.Format("[NATIVE=%d]",nativeError);
    errorString += item;
    // Error Msg
    CString extra((char*) szErrorMsg);
    extra.Replace("\r","");
    extra.Replace("\n","");
    errorString += extra;
  }
  if(errorString.IsEmpty())
  {
    // No information
    errorString = "No error information!";
  }
  ::MessageBox(p_hwnd,errorString,"EDO ODBC ERROR",MB_OK|MB_ICONERROR);
}

