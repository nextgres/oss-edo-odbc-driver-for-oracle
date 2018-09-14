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
// Contents:  Implementation of the ODBC main object
//            Static object for the driver (all environments/connections)
//            Implements logging and the alloc/free of environments
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "MainObject.h"
#include "Environment.h"
#include <stdio.h>
#include <oci.h>
#include <odbcinst.h>
#include <sqlext.h>
#include <locale.h>
#include <math.h>

#pragma warning(disable : 4996)

// Macro re-definition only for this module!!
#undef  WRITELOG
#define WRITELOG(p_format, ...) if(LogSystemOn())\
{\
  PrintLog(p_format,__VA_ARGS__);\
}

// Main module object
// This object auto start, when this module is loaded. And auto stop, when this module is begin unloading
ODBCMainObject ODBCDriver;
// Codepage at startup of this module
UINT codePage = CP_ACP;
// Locale object for sscanf/sprintf conversions
_locale_t ODBC_Locale;

// Supported ODBC functions
static const int supportedFunctions [] = 
{
     SQL_API_SQLALLOCCONNECT         // 1
    ,SQL_API_SQLALLOCENV             // 2
    ,SQL_API_SQLALLOCHANDLE          // 1001
    ,SQL_API_SQLALLOCHANDLESTD	     // 73
    ,SQL_API_SQLALLOCSTMT            // 3
    ,SQL_API_SQLBINDCOL              // 4
    ,SQL_API_SQLBINDPARAM            // 1002
    ,SQL_API_SQLBINDPARAMETER        // 72
    ,SQL_API_SQLBROWSECONNECT        // 55
  //,SQL_API_SQLBULKOPERATIONS	     // 24
    ,SQL_API_SQLCANCEL               // 5
    ,SQL_API_SQLCLOSECURSOR          // 1003
    ,SQL_API_SQLCOLATTRIBUTE         // 6
    ,SQL_API_SQLCOLATTRIBUTES        // 6 
    ,SQL_API_SQLCOLUMNS              // 40
    ,SQL_API_SQLCOLUMNPRIVILEGES     // 56
    ,SQL_API_SQLCONNECT              // 7
    ,SQL_API_SQLCOPYDESC             // 1004
  //,SQL_API_SQLDATASOURCES          // 57    Don't implement -> DriverManager does this
    ,SQL_API_SQLDESCRIBECOL          // 8
    ,SQL_API_SQLDESCRIBEPARAM        // 58
    ,SQL_API_SQLDISCONNECT           // 9
    ,SQL_API_SQLDRIVERCONNECT	       // 41 
  //,SQL_API_SQLDRIVERS              // 71     Don't implement -> DriverManager does this
    ,SQL_API_SQLENDTRAN              // 1005
    ,SQL_API_SQLERROR                // 10
    ,SQL_API_SQLEXECDIRECT           // 11
    ,SQL_API_SQLEXECUTE              // 12
    ,SQL_API_SQLEXTENDEDFETCH        // 59
    ,SQL_API_SQLFETCH                // 13
    ,SQL_API_SQLFETCHSCROLL          // 1021
    ,SQL_API_SQLFOREIGNKEYS          // 60
    ,SQL_API_SQLFREECONNECT          // 14
    ,SQL_API_SQLFREEENV              // 15
    ,SQL_API_SQLFREEHANDLE           // 1006
    ,SQL_API_SQLFREESTMT             // 16
    ,SQL_API_SQLGETCONNECTATTR       // 1007
    ,SQL_API_SQLGETCONNECTOPTION     // 42
    ,SQL_API_SQLGETCURSORNAME        // 17
    ,SQL_API_SQLGETDATA              // 43
    ,SQL_API_SQLGETDESCFIELD         // 1008
    ,SQL_API_SQLGETDESCREC           // 1009
    ,SQL_API_SQLGETDIAGFIELD         // 1010
    ,SQL_API_SQLGETDIAGREC           // 1011
    ,SQL_API_SQLGETENVATTR           // 1012
    ,SQL_API_SQLGETFUNCTIONS         // 44
    ,SQL_API_SQLGETINFO              // 45
    ,SQL_API_SQLGETSTMTATTR          // 1014
    ,SQL_API_SQLGETSTMTOPTION        // 46
    ,SQL_API_SQLGETTYPEINFO          // 47
    ,SQL_API_SQLMORERESULTS          // 61
    ,SQL_API_SQLNATIVESQL            // 62
    ,SQL_API_SQLNUMPARAMS            // 63
    ,SQL_API_SQLNUMRESULTCOLS        // 18
    ,SQL_API_SQLPARAMDATA            // 48
    ,SQL_API_SQLPARAMOPTIONS         // 64
    ,SQL_API_SQLPREPARE              // 19
    ,SQL_API_SQLPRIMARYKEYS          // 65
    ,SQL_API_SQLPROCEDURECOLUMNS     // 66
    ,SQL_API_SQLPROCEDURES           // 67
    ,SQL_API_SQLPUTDATA              // 49
    ,SQL_API_SQLROWCOUNT             // 20
    ,SQL_API_SQLSETCONNECTATTR       // 1016
    ,SQL_API_SQLSETCONNECTOPTION     // 50
    ,SQL_API_SQLSETCURSORNAME        // 21
    ,SQL_API_SQLSETDESCFIELD         // 1017
    ,SQL_API_SQLSETDESCREC           // 1018
    ,SQL_API_SQLSETENVATTR           // 1019
    ,SQL_API_SQLSETPARAM             // 22
    ,SQL_API_SQLSETPOS               // 68
    ,SQL_API_SQLSETSCROLLOPTIONS     // 69    // Deprecated 1.0 -> SQLGetInfo / SQLSetStmtAttr
    ,SQL_API_SQLSETSTMTATTR          // 1020
    ,SQL_API_SQLSETSTMTOPTION        // 51
    ,SQL_API_SQLSPECIALCOLUMNS       // 52
    ,SQL_API_SQLSTATISTICS           // 53
    ,SQL_API_SQLTABLEPRIVILEGES      // 70
    ,SQL_API_SQLTABLES               // 54
    ,SQL_API_SQLTRANSACT             // 23
};

SQLUSMALLINT ODBCFunctions_2[100];
SQLUSMALLINT ODBCFunctions_3[SQL_API_ODBC3_ALL_FUNCTIONS_SIZE];

ODBCMainObject::ODBCMainObject()
               :m_logfile(NULL)
{
  // Get the codepage for Unicode conversions
  codePage = GetACP();
  setlocale(LC_ALL,".ACP");

  // Locale needed for floating point conversions
  ODBC_Locale = _create_locale(LC_ALL,"C");

  // Open the log
  OpenDriverLog();
  // General init functions
  Init();
}

ODBCMainObject::~ODBCMainObject()
{
  // Remove all pending environments
  // m_environments.RemoveAllObjects();
  // Now close the logfile as a last action
  CloseDriverLog();
}

void
ODBCMainObject::Init()
{
  // Init to zero
  memset(ODBCFunctions_2,0,100);
  memset(ODBCFunctions_3,0,SQL_API_ODBC3_ALL_FUNCTIONS_SIZE);

  for (unsigned int n = 0; n < sizeof (supportedFunctions) / sizeof (supportedFunctions [0]); ++n)
  {
    int fn = supportedFunctions[n];

    if(fn < 100)
    {
      // SQL_API_ALL_FUNCTIONS is used by an ODBC 2.x application 
      // to determine support of ODBC 2.x and earlier functions
      // where functionsArray should be 100 elements
      ODBCFunctions_2 [fn] = SQL_TRUE;
    }
    ASSERT((fn >> 4) < SQL_API_ODBC3_ALL_FUNCTIONS_SIZE);
    ODBCFunctions_3[fn >> 4] |= 1 << (fn & 0xf);
  }
}

void
ODBCMainObject::OpenDriverLog()
{
  // @DEBUGONLY
  // Uncomment this line to build with message to wait for application
  // to set your debugging by attaching to the application
  // ::MessageBox(NULL,"Start of the EDO driver","Debug",MB_OK);
#ifdef EDO_LOGGING

  char logfile[_MAX_PATH+1];
  int num = SQLGetPrivateProfileString("EDO Oracle Driver","Tracefile","",logfile,_MAX_PATH,"ODBCINST.INI");
  if(num > 0)
  {
    char num[10];
    SQLGetPrivateProfileString("EDO Oracle Driver","TraceLevel","",num,8,"ODBCINST.INI");
    if(atoi(num) > 0)
    {
      OpenLog(logfile,false);
    }
  }
#endif
}

void
ODBCMainObject::CloseDriverLog()
{
#ifdef EDO_LOGGING
  if(m_logfile)
  {
    fclose(m_logfile);
    m_logfile = NULL;
  }
#endif
}

bool
ODBCMainObject::OpenLog(const char* p_logname,bool append /*=true*/)
{
  bool ret = false;
#ifdef EDO_LOGGING
  char mode[2];

  Guard guard(m_lock_mutex);
  if(m_logfile)
  {
    fclose(m_logfile);
  }
  // Choose open mode
  if(m_logfile == NULL)
  {
    append = false;
  }
  mode[0] = append ? 'a' : 'w';
  mode[1] = 0;
  // Try to open logfile
  fopen_s(&m_logfile,p_logname,mode);
  if(m_logfile)
  {
    ret = true;
    if(append == false)
    {
      fprintf(m_logfile,"EDO: Edo's Driver for Oracle *** Open Oracle ODBC\n");
      fprintf(m_logfile,"=================================================\n");
    }
  }
  else
  {
    char buf[_MAX_PATH*2];
    ret = false;
    strcpy_s(buf,_MAX_PATH*2,"EDO ODBC: Cannot open the logfile: ");
    strcat_s(buf,_MAX_PATH*2,p_logname);
    ::MessageBoxA(NULL,buf,"ERROR",MB_OK | MB_TASKMODAL);
  }
#endif
  return ret;
}

char*
ConvertTimeToString(time_t t,char *Buff)
{
  tm st;
  static clock_t last = 0;
  clock_t next = clock();
  int mili = 0;

  if(last)
  {
    mili = next - last;
  }
  localtime_s(&st,&t);
  sprintf_s(Buff,50,"%02d-%02d-%02d %02d:%02d:%02d [+%d.%03d]"
           ,st.tm_mday
           ,st.tm_mon+1
           ,st.tm_year%100
           ,st.tm_hour
           ,st.tm_min
           ,st.tm_sec
           ,mili ? mili / 1000 : 0
           ,mili % 1000);
  last = next;
  return Buff;
}

int
ODBCMainObject::PrintLog(const char *FormatMsg,...)
{
  if(m_logfile == NULL)
  {
    return 0;
  }
  Guard  guard(m_lock_mutex);
  char   Buff[50];	
  int    len = 0;
  time_t Nowtime;

  time(&Nowtime);
  ConvertTimeToString(Nowtime,Buff);

  va_list ap;
  va_start(ap,FormatMsg);
  char Msg[10001];
  vsnprintf(Msg,10000,FormatMsg,ap);
  Msg[10000]=0;
  len = fprintf(m_logfile,"%s : %s\n",Buff,Msg);
  fflush(m_logfile);
  va_end(ap);

  return len;
}

void
ODBCMainObject::RemoveEnv(ODBCEnvironment* p_environment)
{
  m_environments.RemoveObject((ODBCBaseObject*)p_environment);
}

SQLRETURN 
ODBCMainObject::AllocEnv(ODBCEnvironment** p_environment)
{
  WRITELOG("SQLAllocHandle(SQL_HANDLE_ENV,NULL,AppRef)");
  Guard guard(m_lock_mutex);

  OCIEnv *hEnv;
  // Allocate OCI Environment object
  if(OCIEnvCreate(&hEnv
                  ,OCI_THREADED | OCI_OBJECT
                  ,NULL
                  ,NULL
                  ,NULL
                  ,NULL
                  ,NULL
                  ,(dvoid**)0) != OCI_SUCCESS)
  {
    WRITELOG("SQLAllocHandle(SQL_HANDLE_ENV,NULL,AppRef) returns SQL_ERROR (FAILED IN OCIEnvCreate)");
    //Failed to create environment
    return SQL_ERROR;
  }
  *p_environment = new ODBCEnvironment(hEnv);
  if(*p_environment == NULL)
  {
    WRITELOG("SQLAllocHandle returns SQL_ERROR (FAILED in new memory)");
    return SQL_ERROR;
  }
  m_environments.AddObject((ODBCBaseObject*)*p_environment);
  WRITELOG("SQLAllocHandle returns SQL_SUCCES (%p)",*p_environment);
  return SQL_SUCCESS;
}

