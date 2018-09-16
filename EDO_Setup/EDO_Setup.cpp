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
// Contents:  Defines the initialization routines for the DLL.
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "EDO_Setup.h"
#include "SetupDialog.h"
#include "Attributes.h"
#include "errors.h"
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CEDO_SetupApp, CWinApp)
END_MESSAGE_MAP()

// CEDO_SetupApp construction
CEDO_SetupApp::CEDO_SetupApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CEDO_SetupApp object
CEDO_SetupApp theApp;

// CEDO_SetupApp initialization
BOOL CEDO_SetupApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}

#define EDO_DRIVER "EDO Open Oracle Driver"

CString
GetDatasource(CString attrib)
{
  CString source;
  int pos = attrib.Find('=');
  if(pos >= 0)
  {
    source = attrib.Mid(pos + 1);
  }
  return source;
}

BOOL INSTAPI
ConfigDriver(HWND    hwndParent,
             WORD    fRequest,
             LPCSTR  lpszDriver,
             LPCSTR  lpszArgs,
             LPSTR   lpszMsg,
             WORD    cchMsgMax,
             WORD*   pcchMsgOut)
{
  return FALSE;
}


BOOL INSTAPI
ConfigDSN(HWND   hwndParent
         ,WORD   request
         ,LPCSTR lpszDriver
         ,LPCSTR lpszAttributes)
{
  // We are called from the outside by LoadLibrary 
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  InitCommonControls();

  CString action;
  switch(request)
  {
    case ODBC_ADD_DSN:          action = "Add %s datasource";         break;
    case ODBC_CONFIG_DSN:       action = "Configure %s datasource";   break;
    case ODBC_REMOVE_DSN:       action = "Remove %s datasource";      break;
  }
  // Get user/system mode
  UWORD intMode;
  CString mode;
  if(SQLGetConfigMode(&intMode) == FALSE)
  {
    InstallerError(hwndParent);
    return FALSE;
  }
  mode.Format(action,(intMode == ODBC_USER_DSN) ? "user" : "system");

#ifdef DEBUG
// DEBUGGING of the call
//   CString text;
//   text.Format("HWND  \t:  %p\n"
//               "Action\t:  %s\n"
//               "Driver\t:  %s\n"
//               "Attribute\t:  %s\n",hwndParent,mode,lpszDriver,lpszAttributes);
//   ::MessageBox(hwndParent,text,"EDO Driver",MB_OK);
#endif

  CString datasource = GetDatasource(lpszAttributes);
  Attributes attrib(hwndParent,datasource);
  // Do the action
  if(request != ODBC_ADD_DSN)
  {
    attrib.ReadAllAttributes();
  }
  if(request == ODBC_REMOVE_DSN)
  {
    if(SQLRemoveDSNFromIni(datasource) == FALSE)
    {
      InstallerError(hwndParent);
      return FALSE;
    }
    return TRUE;  
  }
  CWnd* wnd = CWnd::FromHandle(hwndParent);
  SetupDialog dlg(wnd,mode,datasource,&attrib);
  if(dlg.DoModal() == IDOK)
  {
    if(request == ODBC_ADD_DSN)
    {
      datasource = attrib.GetAttribute(ATTRIB_DSN);
      attrib.SetDatasource(datasource);
      if(SQLWriteDSNToIni(datasource,lpszDriver) == FALSE)
      {
        InstallerError(hwndParent);
        return FALSE;
      }
    }
    return attrib.WriteAllAttributes();
  }
  return FALSE;
}
