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
// Contents:  ODBCMain Module for ANSI Calls
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "ErrorInfo.h"
#include "BaseObject.h"
#include "MainObject.h"
#include "Environment.h"
#include "Connection.h"
#include "Statement.h"
#include "DescriptorIRD.h"
#include "DescriptorARD.h"
#include "ODBCTypes.h"
#include "Utility.h"
#include <oci.h>
#include <intrin.h>

#ifdef _DEBUG
void DriverCall()
{
  // Use this to set a breakpoint for all interface calls
  __nop();
}
#else
#define DriverCall()
#endif

//////////////////////////////////////////////////////////////////////////
//
// Allocation of a generic ODBC Handle through SQLAllocHandle
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API 
SQLAllocHandle(SQLSMALLINT HandleType
              ,SQLHANDLE   InputHandle
              ,SQLHANDLE*  OutputHandlePtr)
{
  DriverCall();
  WRITELOG("SQLAllocHandle(%s,%p,%p)",GetHandleTypeDescription(HandleType),InputHandle,OutputHandlePtr);

  if(HandleType == SQL_HANDLE_ENV)
  {	
    return ODBCDriver.AllocEnv((ODBCEnvironment**)OutputHandlePtr);		
  }
	if(HandleType == SQL_HANDLE_DBC)
	{
		ODBCBaseObject* Obj = (ODBCBaseObject*)InputHandle;	
		if(Obj->ValidObject() != SQL_HANDLE_ENV)
		{
      WRITELOG("SQLAllocHandle(%s,%p) returns SQL_INVALID_HANDLE",GetHandleTypeDescription(HandleType),InputHandle);
			return SQL_INVALID_HANDLE;
		}
		ODBCEnvironment* environment = (ODBCEnvironment*)Obj;
		return environment->AllocDBConnection((ODBCConnection**)OutputHandlePtr);
	}
	if(HandleType == SQL_HANDLE_STMT)
	{
		ODBCBaseObject* Obj = (ODBCBaseObject*)InputHandle;	
		if(Obj->ValidObject()!=SQL_HANDLE_DBC)
		{
      WRITELOG("SQLAllocHandle(%s,%p) returns SQL_INVALID_HANDLE",GetHandleTypeDescription(HandleType),InputHandle);
			return SQL_INVALID_HANDLE;
		}
		ODBCConnection* connection = (ODBCConnection*)Obj;
		return connection->AllocDBStatement((ODBCStatement**)OutputHandlePtr);
	}
  if(HandleType == SQL_HANDLE_DESC)
  {
    ODBCBaseObject* Obj = (ODBCBaseObject*)InputHandle;
    if(Obj->ValidObject() != SQL_HANDLE_DBC)
    {
      WRITELOG("SQLAllocHandle(%s,%p) returns SQL_INVALID_HANDLE",GetHandleTypeDescription(HandleType),InputHandle);
      return SQL_INVALID_HANDLE;
    }
    ODBCConnection* connection = (ODBCConnection*)Obj;
    return connection->AllocDescriptor((ODBC_ARD**)OutputHandlePtr);
  }
  WRITELOG("SQLAllocHandle(%s,%p) return SQL_ERROR",GetHandleTypeDescription(HandleType),InputHandle);
  return SQL_ERROR;
}

SQLRETURN SQL_API 
SQLAllocHandleStd(SQLSMALLINT fHandleType,
                  SQLHANDLE   hInput,
                  SQLHANDLE*  phOutput)
{
  DriverCall();
  WRITELOG("SQLAllocHandleStd(%d,%p,%p)",fHandleType,hInput,phOutput);
  SQLRETURN ret = SQLAllocHandle(fHandleType,hInput,phOutput);
  if(fHandleType == SQL_HANDLE_ENV && ret == SQL_SUCCESS)
  {
    ODBCBaseObject* Obj = (ODBCBaseObject*)*phOutput;
    if(Obj->ValidObject() != SQL_HANDLE_ENV)
    {
      WRITELOG("SQLAllocHandleStd(%p) returns SQL_INVALID_HANDLE",hInput);
      return SQL_INVALID_HANDLE;
    }
    ODBCEnvironment* lpEnvObj=(ODBCEnvironment*)Obj;
    WRITELOG("SQLAllocHandleStd sets ODBC 3.x");
    lpEnvObj->SetODBCVersion(SQL_OV_ODBC3);
  }
  return ret;
}

//////////////////////////////////////////////////////////////////////////
//
// Deprecated ODBC 1.x & 2.x versions of Allocaction of handles
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API 
SQLAllocEnv(SQLHENV *EnvironmentHandle)
{
  DriverCall();
  WRITELOG("SQLAllocEnv(%p)",EnvironmentHandle);
  return SQLAllocHandle(SQL_HANDLE_ENV,NULL,(SQLHANDLE*)EnvironmentHandle);
}

SQLRETURN SQL_API 
SQLAllocConnect(SQLHENV  EnvironmentHandle
               ,SQLHDBC* ConnectionHandle)
{
  DriverCall();
  WRITELOG("SQLAllocConnect(%p,%p)",EnvironmentHandle,ConnectionHandle);
  return SQLAllocHandle(SQL_HANDLE_DBC
                       ,(SQLHANDLE)EnvironmentHandle
                       ,(SQLHANDLE*)ConnectionHandle);
}

SQLRETURN SQL_API 
SQLAllocStmt(SQLHDBC   ConnectionHandle
            ,SQLHSTMT* StatementHandle)
{
  DriverCall();
  return SQLAllocHandle(SQL_HANDLE_STMT
                       ,(SQLHANDLE)ConnectionHandle
                       ,(SQLHANDLE*)StatementHandle);
}

//////////////////////////////////////////////////////////////////////////
//
// And freeing of the generic ODBC handle
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API 
SQLFreeHandle(SQLSMALLINT HandleType
             ,SQLHANDLE   InputHandle)
{
  DriverCall();
  WRITELOG("SQLFreeHandle(%s,%p)",GetHandleTypeDescription(HandleType),InputHandle);
	ODBCBaseObject *Obj=(ODBCBaseObject*)InputHandle;	
	if(Obj->ValidObject() != HandleType)
	{
    WRITELOG("SQLFreeHandle(%s,%p) returns SQL_INVALID_HANDLE",GetHandleTypeDescription(HandleType),InputHandle);
		return SQL_INVALID_HANDLE;
	}
	SQLRETURN ret = Obj->FreeObjectHandler();
  WRITELOG("SQLFreeHandle(%s,%p) done! Result: %d",GetHandleTypeDescription(HandleType),InputHandle,ret);
  return ret;
}

SQLRETURN SQL_API 
SQLFreeStmt(SQLHSTMT StatementHandle
           ,SQLUSMALLINT Option)
{
  DriverCall();
  WRITELOG("SQLFreeStmt(%p,%d)",StatementHandle,Option);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLFreeStmt(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  if(Option == SQL_DROP)
  {
    return SQLFreeHandle(SQL_HANDLE_STMT,(SQLHANDLE)StatementHandle);
  }
  return statement->SQLFreeStmt(Option);
}

//////////////////////////////////////////////////////////////////////////
//
// Deprecated ODBC 1.x & 2.x versions of SQLFreeHandle
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API 
SQLFreeEnv(SQLHENV EnvironmentHandle)
{
  DriverCall();
  WRITELOG("SQLFreeEnv(%p)",EnvironmentHandle);
  return SQLFreeHandle(SQL_HANDLE_ENV,(SQLHANDLE)EnvironmentHandle);
}

SQLRETURN SQL_API 
SQLFreeConnect(SQLHDBC ConnectionHandle)
{
  DriverCall();
  WRITELOG("SQLFreeConnect(%p)",ConnectionHandle);
  return SQLFreeHandle(SQL_HANDLE_DBC,(SQLHANDLE)ConnectionHandle);
}

//////////////////////////////////////////////////////////////////////////
//
// MAKE CONNECTION WITH THE DATABASE
// SQLConnect
// SQLDriverConnect
// SQLBrowseConnect
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API 
SQLConnect(SQLHDBC     ConnectionHandle
          ,SQLCHAR*    ServerName
          ,SQLSMALLINT NameLength1
          ,SQLCHAR*    UserName
          ,SQLSMALLINT NameLength2
          ,SQLCHAR*    Authentication
          ,SQLSMALLINT NameLength3)
{
  DriverCall();
  WRITELOG("SQLConnect(%p,%p,%d,%p,%d,%p,%d)",ConnectionHandle
          ,ServerName,NameLength1
          ,UserName,  NameLength2
          ,Authentication,NameLength3);
  ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLConnect(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;
  CString server = MakeDriverString(ServerName,    NameLength1);
  CString user   = MakeDriverString(UserName,      NameLength2);
  CString pwd    = MakeDriverString(Authentication,NameLength3);
  return connection->SQLConnect(server,user,pwd);
}

SQLRETURN SQL_API 
SQLDriverConnect(SQLHDBC      hdbc
                ,SQLHWND      hwnd
                ,SQLCHAR*     szConnStrIn
                ,SQLSMALLINT  cbConnStrIn
                ,SQLCHAR*     szConnStrOut
                ,SQLSMALLINT  cbConnStrOutMax
                ,SQLSMALLINT* pcbConnStrOut
                ,SQLUSMALLINT fDriverCompletion)
{
  DriverCall();
  // Get static module state of MFC in case we do a connection dialog
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  ODBCBaseObject* Obj = (ODBCBaseObject*)hdbc;	
  //printf("SQLDriverConnect 1\n");
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLDriverConnect(%p) returns SQL_INVALID_HANDLE",hdbc);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;

  CString connectString = MakeDriverString(szConnStrIn,cbConnStrIn);
  return connection->SQLDriverConnect(hwnd
                                     ,connectString
                                     ,szConnStrOut
                                     ,cbConnStrOutMax
                                     ,pcbConnStrOut
                                     ,fDriverCompletion);
}

SQLRETURN SQL_API
SQLBrowseConnect(SQLHDBC      hdbc
                ,SQLCHAR*     szConnStrIn
                ,SQLSMALLINT  cbConnStrIn
                ,SQLCHAR*     szConnStrOut
                ,SQLSMALLINT  cbConnStrOutMax
                ,SQLSMALLINT* pcbConnStrOut)
{
  DriverCall();
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  WRITELOG("SQLBrowseConnect(%p,%p,%d,%p,%d,%p)",hdbc,szConnStrIn,cbConnStrIn,szConnStrOut,cbConnStrOutMax,pcbConnStrOut);
  ODBCBaseObject* obj = (ODBCBaseObject*)hdbc;
  if(obj->ValidObject() != SQL_HANDLE_DBC)
  {
    WRITELOG("SQLBrowseConnect(%p) returns SQL_INVALID_HANDLE",hdbc);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)obj;
  CString connectString = MakeDriverString(szConnStrIn,cbConnStrIn);
  return connection->SQLBrowseConnect(connectString,szConnStrOut,cbConnStrOutMax,pcbConnStrOut);
}



//////////////////////////////////////////////////////////////////////////
//
// Attributes for the Environment
//
SQLRETURN SQL_API 
SQLSetEnvAttr (SQLHENV    EnvironmentHandle
              ,SQLINTEGER Attribute
              ,SQLPOINTER Value
              ,SQLINTEGER StringLength)
{
  DriverCall();
  WRITELOG("SQLSetEnvAttr(%p,%d,%p,%d)",EnvironmentHandle,Attribute,Value,StringLength);
	ODBCBaseObject* Obj = (ODBCBaseObject*)EnvironmentHandle;	
	if(Obj->ValidObject() != SQL_HANDLE_ENV)
	{
    WRITELOG("SQLSetEnvAttr(%p) returns SQL_INVALID_HANDLE",EnvironmentHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCEnvironment* environment = (ODBCEnvironment*)Obj;
	return environment->SetEnvAttr(Attribute,Value,StringLength);	
}

SQLRETURN SQL_API 
SQLGetEnvAttr (SQLHENV     EnvironmentHandle
              ,SQLINTEGER  Attribute
              ,SQLPOINTER  Value
              ,SQLINTEGER  BufferLength
              ,SQLINTEGER* StringLength)
{
  DriverCall();
  WRITELOG("SQLGetEnvAttr(%p,%d,%p,%d,%p)",EnvironmentHandle,Attribute,Value,BufferLength,StringLength);
	ODBCBaseObject* Obj = (ODBCBaseObject*)EnvironmentHandle;	
	if(Obj->ValidObject() != SQL_HANDLE_ENV)
	{
    WRITELOG("SQLGetEnvAttr(%p) returns SQL_INVALID_HANDLE",EnvironmentHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCEnvironment* lpEnvObj = (ODBCEnvironment*)Obj;
	return lpEnvObj->GetEnvAttr(Attribute,Value,BufferLength,StringLength);	
}

//////////////////////////////////////////////////////////////////////////
//
// Attributes of the connection
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API 
SQLSetConnectAttr(SQLHDBC    ConnectionHandle
                 ,SQLINTEGER Attribute
                 ,SQLPOINTER Value
                 ,SQLINTEGER StringLength)
{
  DriverCall();
  WRITELOG("SQLSetConnectAttr(%p,%d,%p,%d)",ConnectionHandle,Attribute,Value,StringLength);
	ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_DBC)
	{
    WRITELOG("SQLSetConnectAttr(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCConnection* connection = (ODBCConnection*)Obj;
	return connection->SQLSetConnectAttr(Attribute,Value,StringLength);	
}

SQLRETURN SQL_API 
SQLGetConnectAttr(SQLHDBC     ConnectionHandle
                 ,SQLINTEGER  Attribute
                 ,SQLPOINTER  Value
                 ,SQLINTEGER  BufferLength
                 ,SQLINTEGER* StringLength)
{
  DriverCall();
  WRITELOG("SQLGetConnectAttr(%p,%d,%p,%d,%p)",ConnectionHandle,Attribute,Value,BufferLength,StringLength);
	ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_DBC)
	{
    WRITELOG("SQLGetConnectAttr(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCConnection* connection = (ODBCConnection*)Obj;
	return connection->SQLGetConnectAttr(Attribute,Value,BufferLength,StringLength);
}

SQLRETURN SQL_API
SQLGetConnectOption(SQLHDBC      ConnectionHandle
                   ,SQLUSMALLINT Option
                   ,SQLPOINTER   Value)
{
  DriverCall();
  WRITELOG("SQLGetConnectOption(%p,%d,%p)",ConnectionHandle,Option,Value);
  ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLGetConnectAttr(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection *lpConnObj=(ODBCConnection*)Obj;
  return lpConnObj->SQLGetConnectOption(Option,Value);
}

SQLRETURN SQL_API
SQLSetConnectOption(SQLHDBC      ConnectionHandle
                   ,SQLUSMALLINT Option
                   ,SQLULEN      Value)
{
  DriverCall();
  WRITELOG("SQLSetConnectOption(%p,%d,%p)",ConnectionHandle,Option,Value);
  ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLSetConnectAttr(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection *lpConnObj=(ODBCConnection*)Obj;
  return lpConnObj->SQLSetConnectOption(Option,Value);
}

SQLRETURN SQL_API 
SQLGetInfo(SQLHDBC      ConnectionHandle
          ,SQLUSMALLINT InfoType
          ,SQLPOINTER   InfoValue
          ,SQLSMALLINT  BufferLength
          ,SQLSMALLINT* StringLength)
{
  DriverCall();
  WRITELOG("SQLGetInfo(%p,%d,%p,%d,%p)",ConnectionHandle,InfoType,InfoValue,BufferLength,StringLength);
	ODBCBaseObject* Obj = (ODBCBaseObject*)ConnectionHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_DBC)
	{
    WRITELOG("SQLGetInfo(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCConnection* lpConnObj = (ODBCConnection*)Obj;
	return lpConnObj->SQLGetInfo(InfoType,InfoValue,BufferLength,StringLength);
}

//////////////////////////////////////////////////////////////////////////
//
// Attributes of the statement
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API 
SQLGetStmtAttr(SQLHSTMT    StatementHandle
              ,SQLINTEGER  Attribute
              ,SQLPOINTER  Value
              ,SQLINTEGER  BufferLength
              ,SQLINTEGER* StringLength)
{
  DriverCall();
  WRITELOG("SQLGetStmtAttr(%p,%d,%p,%d,%p)",StatementHandle,Attribute,Value,BufferLength,StringLength);
	ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject() != SQL_HANDLE_STMT)
	{
    WRITELOG("SQLGetStmtAttr(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement = (ODBCStatement*)Obj;
	return statement->SQLGetStmtAttr(Attribute,Value,BufferLength,StringLength);
}

SQLRETURN SQL_API
SQLSetStmtAttr(SQLHSTMT   StatementHandle
              ,SQLINTEGER Attribute
              ,SQLPOINTER Value
              ,SQLINTEGER StringLength)
{
  DriverCall();
  WRITELOG("SQLSetStmtAttr(%p,%d,%p,%d)",StatementHandle,Attribute,Value,StringLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSetStmtAttr(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLSetStmtAttr(Attribute,Value,StringLength);
}

SQLRETURN SQL_API
SQLGetStmtOption(SQLHSTMT     StatementHandle
                ,SQLUSMALLINT Option
                ,SQLPOINTER   Value)
{
  DriverCall();
  WRITELOG("SQLGetStmtOption(%p,%d,%p)",StatementHandle,Option,Value);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLGetStmtOption(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLGetStmtOption(Option,Value);
}

SQLRETURN SQL_API
SQLSetStmtOption(SQLHSTMT     StatementHandle
                ,SQLUSMALLINT Option
                ,SQLULEN      Value)
{
  DriverCall();
  WRITELOG("SQLSetStmtOption(%p,%d,%d)",StatementHandle,Option,Value);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSetStmtOption(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLSetStmtOption(Option,Value);
}

SQLRETURN  SQL_API 
SQLBindCol(SQLHSTMT     StatementHandle
          ,SQLUSMALLINT ColumnNumber
          ,SQLSMALLINT  TargetType
          ,SQLPOINTER   TargetValue
          ,SQLLEN       BufferLength
          ,SQLLEN*      StrLen_or_Ind)
{
  DriverCall();
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLBindCol(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement = (ODBCStatement*)Obj;
	return statement->SQLBindCol(ColumnNumber,TargetType,TargetValue,BufferLength,StrLen_or_Ind);
}

SQLRETURN SQL_API 
SQLBindParameter(SQLHSTMT     StatementHandle
                ,SQLUSMALLINT ParameterNumber
                ,SQLSMALLINT  InputOutputType
                ,SQLSMALLINT  ValueType
                ,SQLSMALLINT  ParameterType
                ,SQLLEN       ColumnSize
                ,SQLSMALLINT  DecimalDigits
                ,SQLPOINTER   ParameterValuePtr
                ,SQLLEN       BufferLength
                ,SQLLEN*      StrLen_or_IndPtr)
{
  DriverCall();
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLBindParameter(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement = (ODBCStatement*)Obj;
	return statement->SQLBindParameter(ParameterNumber
                                   ,InputOutputType
                                   ,ValueType
                                   ,ParameterType
                                   ,ColumnSize
                                   ,DecimalDigits
                                   ,ParameterValuePtr
                                   ,BufferLength
                                   ,StrLen_or_IndPtr);
}

SQLRETURN SQL_API 
SQLBindParam(SQLHSTMT     StatementHandle
            ,SQLUSMALLINT ParameterNumber
            ,SQLSMALLINT  ValueType
            ,SQLSMALLINT  ParameterType
            ,SQLLEN       ColumnSize
            ,SQLSMALLINT  DecimalDigits
            ,SQLPOINTER   ParameterValuePtr
            ,SQLLEN*      StrLen_or_IndPtr)
{
  DriverCall();
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLBindParam(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement =(ODBCStatement*)Obj;
	return statement->SQLBindParameter(ParameterNumber
                                    ,SQL_PARAM_INPUT
                                    ,ValueType
                                    ,ParameterType
                                    ,ColumnSize
                                    ,DecimalDigits
                                    ,ParameterValuePtr
                                    ,0
                                    ,StrLen_or_IndPtr);
}

SQLRETURN SQL_API 
SQLNumParams(SQLHSTMT     StatementHandle
            ,SQLSMALLINT* ParameterCountPtr)
{
  DriverCall();
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLNumParams(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement = (ODBCStatement*)Obj;
	return statement->SQLNumParams(ParameterCountPtr);
}

SQLRETURN SQL_API 
SQLCloseCursor(SQLHSTMT StatementHandle)
{
  DriverCall();
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLCloseCursor(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement *lpConnObj=(ODBCStatement*)Obj;
	return lpConnObj->SQLCloseCursor();
}

SQLRETURN SQL_API 
SQLDisconnect(SQLHDBC ConnectionHandle)
{
  DriverCall();
	ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_DBC)
	{
    WRITELOG("SQLDisconnect(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCConnection *lpConnObj=(ODBCConnection*)Obj;
	return lpConnObj->SQLDisconnect();
}

SQLRETURN SQL_API 
SQLPrepare(SQLHSTMT   StatementHandle
          ,SQLCHAR*   StatementText
          ,SQLINTEGER TextLength)
{
  DriverCall();
  WRITELOG("SQLPrepare(%p,%s,%d)",StatementHandle,StatementText,TextLength);
	ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject() != SQL_HANDLE_STMT)
	{
    WRITELOG("SQLPrepare(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement = (ODBCStatement*)Obj;
  CString sql = MakeDriverString(StatementText,TextLength);
	return statement->SQLPrepare(sql);
}

SQLRETURN SQL_API 
SQLExecDirect(SQLHSTMT   StatementHandle
             ,SQLCHAR*   StatementText
             ,SQLINTEGER TextLength)
{
  DriverCall();
  WRITELOG("SQLExecDirect(%p,%s,%d)",StatementHandle,StatementText,TextLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLExecDirect(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  CString sql = MakeDriverString(StatementText,TextLength);
  SQLRETURN result = statement->SQLPrepare(sql);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return statement->SQLExecute();
  }
  return result;
}

SQLRETURN SQL_API 
SQLExecute(SQLHSTMT StatementHandle)
{
  DriverCall();
  WRITELOG("SQLExecute(%p)",StatementHandle);
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLExecute(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement *lpConnObj=(ODBCStatement*)Obj;
	return lpConnObj->SQLExecute();
}

SQLRETURN SQL_API 
SQLNumResultCols(SQLHSTMT     StatementHandle
                ,SQLSMALLINT* ColumnCount)
{
  DriverCall();
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLNumResultCols(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement *lpConnObj=(ODBCStatement*)Obj;
	return lpConnObj->SQLNumResultCols(ColumnCount);
}

SQLRETURN SQL_API 
SQLDescribeCol(SQLHSTMT     StatementHandle
              ,SQLUSMALLINT ColumnNumber
              ,SQLCHAR*     ColumnName
              ,SQLSMALLINT  BufferLength
              ,SQLSMALLINT* NameLength
              ,SQLSMALLINT* DataType
              ,SQLULEN*     ColumnSize
              ,SQLSMALLINT* DecimalDigits
              ,SQLSMALLINT* Nullable)
{
  DriverCall();
  WRITELOG("SQLDescribeCol(%p,%d,%p,%d,%p,%p,%p,%p,%p)",StatementHandle,ColumnNumber,ColumnName,BufferLength
          ,NameLength,DataType,ColumnSize,DecimalDigits,Nullable);
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject() != SQL_HANDLE_STMT)
	{
    WRITELOG("SQLDescribeCol(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement = (ODBCStatement*)Obj;
	return statement->SQLDescribeCol(ColumnNumber
                                  ,ColumnName
                                  ,BufferLength
		                              ,NameLength
                                  ,DataType
                                  ,ColumnSize
                                  ,DecimalDigits
                                  ,Nullable);
}

SQLRETURN SQL_API 
SQLFetch(SQLHSTMT StatementHandle)
{	
  DriverCall();
  WRITELOG("SQLFetch(%p)",StatementHandle);
	ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject() != SQL_HANDLE_STMT)
	{
    WRITELOG("SQLFetch(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement =(ODBCStatement*)Obj;
	return statement->SQLFetch();
}

SQLRETURN SQL_API
SQLFetchScroll(SQLHSTMT    StatementHandle
              ,SQLSMALLINT FetchOrientation
              ,SQLLEN      FetchOffset)
{
  DriverCall();
  WRITELOG("SQLFetchScroll(%p,%d,%d)",StatementHandle,FetchOrientation,FetchOffset);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLFetchScroll(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement =(ODBCStatement*)Obj;
  return statement->SQLFetchScroll(FetchOrientation,FetchOffset);
}

SQLRETURN SQL_API
SQLExtendedFetch(SQLHSTMT       StatementHandle
                ,SQLUSMALLINT   fFetchType
                ,SQLLEN         irow
                ,SQLULEN*       pcrow
                ,SQLUSMALLINT*  rgfRowStatus)
{
  DriverCall();
  WRITELOG("SQLExtendedFetch(%p,%d,%d,%p,%p)",StatementHandle,fFetchType,irow,pcrow,rgfRowStatus);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLExtendedFetch(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement =(ODBCStatement*)Obj;
  return statement->SQLExtendedFetch(fFetchType,irow,pcrow,rgfRowStatus);
}

SQLRETURN SQL_API
SQLSetPos(SQLHSTMT      StatementHandle
         ,SQLSETPOSIROW irow
         ,SQLUSMALLINT  fOption
         ,SQLUSMALLINT  fLock)
{
  DriverCall();
  WRITELOG("SQLSetPos(%p,%d,%d,%d)",StatementHandle,irow,fOption,fLock);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSetPos(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLSetPos(irow,fOption,fLock);
}

SQLRETURN SQL_API 
SQLGetData(SQLHSTMT     StatementHandle
          ,SQLUSMALLINT ColumnNumber
          ,SQLSMALLINT  TargetType
          ,SQLPOINTER   TargetValue
          ,SQLLEN       BufferLength
          ,SQLLEN*      StrLen_or_Ind)
{
  DriverCall();
  WRITELOG("SQLGetData(%p,%d,%d,%p,%d,%p)",StatementHandle,ColumnNumber,TargetType,TargetValue,BufferLength,StrLen_or_Ind);
	ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLGetData(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement* statement = (ODBCStatement*)Obj;
	return statement->SQLGetData(ColumnNumber
                              ,TargetType
                              ,TargetValue
		                          ,BufferLength
                              ,StrLen_or_Ind);
}

SQLRETURN SQL_API 
SQLMoreResults(SQLHSTMT StatementHandle)
{	
  DriverCall();
  WRITELOG("SQLMoreResults(%p)",StatementHandle);
	ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
	if(Obj->ValidObject()!=SQL_HANDLE_STMT)
	{
    WRITELOG("SQLMoreResults(%p) returns SQL_INVALID_HANDLE",StatementHandle);
		return SQL_INVALID_HANDLE;
	}
	ODBCStatement *p_statement = (ODBCStatement*)Obj;
	return p_statement->SQLMoreResults();
}

SQLRETURN SQL_API 
SQLRowCount(SQLHSTMT  StatementHandle
           ,SQLLEN*   RowCount)
{
  DriverCall();
  WRITELOG("SQLRowCount(%p,%p)",StatementHandle,RowCount);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLRowCount(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLRowCount(RowCount);
}

SQLRETURN SQL_API
SQLColAttribute(SQLHSTMT     StatementHandle
               ,SQLUSMALLINT ColumnNumber
               ,SQLUSMALLINT FieldIdentifier
               ,SQLPOINTER   CharacterAttribute
               ,SQLSMALLINT  BufferLength
               ,SQLSMALLINT* StringLength
               ,SQLPOINTER   NumericAttribute)
{
  DriverCall();
  WRITELOG("SQLColAttribute(%p,%d,%d,%p,%d,%p,%p)"
           ,StatementHandle,ColumnNumber,FieldIdentifier,CharacterAttribute,BufferLength,StringLength,NumericAttribute);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLColAttribute(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLColAttribute(ColumnNumber
                                   ,FieldIdentifier
                                   ,(SQLCHAR*)CharacterAttribute
                                   ,BufferLength
                                   ,StringLength
                                   ,(SQLLEN*)NumericAttribute);
}

// Maps old-style SQLColAttributes (ODBC 2.x) to SQLColAttribute (ODBC 3.x)
// Are these all?
SQLRETURN SQL_API 
SQLColAttributes(SQLHSTMT     StatementHandle
                ,SQLUSMALLINT icol
                ,SQLUSMALLINT fDescType
                ,SQLPOINTER   rgbDesc
                ,SQLSMALLINT  cbDescMax
                ,SQLSMALLINT* pcbDesc
                ,SQLLEN*      pfDesc)
{
  DriverCall();
  WRITELOG("SQLColAttributes(%p,%d,%d,%p,%d,%p,%d)",StatementHandle,icol,fDescType,rgbDesc,cbDescMax,pcbDesc,pfDesc);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLColAttributes(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement =(ODBCStatement*)Obj;

  SQLUSMALLINT FieldIdentifier = 0;
  switch(fDescType)
  {
    case SQL_COLUMN_COUNT:            FieldIdentifier = SQL_DESC_COUNT;             break;
    case SQL_COLUMN_NAME:             FieldIdentifier = SQL_DESC_NAME;              break;
    case SQL_COLUMN_TYPE:             FieldIdentifier = SQL_DESC_TYPE;              break;
    case SQL_COLUMN_LENGTH:           FieldIdentifier = SQL_DESC_LENGTH;            break;
    case SQL_COLUMN_PRECISION:        FieldIdentifier = SQL_DESC_PRECISION;         break;
    case SQL_COLUMN_SCALE:            FieldIdentifier = SQL_DESC_SCALE;             break;
    case SQL_COLUMN_DISPLAY_SIZE:     FieldIdentifier = SQL_DESC_DISPLAY_SIZE;      break;
    case SQL_COLUMN_NULLABLE:         FieldIdentifier = SQL_DESC_NULLABLE;          break;
    case SQL_COLUMN_UNSIGNED:         FieldIdentifier = SQL_DESC_UNSIGNED;          break;
    case SQL_COLUMN_MONEY:            FieldIdentifier = SQL_DESC_FIXED_PREC_SCALE;  break;
    case SQL_COLUMN_UPDATABLE:        FieldIdentifier = SQL_DESC_UPDATABLE;         break;
    case SQL_COLUMN_AUTO_INCREMENT:   FieldIdentifier = SQL_DESC_AUTO_UNIQUE_VALUE; break;
    case SQL_COLUMN_CASE_SENSITIVE:   FieldIdentifier = SQL_DESC_CASE_SENSITIVE;    break;
    case SQL_COLUMN_SEARCHABLE:       FieldIdentifier = SQL_DESC_SEARCHABLE;        break;
    case SQL_COLUMN_TYPE_NAME:        FieldIdentifier = SQL_DESC_TYPE_NAME;         break;
    case SQL_COLUMN_TABLE_NAME:       FieldIdentifier = SQL_DESC_TABLE_NAME;        break;
    case SQL_COLUMN_OWNER_NAME:       FieldIdentifier = SQL_DESC_SCHEMA_NAME;       break;
    case SQL_COLUMN_QUALIFIER_NAME:   FieldIdentifier = SQL_DESC_CATALOG_NAME;      break;
    case SQL_COLUMN_LABEL:            FieldIdentifier = SQL_DESC_LABEL;             break;
    default: WRITELOG("SQLColAttributes(%p). Invalid fDescType translation (%d)",fDescType);
             return SQL_ERROR;
  }
  return statement->SQLColAttribute(icol,FieldIdentifier,(SQLCHAR*)rgbDesc,cbDescMax,pcbDesc,(SQLLEN*)pfDesc);
}
  
SQLRETURN SQL_API
SQLGetFunctions(SQLHDBC       ConnectionHandle
               ,SQLUSMALLINT  FunctionId
               ,SQLUSMALLINT* Supported)
{
  DriverCall();
  WRITELOG("SQLGetFunctions(%p,%d,%p)",ConnectionHandle,FunctionId,Supported);
  ODBCBaseObject* Obj = (ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLGetFunctions(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* lpConnObj = (ODBCConnection*)Obj;
  return lpConnObj->SQLGetFunctions(FunctionId,Supported);
}

SQLRETURN SQL_API
SQLGetDescField(SQLHDESC    DescriptorHandle
               ,SQLSMALLINT RecNumber
               ,SQLSMALLINT FieldIdentifier
               ,SQLPOINTER  Value
               ,SQLINTEGER  BufferLength
               ,SQLINTEGER* StringLength)
{
  DriverCall();
  WRITELOG("SQLGetDescField(%p,%d,%d,%p,%d,%p)",DescriptorHandle,RecNumber,FieldIdentifier,Value,BufferLength,StringLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)DescriptorHandle;
  if(Obj->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLGetDescField(%p) returns SQL_INVALID_HANDLE",DescriptorHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBC_ARD* descriptor = (ODBC_ARD*)Obj;
  return descriptor->SQLGetDescField(RecNumber,FieldIdentifier,Value,BufferLength,StringLength);
}

SQLRETURN SQL_API
SQLSetDescField(SQLHDESC    DescriptorHandle
               ,SQLSMALLINT RecNumber
               ,SQLSMALLINT FieldIdentifier
               ,SQLPOINTER  Value
               ,SQLINTEGER  BufferLength)
{
  DriverCall();
  WRITELOG("SQLSetDescField(%p,%d,%d,%p,%d)",DescriptorHandle,RecNumber,FieldIdentifier,Value,BufferLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)DescriptorHandle;
  if(Obj->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLSetDescField(%p) returns SQL_INVALID_HANDLE",DescriptorHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBC_ARD* descriptor = (ODBC_ARD*)Obj;
  if(descriptor->IsInternal())
  {
    descriptor->GetOCIErrorObject()->AddError("HY016");
    return SQL_ERROR;
  }
  return descriptor->SQLSetDescField(RecNumber,FieldIdentifier,Value,BufferLength);
}

SQLRETURN SQL_API
SQLGetDescRec(SQLHDESC     DescriptorHandle
             ,SQLSMALLINT  RecNumber
             ,SQLCHAR*     Name
             ,SQLSMALLINT  BufferLength
             ,SQLSMALLINT* StringLength
             ,SQLSMALLINT* Type
             ,SQLSMALLINT* SubType
             ,SQLLEN*      Length
             ,SQLSMALLINT* Precision
             ,SQLSMALLINT* Scale
             ,SQLSMALLINT* Nullable)
{
  DriverCall();
  WRITELOG("SQLGetDescRec(%p,%d,%p,%d,%p,%p,%p,%p,%p,%p,%p"
          ,DescriptorHandle,RecNumber,Name,BufferLength,StringLength
          ,Type,SubType,Length,Precision,Scale,Nullable);
  ODBCBaseObject* Obj = (ODBCBaseObject*)DescriptorHandle;
  if(Obj->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLGetDescRec(%p) returns SQL_INVALID_HANDLE",DescriptorHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBC_ARD* descriptor = (ODBC_ARD*)Obj;
  return descriptor->SQLGetDescRec(RecNumber,Name,BufferLength,StringLength,Type,SubType,Length,Precision,Scale,Nullable);
}

SQLRETURN SQL_API
SQLSetDescRec(SQLHDESC    DescriptorHandle
             ,SQLSMALLINT RecNumber
             ,SQLSMALLINT Type
             ,SQLSMALLINT SubType
             ,SQLLEN      Length
             ,SQLSMALLINT Precision
             ,SQLSMALLINT Scale
             ,SQLPOINTER  Data
             ,SQLLEN*     StringLength
             ,SQLLEN*     Indicator)
{
  DriverCall();
  WRITELOG("SQLSetDescRec(%p,%d,%d,%d,%d,%d,%d,%p,%p,%p)"
          ,DescriptorHandle,RecNumber,Type,SubType,Length,Precision
          ,Scale,Data,StringLength,Indicator);
  ODBCBaseObject* Obj = (ODBCBaseObject*)DescriptorHandle;
  if(Obj->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLSetDescRec(%p) returns SQL_INVALID_HANDLE",DescriptorHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBC_ARD* descriptor = (ODBC_ARD*)Obj;
  if(descriptor->IsInternal())
  {
    descriptor->GetOCIErrorObject()->AddError("HY016");
    return SQL_ERROR;
  }
  return descriptor->SQLSetDescRec(RecNumber,Type,SubType,(SQLINTEGER)Length,Precision,Scale,Data,StringLength,Indicator);
}

//////////////////////////////////////////////////////////////////////////
//
// TRANSACTIONS
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API 
SQLEndTran(SQLSMALLINT HandleType
          ,SQLHANDLE   Handle
          ,SQLSMALLINT CompletionType)
{
  DriverCall();
  ODBCBaseObject *Obj=(ODBCBaseObject*)Handle;	
  if(HandleType == SQL_HANDLE_ENV)
  {
    if(Obj->ValidObject() != SQL_HANDLE_ENV)
    {
      WRITELOG("SQLEndTran(%p) returns SQL_INVALID_HANDLE",Handle);
      return SQL_INVALID_HANDLE;
    }
    ODBCEnvironment* environment = (ODBCEnvironment*)Obj;
    return environment->SQLEndTran(CompletionType);
  }
  else if(HandleType == SQL_HANDLE_DBC)
  {
    if(Obj->ValidObject() != SQL_HANDLE_DBC)
    {
      WRITELOG("SQLEndTran(%p) returns SQL_INVALID_HANDLE",Handle);
      return SQL_INVALID_HANDLE;
    }
    ODBCConnection* connection = (ODBCConnection*)Obj;
    return connection->SQLEndTran(CompletionType);
  }
  return SQL_ERROR;
}

SQLRETURN SQL_API
SQLTransact(SQLHENV      EnvironmentHandle
           ,SQLHDBC      ConnectionHandle
           ,SQLUSMALLINT CompletionType)
{
  DriverCall();
  WRITELOG("SQLTransact(%p,%p,%d)",EnvironmentHandle,ConnectionHandle,CompletionType);
  if(ConnectionHandle == NULL)
  {
    WRITELOG("SQLTransact no HDBC. Returns SQL_INVALID_HANDLE");
    return SQL_INVALID_HANDLE;
  }
  ODBCBaseObject* Obj = (ODBCBaseObject*)ConnectionHandle;
  if(Obj->ValidObject() != SQL_HANDLE_DBC)
  {
    WRITELOG("SQLEndTran(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* lpConn = (ODBCConnection*)Obj;
  return lpConn->SQLEndTran(CompletionType);
}

//////////////////////////////////////////////////////////////////////////
//
// TYPE AND OBJECT INFO
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API
SQLGetTypeInfo(SQLHSTMT    StatementHandle
              ,SQLSMALLINT DataType)
{
  DriverCall();
  WRITELOG("SQLGetTypeInfo(%p,%d)",StatementHandle,DataType);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLGetTypeInfo(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLGetTypeInfo(DataType);
}

SQLRETURN SQL_API
SQLTables(SQLHSTMT    StatementHandle
         ,SQLCHAR*    CatalogName
         ,SQLSMALLINT NameLength1
         ,SQLCHAR*    SchemaName
         ,SQLSMALLINT NameLength2
         ,SQLCHAR*    TableName
         ,SQLSMALLINT NameLength3
         ,SQLCHAR*    TableType
         ,SQLSMALLINT NameLength4)
{
  DriverCall();
  WRITELOG("SQLTables(%p,%p,%d,%p,%d,%p,%d,%p,%d)"
          ,StatementHandle
          ,CatalogName,NameLength1
          ,SchemaName ,NameLength2
          ,TableName  ,NameLength3
          ,TableType  ,NameLength4);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLTables(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLTables(CatalogName,NameLength1
                             ,SchemaName ,NameLength2
                             ,TableName  ,NameLength3
                             ,TableType  ,NameLength4);

}

SQLRETURN SQL_API
SQLColumns(SQLHSTMT    StatementHandle
          ,SQLCHAR*    CatalogName
          ,SQLSMALLINT NameLength1
          ,SQLCHAR*    SchemaName
          ,SQLSMALLINT NameLength2
          ,SQLCHAR*    TableName
          ,SQLSMALLINT NameLength3
          ,SQLCHAR*    ColumnName
          ,SQLSMALLINT NameLength4)
{
  DriverCall();
  WRITELOG("SQLColumns(%p,%p,%d,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,CatalogName,NameLength1
          ,SchemaName ,NameLength2
          ,TableName  ,NameLength3
          ,ColumnName ,NameLength4);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLColumns(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLColumns(CatalogName,NameLength1
                              ,SchemaName ,NameLength2
                              ,TableName  ,NameLength3
                              ,ColumnName ,NameLength4);

}

SQLRETURN SQL_API
SQLPrimaryKeys(SQLHSTMT    StatementHandle
              ,SQLCHAR*    szCatalogName
              ,SQLSMALLINT cbCatalogName
              ,SQLCHAR*    szSchemaName
              ,SQLSMALLINT cbSchemaName
              ,SQLCHAR*    szTableName
              ,SQLSMALLINT cbTableName)
{
  DriverCall();
  WRITELOG("SQLPrimaryKeys(%p,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szCatalogName,cbCatalogName
          ,szSchemaName, cbSchemaName
          ,szTableName,  cbTableName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLPrimaryKeys(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLPrimaryKeys(szCatalogName,cbCatalogName
                                  ,szSchemaName, cbSchemaName
                                  ,szTableName,  cbTableName);
}

SQLRETURN SQL_API
SQLForeignKeys(SQLHSTMT    StatementHandle
              ,SQLCHAR*    szPkCatalogName
              ,SQLSMALLINT cbPkCatalogName
              ,SQLCHAR*    szPkSchemaName
              ,SQLSMALLINT cbPkSchemaName
              ,SQLCHAR*    szPkTableName
              ,SQLSMALLINT cbPkTableName
              ,SQLCHAR*    szFkCatalogName
              ,SQLSMALLINT cbFkCatalogName
              ,SQLCHAR*    szFkSchemaName
              ,SQLSMALLINT cbFkSchemaName
              ,SQLCHAR*    szFkTableName
              ,SQLSMALLINT cbFkTableName)
{
  DriverCall();
  WRITELOG("SQLForeignKeys(%p,%p,%d,%p,%d,%p,%d,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szPkCatalogName,cbPkCatalogName
          ,szPkSchemaName, cbPkSchemaName
          ,szPkTableName,  cbPkTableName
          ,szFkCatalogName,cbFkCatalogName
          ,szFkSchemaName, cbFkSchemaName
          ,szFkTableName,  cbFkTableName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLForeignKeys(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLForeignKeys(szPkCatalogName,cbPkCatalogName
                                  ,szPkSchemaName, cbPkSchemaName
                                  ,szPkTableName,  cbPkTableName
                                  ,szFkCatalogName,cbFkCatalogName
                                  ,szFkSchemaName, cbFkSchemaName
                                  ,szFkTableName,  cbFkTableName);
}

SQLRETURN SQL_API
SQLStatistics( SQLHSTMT     StatementHandle
              ,SQLCHAR*     CatalogName
              ,SQLSMALLINT  NameLength1
              ,SQLCHAR*     SchemaName
              ,SQLSMALLINT  NameLength2
              ,SQLCHAR*     TableName
              ,SQLSMALLINT  NameLength3
              ,SQLUSMALLINT Unique
              ,SQLUSMALLINT Reserved)
{
  DriverCall();
  WRITELOG("SQLStatistics(%p,%p,%d,%p,%d,%p,%d,%d,%d)"
    ,StatementHandle
    ,CatalogName,NameLength1
    ,SchemaName, NameLength2
    ,TableName,  NameLength3
    ,Unique,Reserved);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLStatistics(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLStatistics(CatalogName,NameLength1
                                 ,SchemaName, NameLength2
                                 ,TableName,  NameLength3
                                 ,Unique,     Reserved);
}

SQLRETURN SQL_API
SQLSpecialColumns(SQLHSTMT     StatementHandle
                 ,SQLUSMALLINT IdentifierType
                 ,SQLCHAR*     CatalogName
                 ,SQLSMALLINT  NameLength1
                 ,SQLCHAR*     SchemaName
                 ,SQLSMALLINT  NameLength2
                 ,SQLCHAR*     TableName
                 ,SQLSMALLINT  NameLength3
                 ,SQLUSMALLINT Scope
                 ,SQLUSMALLINT Nullable)
{
  DriverCall();
  WRITELOG("SQLSpecialColumns(%p,%d,%p,%d,%p,%d,%p,%d,%d,%d)"
          ,StatementHandle
          ,IdentifierType
          ,CatalogName,NameLength1
          ,SchemaName ,NameLength2
          ,TableName  ,NameLength3
          ,Scope
          ,Nullable);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSpecialColumns(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  // BEWARE AND BEHOLD:
  // The following three parameters are ignored as they have no relevant meaning
  // on the ORACLE platform whatsoever:
  // IdentifierType SQL_BEST_ROWID         -> Oracle has always the pseudo ROWID
  //                SQL_ROWVER             -> Never updated by a transaction
  // Scope          SQL_SCOPE_CURROW       -> Always on that row
  //                SQL_SCOPE_TRANSACTION  -> Valid throughout the whole transaction
  //                SQL_SCOPE_SESSION      -> Valid throughout the session
  // Nullable       SQL_NO_NULLS           -> Never a NULL
  //                SQL_NULLABLE           -> Never nullable
  return statement->SQLSpecialColumns(CatalogName,NameLength1
                                     ,SchemaName ,NameLength2
                                     ,TableName  ,NameLength3);

}

SQLRETURN SQL_API
SQLTablePrivileges(SQLHSTMT    StatementHandle
                  ,SQLCHAR*    szCatalogName
                  ,SQLSMALLINT cbCatalogName
                  ,SQLCHAR*    szSchemaName
                  ,SQLSMALLINT cbSchemaName
                  ,SQLCHAR*    szTableName
                  ,SQLSMALLINT cbTableName)
{
  DriverCall();
  WRITELOG("SQLTablePrivileges(%p,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szCatalogName,cbCatalogName
          ,szSchemaName ,cbSchemaName
          ,szTableName  ,cbTableName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLTablePrivileges(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLTablePrivileges(szCatalogName,cbCatalogName
                                      ,szSchemaName ,cbSchemaName
                                      ,szTableName  ,cbTableName);

}

SQLRETURN SQL_API
SQLColumnPrivileges(SQLHSTMT    StatementHandle
                   ,SQLCHAR*    /*szCatalogName*/
                   ,SQLSMALLINT cbCatalogName
                   ,SQLCHAR*    szSchemaName
                   ,SQLSMALLINT cbSchemaName
                   ,SQLCHAR*    szTableName
                   ,SQLSMALLINT cbTableName
                   ,SQLCHAR*    szColumnName
                   ,SQLSMALLINT cbColumnName)
{
  DriverCall();
  WRITELOG("SQLColumnPrivileges(%p,%p,%d,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szColumnName,cbCatalogName
          ,szSchemaName,cbSchemaName
          ,szTableName ,cbTableName
          ,szColumnName,cbColumnName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLColumnPrivileges(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLColumnPrivileges(szColumnName,cbCatalogName
                                       ,szSchemaName,cbSchemaName
                                       ,szTableName ,cbTableName
                                       ,szColumnName,cbColumnName);
}

SQLRETURN SQL_API
SQLProcedures(SQLHSTMT    StatementHandle
             ,SQLCHAR*    szCatalogName
             ,SQLSMALLINT cbCatalogName
             ,SQLCHAR*    szSchemaName
             ,SQLSMALLINT cbSchemaName
             ,SQLCHAR*    szProcName
             ,SQLSMALLINT cbProcName)
{
  DriverCall();
  WRITELOG("SQLProcedures(%p,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szCatalogName,cbCatalogName
          ,szSchemaName, cbSchemaName
          ,szProcName,   cbProcName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLProcedures(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLProcedures(szCatalogName,cbCatalogName
                                 ,szSchemaName, cbSchemaName
                                 ,szProcName,   cbProcName);
}

SQLRETURN SQL_API
SQLProcedureColumns(SQLHSTMT    StatementHandle
                   ,SQLCHAR*    szCatalogName
                   ,SQLSMALLINT cbCatalogName
                   ,SQLCHAR*    szSchemaName
                   ,SQLSMALLINT cbSchemaName
                   ,SQLCHAR*    szProcName
                   ,SQLSMALLINT cbProcName
                   ,SQLCHAR*    szColumnName
                   ,SQLSMALLINT cbColumnName)
{
  DriverCall();
  WRITELOG("SQLProcedureColumns(%p,%p,%d,%p,%d,%p,%d,%p,%d)"
          ,StatementHandle
          ,szCatalogName,cbCatalogName
          ,szSchemaName, cbSchemaName
          ,szProcName,   cbProcName
          ,szColumnName, cbColumnName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLProcedureColumns(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLProcedureColumns(szCatalogName,cbCatalogName
                                        ,szSchemaName, cbSchemaName
                                        ,szProcName,   cbProcName
                                        ,szColumnName, cbColumnName);

}

//////////////////////////////////////////////////////////////////////////
//
// ERROR HANDLING
// SQLError
// SQLGetDiagRec
// SQLGetDiagField
//
//////////////////////////////////////////////////////////////////////////

SQLRETURN SQL_API
SQLError(SQLHENV      EnvironmentHandle
        ,SQLHDBC      ConnectionHandle
        ,SQLHSTMT     StatementHandle
        ,SQLCHAR*     Sqlstate
        ,SQLINTEGER*  NativeError
        ,SQLCHAR*     MessageText
        ,SQLSMALLINT  BufferLength
        ,SQLSMALLINT* TextLength)
{
  DriverCall();
  WRITELOG("SQLError(%p,%p,%p,%p,%p,%p,%d,%p)"
          ,EnvironmentHandle
          ,ConnectionHandle
          ,StatementHandle
          ,Sqlstate
          ,NativeError
          ,MessageText  
          ,BufferLength
          ,TextLength);

  ODBCBaseObject* Obj = NULL;
  if(StatementHandle)
  {
    Obj = (ODBCBaseObject*) StatementHandle;
  }
  else if(ConnectionHandle)
  {
    Obj = (ODBCBaseObject*) ConnectionHandle;
  }
  else if(EnvironmentHandle)
  {
    Obj = (ODBCBaseObject*) EnvironmentHandle;
  }
  if(Obj && Obj->GetOCIErrorObject()->DiagRecords())
  {
    // Gets the NEXT error record number
    int record = Obj->GetOCIErrorObject()->GetErrorRecord();
    if(record == -1)
    {
      // Already gotten by SQLError
      return SQL_NO_DATA_FOUND;
    }
    Obj->GetOCIErrorObject()->SQLGetDiagRec((SQLSMALLINT)record + 1,Sqlstate,NativeError,MessageText,BufferLength,TextLength);
    return SQL_SUCCESS;
  }
  return SQL_NO_DATA_FOUND;
}

SQLRETURN SQL_API 
SQLGetDiagRec(SQLSMALLINT  HandleType
             ,SQLHANDLE    Handle
             ,SQLSMALLINT  RecNumber
             ,SQLCHAR*     Sqlstate
             ,SQLINTEGER*  NativeError
             ,SQLCHAR*     MessageText
             ,SQLSMALLINT  BufferLength
             ,SQLSMALLINT* TextLength)
{
  DriverCall();
  WRITELOG("SQLGetDiagRec(%s,%p,%d,%p,%p,%p,%d,%p)",GetHandleTypeDescription(HandleType),Handle,RecNumber,Sqlstate,NativeError,MessageText,BufferLength,TextLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)Handle;	
  if(Obj == NULL)
  {
    // Special case for debugging of SQLAllocHandle
    return ODBCDriver.GetOCIErrorObject()->SQLGetDiagRec(RecNumber,Sqlstate,NativeError,MessageText,BufferLength,TextLength);    
  }
  if(Obj->ValidObject() != HandleType)
  {
    WRITELOG("SQLGetDiagRec(%s,%p) returns SQL_INVALID_HANDLE",GetHandleTypeDescription(HandleType),Handle);
    return SQL_INVALID_HANDLE;
  }
  return Obj->GetOCIErrorObject()->SQLGetDiagRec(RecNumber,Sqlstate,NativeError,MessageText,BufferLength,TextLength);    
}

SQLRETURN SQL_API
SQLGetDiagField(SQLSMALLINT   HandleType
               ,SQLHANDLE     Handle
               ,SQLSMALLINT   RecNumber
               ,SQLSMALLINT   DiagIdentifier
               ,SQLPOINTER    DiagInfo
               ,SQLSMALLINT   BufferLength
               ,SQLSMALLINT*  StringLength)
{
  DriverCall();
  WRITELOG("SQLGetDiagField(%d,%p,%d,%d,%p,%d,%p)",HandleType,Handle,RecNumber,DiagIdentifier,DiagInfo,BufferLength,StringLength);
  ODBCBaseObject* obj = (ODBCBaseObject*) Handle;
  if(obj->ValidObject() != HandleType)
  {
    WRITELOG("SQLGetDiagField(%p) returns SQL_INVALID_HANDLE",Handle);
    return SQL_INVALID_HANDLE;
  }
  return obj->GetOCIErrorObject()->SQLGetDiagField(HandleType,obj,RecNumber,DiagIdentifier,DiagInfo,BufferLength,StringLength);
}

SQLRETURN SQL_API
SQLSetCursorName(SQLHSTMT    StatementHandle
                ,SQLCHAR*    CursorName
                ,SQLSMALLINT NameLength)
{
  DriverCall();
  WRITELOG("SQLSetCursorName(%p,%p,%d)",StatementHandle,CursorName,NameLength);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSetCursorName(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  CString cursor = MakeDriverString(CursorName,NameLength);
  return statement->SQLSetCursorName((SQLCHAR*)cursor.GetString());
}

SQLRETURN SQL_API
SQLGetCursorName(SQLHSTMT     StatementHandle
                ,SQLCHAR*     CursorName
                ,SQLSMALLINT  BufferLength
                ,SQLSMALLINT* NameLength)
{
  DriverCall();
  WRITELOG("SQLGetCursorName(%p,%p,%d,%p)",StatementHandle,CursorName,BufferLength,NameLength);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLGetCursorName(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  return statement->SQLGetCursorName(CursorName,BufferLength,NameLength);
}

SQLRETURN SQL_API
SQLParamOptions(SQLHSTMT hstmt, SQLULEN crow, SQLULEN* pirow)
{
  DriverCall();
  WRITELOG("SQLParamOptions(%p,%d,%p)",hstmt,crow,pirow);
  ODBCBaseObject* obj = (ODBCBaseObject*)hstmt;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLParamOptions(%p) returns SQL_INVALID_HANDLE",hstmt);
    return SQL_INVALID_HANDLE;
  }
  SQLRETURN res1 = SQL_ERROR;
  SQLRETURN res2 = SQL_ERROR;
  ODBCStatement* statement = (ODBCStatement*)obj;
  res1 = statement->SQLSetStmtAttr(SQL_ATTR_PARAMSET_SIZE,       (SQLPOINTER)crow, 0);
  res2 = statement->SQLSetStmtAttr(SQL_ATTR_PARAMS_PROCESSED_PTR,(SQLPOINTER)pirow,0);
  if(res1 != SQL_SUCCESS || res2 != SQL_SUCCESS)
  {
    return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

SQLRETURN SQL_API
SQLCopyDesc(SQLHDESC SourceDescHandle
           ,SQLHDESC TargetDescHandle)
{
  DriverCall();
  WRITELOG("SQLCopyDesc(%p,%p)",SourceDescHandle,TargetDescHandle);
  ODBCBaseObject* obj1 = (ODBCBaseObject*)SourceDescHandle;
  ODBCBaseObject* obj2 = (ODBCBaseObject*)TargetDescHandle;
  if(obj1->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLCopyDesc(%p) source SQL_INVALID_HANDLE",SourceDescHandle);
    return SQL_INVALID_HANDLE;
  }
  if(obj2->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLCopyDesc(%p) target SQL_INVALID_HANDLE",TargetDescHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCDescriptor* source = (ODBCDescriptor*)obj1;
  ODBCDescriptor* target = (ODBCDescriptor*)obj2;
  if(target->IsInternal() && target->IsRowDescriptor())
  {
    // Errors are returned in the error descriptor of the target
    // Cannot modify an internal row descriptor (IRD)
    target->GetOCIErrorObject()->AddError("HY016");
    return SQL_ERROR;
  }
  if(source->IsInternal()      != target->IsInternal()    ||
     source->IsRowDescriptor() != target->IsRowDescriptor())
  {
    // Inconsistent descriptors
    target->GetOCIErrorObject()->AddError("HY021");
    return SQL_ERROR;
  }
  if(source->IsInternal())
  {
    // Both now must be internal
    ODBC_IRD* ird_source = (ODBC_IRD*)source;
    ODBC_IRD* ird_target = (ODBC_IRD*)target;
    return ird_target->CopyFrom(ird_source);
  }
  else
  {
    // Both now must be external
    ODBC_ARD* ard_source = (ODBC_ARD*)source;
    ODBC_ARD* ard_target = (ODBC_ARD*)target;
    return ard_target->CopyFrom(ard_source);
  }
}

SQLRETURN SQL_API
SQLDescribeParam(SQLHSTMT     StatementHandle
                ,SQLUSMALLINT ipar
                ,SQLSMALLINT* pfSqlType
                ,SQLULEN*     pcbParamDef
                ,SQLSMALLINT* pibScale
                ,SQLSMALLINT* pfNullable)
{
  DriverCall();
  WRITELOG("SQLDescribeParam(%p,%d,%p,%p,%p,%p)",StatementHandle,ipar,pfSqlType,pcbParamDef,pibScale,pfNullable);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLDescribeParam(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  return statement->SQLDescribeParam(ipar,pfSqlType,pcbParamDef,pibScale,pfNullable);
}

SQLRETURN SQL_API
SQLCancel(SQLHSTMT StatementHandle)
{
  DriverCall();
  WRITELOG("SQLCancel(%p)",StatementHandle);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLCancel(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  return statement->SQLCancel();
}

SQLRETURN SQL_API
SQLNativeSql(SQLHDBC     ConnectionHandle
            ,SQLCHAR*    InStatementText
            ,SQLINTEGER  TextLength1
            ,SQLCHAR*    OutStatementText
            ,SQLINTEGER  BufferLength
            ,SQLINTEGER* TextLength2)
{
  DriverCall();
  WRITELOG("SQLNativeSQL(%p,%p,%d,%p,%d,%p)"
          ,ConnectionHandle
          ,InStatementText, TextLength1
          ,OutStatementText,BufferLength,TextLength2);
  ODBCBaseObject* obj = (ODBCBaseObject*)ConnectionHandle;
  if(obj->ValidObject() != SQL_HANDLE_DBC)
  {
    WRITELOG("SQLNativeSql(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)obj;
  return connection->SQLNativeSql(InStatementText,TextLength1,OutStatementText,BufferLength,TextLength2);
}

SQLRETURN SQL_API
SQLSetParam(SQLHSTMT     StatementHandle
           ,SQLUSMALLINT ParameterNumber
           ,SQLSMALLINT  ValueType
           ,SQLSMALLINT  ParameterType
           ,SQLULEN      LengthPrecision
           ,SQLSMALLINT  ParameterScale
           ,SQLPOINTER   ParameterValue
           ,SQLLEN*      StrLen_or_Ind)
{
  DriverCall();
  WRITELOG("SQLSetParam(%p,%d,%d,%d,%d,%d,%d,%p)",StatementHandle,ParameterNumber,ValueType,ParameterType,LengthPrecision,ParameterScale,ParameterValue,StrLen_or_Ind);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSetParam(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  int length = 0;
  switch(ValueType)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_WCHAR:
    case SQL_WVARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_WLONGVARCHAR:  length = (int)strlen((char*)ParameterValue);
  }
  return statement->SQLBindParameter(ParameterNumber
                                    ,SQL_PARAM_INPUT   // Old style 1.0 is always param input
                                    ,ValueType
                                    ,ParameterType
                                    ,LengthPrecision
                                    ,ParameterScale
                                    ,ParameterValue
                                    ,length
                                    ,StrLen_or_Ind);
}

SQLRETURN SQL_API
SQLSetScrollOptions(SQLHSTMT     StatementHandle
                   ,SQLUSMALLINT fConcurrency
                   ,SQLLEN       crowKeyset
                   ,SQLUSMALLINT crowRowset)
{
  DriverCall();
  WRITELOG("SQLSetScrollOptions(%p,%d,%d,%d)",StatementHandle,fConcurrency,crowKeyset,crowRowset);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSetScrollOptions(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  return statement->SQLSetScrollOptions(fConcurrency,crowKeyset,crowRowset);
}

SQLRETURN SQL_API
SQLParamData(SQLHSTMT    StatementHandle
            ,SQLPOINTER* Value)
{
  DriverCall();
  WRITELOG("SQLParamData(%p,%p)",StatementHandle,Value);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLParamData(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  return statement->SQLParamData(Value);
}

SQLRETURN SQL_API
SQLPutData(SQLHSTMT   StatementHandle
          ,SQLPOINTER Data
          ,SQLLEN     StrLen_or_Ind)
{
  DriverCall();
  WRITELOG("SQLPutData(%p,%p,%d)",StatementHandle,Data,StrLen_or_Ind);
  ODBCBaseObject* obj = (ODBCBaseObject*) StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLPutData(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*) obj;
  return statement->SQLPutData(Data,StrLen_or_Ind);
}
