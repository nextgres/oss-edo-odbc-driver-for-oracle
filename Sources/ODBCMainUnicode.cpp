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
// Contents:  ODBCMain Module for UNICODE
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
#include <oci.h>
#include "Unicode.h"
#include <intrin.h>

#ifdef _DEBUG
void UnicodeCall()
{
  // Use this to set a breakpoint for all interface calls
  __nop();
}
#else
#define UnicodeCall()
#endif

SQLRETURN SQL_API 
SQLDriverConnectW(SQLHDBC      hdbc
                 ,SQLHWND      hwnd
                 ,SQLWCHAR*    szConnStrIn
                 ,SQLSMALLINT  cbConnStrIn
                 ,SQLWCHAR*    szConnStrOut
                 ,SQLSMALLINT  cbConnStrOutMax
                 ,SQLSMALLINT* pcbConnStrOut
                 ,SQLUSMALLINT fDriverCompletion)
{
  UnicodeCall();
  // Get static module state of MFC in case we do a connection dialog
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  WRITELOG("SQLDriverConnectW(%p,%p,%p,%d,%p,%d,%p,%d)",hdbc,hwnd,szConnStrIn,cbConnStrIn
           ,szConnStrOut,cbConnStrOutMax,fDriverCompletion);
  ODBCBaseObject* Obj = (ODBCBaseObject*)hdbc;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLDriverConnectW(%p) returns SQL_INVALID_HANDLE",hdbc);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;

  ConvertingString<> connectString(szConnStrIn,cbConnStrIn);
  ConvertingString<> resultString(cbConnStrOutMax,szConnStrOut,pcbConnStrOut,true);

  return connection->SQLDriverConnect(hwnd
                                     ,CString(connectString)
                                     ,resultString
                                     ,cbConnStrOutMax
                                     ,pcbConnStrOut
                                     ,fDriverCompletion);
}

SQLRETURN SQL_API
SQLBrowseConnectW(SQLHDBC      hdbc
                 ,SQLWCHAR*    szConnStrIn
                 ,SQLSMALLINT  cbConnStrIn
                 ,SQLWCHAR*    szConnStrOut
                 ,SQLSMALLINT  cbConnStrOutMax
                 ,SQLSMALLINT* pcbConnStrOut)
{
  UnicodeCall();
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  WRITELOG("SQLBrowseConnectW(%p,%p,%d,%p,%d,%p)",hdbc,szConnStrIn,cbConnStrIn,szConnStrOut,cbConnStrOutMax,pcbConnStrOut);
  ODBCBaseObject* obj = (ODBCBaseObject*)hdbc;
  if(obj->ValidObject() != SQL_HANDLE_DBC)
  {
    WRITELOG("SQLBrowseConnectW(%p) returns SQL_INVALID_HANDLE",hdbc);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)obj;
  ConvertingString<> connectString(szConnStrIn,cbConnStrIn);
  ConvertingString<> resultString (cbConnStrOutMax,szConnStrOut,pcbConnStrOut,true);
  return connection->SQLBrowseConnect(CString(connectString),resultString,resultString,pcbConnStrOut);
}

SQLRETURN SQL_API 
SQLPrepareW(SQLHSTMT   StatementHandle
           ,SQLWCHAR*  StatementText
           ,SQLINTEGER TextLength)
{
  UnicodeCall();
  WRITELOG("SQLPrepareW(%p,%s,%d)",StatementHandle,StatementText,TextLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLPrepareW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> sql(StatementText,TextLength);
  return statement->SQLPrepare(CString(sql));
}

SQLRETURN SQL_API 
SQLExecDirectW(SQLHSTMT   StatementHandle
              ,SQLWCHAR*  StatementText
              ,SQLINTEGER TextLength)
{
  UnicodeCall();
  WRITELOG("SQLExecDirectW(%p,%s,%d)",StatementHandle,StatementText,TextLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLExecDirectW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> sql(StatementText,TextLength);
  SQLRETURN result = statement->SQLPrepare(CString(sql));
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return statement->SQLExecute();
  }
  return result;
}

SQLRETURN SQL_API 
SQLColAttributesW(SQLHSTMT     StatementHandle
                 ,SQLUSMALLINT columnNumber
                 ,SQLUSMALLINT fieldIdentifier
                 ,SQLPOINTER   characterAttribute
                 ,SQLSMALLINT  bufferLength
                 ,SQLSMALLINT* stringLength
                 ,SQLLEN*      numericAttribute)
{
  UnicodeCall();
  WRITELOG("SQLColAttributesW(%p,%d,%d,%p,%d,%p,%d)",StatementHandle,columnNumber,fieldIdentifier
          ,characterAttribute,bufferLength,stringLength,numericAttribute);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLColAttributesW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement =(ODBCStatement*)Obj;

  SQLUSMALLINT FieldIdentifier = 0;
  switch(fieldIdentifier)
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
    default: WRITELOG("SQLColAttributes(%p). Invalid fDescType translation (%d)",fieldIdentifier);
             return SQL_ERROR;
  }
  switch (FieldIdentifier )
  {
    case SQL_DESC_LABEL:
    case SQL_DESC_BASE_COLUMN_NAME:
    case SQL_COLUMN_NAME:
    case SQL_DESC_NAME:
    case SQL_DESC_TYPE_NAME:
    case SQL_DESC_BASE_TABLE_NAME:
    case SQL_DESC_TABLE_NAME:
    case SQL_DESC_SCHEMA_NAME:
    case SQL_DESC_CATALOG_NAME:

    if(bufferLength > 0)
    {
      ConvertingString<> CharacterAttribute(bufferLength,(SQLWCHAR *)characterAttribute,stringLength);
      return statement->SQLColAttribute(columnNumber
                                       ,FieldIdentifier
                                       ,(SQLCHAR*)CharacterAttribute
                                       ,CharacterAttribute // Bufferlength
                                       ,stringLength
                                       ,numericAttribute);
    }
  }
  return statement->SQLColAttribute(columnNumber
                                   ,FieldIdentifier
                                   ,(SQLCHAR*)characterAttribute
                                   ,bufferLength
                                   ,stringLength
                                   ,numericAttribute);
}

SQLRETURN SQL_API 
SQLConnectW(SQLHDBC     ConnectionHandle
           ,SQLWCHAR*   ServerName
           ,SQLSMALLINT NameLength1
           ,SQLWCHAR*   UserName
           ,SQLSMALLINT NameLength2
           ,SQLWCHAR*   Authentication
           ,SQLSMALLINT NameLength3)
{
  UnicodeCall();
  WRITELOG("SQLConnectW(%p,%p,%d,%p,%d,%p,%d)",ConnectionHandle
          ,ServerName,NameLength1
          ,UserName,  NameLength2
          ,Authentication,NameLength3);
  ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLConnectW(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;
  ConvertingString<> server(ServerName,NameLength1);
  ConvertingString<> user  (UserName,  NameLength2);
  ConvertingString<> pwd   (Authentication,NameLength3);
  return connection->SQLConnect(CString(server),CString(user),CString(pwd));
}

SQLRETURN SQL_API 
SQLDescribeColW(SQLHSTMT     StatementHandle
               ,SQLUSMALLINT ColumnNumber
               ,SQLWCHAR*    ColumnName
               ,SQLSMALLINT  BufferLength
               ,SQLSMALLINT* NameLength
               ,SQLSMALLINT* DataType
               ,SQLULEN*     ColumnSize
               ,SQLSMALLINT* DecimalDigits
               ,SQLSMALLINT* Nullable)
{
  UnicodeCall();
  WRITELOG("SQLDescribeColW(%p,%d,%p,%d,%p,%p,%p,%p,%p)",StatementHandle,ColumnNumber,ColumnName,BufferLength
          ,NameLength,DataType,ColumnSize,DecimalDigits,Nullable);
  ODBCBaseObject *Obj=(ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLDescribeColW%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> columnName(BufferLength,ColumnName,NameLength,false);
  return statement->SQLDescribeCol(ColumnNumber
                                  ,columnName
                                  ,columnName // BufferLength
                                  ,NameLength
                                  ,DataType
                                  ,ColumnSize
                                  ,DecimalDigits
                                  ,Nullable);
}

SQLRETURN SQL_API
SQLErrorW(SQLHENV      EnvironmentHandle
         ,SQLHDBC      ConnectionHandle
         ,SQLHSTMT     StatementHandle
         ,SQLWCHAR*    Sqlstate
         ,SQLINTEGER*  NativeError
         ,SQLWCHAR*    MessageText
         ,SQLSMALLINT  BufferLength
         ,SQLSMALLINT* TextLength)
{
  UnicodeCall();
  WRITELOG("SQLErrorW(%p,%p,%p,%p,%p,%p,%d,%p)"
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
    ConvertingString<> State (12,Sqlstate);
    ConvertingString<> Buffer(BufferLength,MessageText,TextLength,false);

    Obj->GetOCIErrorObject()->SQLGetDiagRec((SQLSMALLINT)record + 1,State,NativeError,Buffer,Buffer,TextLength);
    return SQL_SUCCESS;
  }
  return SQL_NO_DATA_FOUND;
}

SQLRETURN SQL_API
SQLGetCursorNameW(SQLHSTMT     StatementHandle
                 ,SQLWCHAR*    CursorName
                 ,SQLSMALLINT  BufferLength
                 ,SQLSMALLINT* NameLength)
{
  UnicodeCall();
  WRITELOG("SQLGetCursorNameW(%p,%p,%d,%p)",StatementHandle,CursorName,BufferLength,NameLength);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLGetCursorNameW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  ConvertingString<> cursorName(BufferLength,CursorName,NameLength,false);
  return statement->SQLGetCursorName(cursorName,cursorName,NameLength);
}

SQLRETURN SQL_API
SQLSetCursorNameW(SQLHSTMT    StatementHandle
                 ,SQLWCHAR*   CursorName
                 ,SQLSMALLINT NameLength)
{
  UnicodeCall();
  WRITELOG("SQLSetCursorNameW(%p,%p,%d)",StatementHandle,CursorName,NameLength);
  ODBCBaseObject* obj = (ODBCBaseObject*)StatementHandle;
  if(obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSetCursorNameW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)obj;
  ConvertingString<> cursor(CursorName,NameLength);
  return statement->SQLSetCursorName(cursor);
}

SQLRETURN SQL_API
SQLColumnsW(SQLHSTMT    StatementHandle
           ,SQLWCHAR*   CatalogName
           ,SQLSMALLINT NameLength1
           ,SQLWCHAR*   SchemaName
           ,SQLSMALLINT NameLength2
           ,SQLWCHAR*   TableName
           ,SQLSMALLINT NameLength3
           ,SQLWCHAR*   ColumnName
           ,SQLSMALLINT NameLength4)
{
  UnicodeCall();
  WRITELOG("SQLColumnsW(%p,%p,%d,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,CatalogName,NameLength1
          ,SchemaName ,NameLength2
          ,TableName  ,NameLength3
          ,ColumnName ,NameLength4);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLColumnsW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement =(ODBCStatement*)Obj;
  ConvertingString<> catalog(CatalogName,NameLength1);
  ConvertingString<> schema (SchemaName, NameLength2);
  ConvertingString<> table  (TableName,  NameLength3);
  ConvertingString<> column (ColumnName, NameLength4);

  return statement->SQLColumns(catalog,catalog,schema,schema,table,table,column,column);
}

SQLRETURN SQL_API
SQLGetConnectOptionW(SQLHDBC      ConnectionHandle
                    ,SQLUSMALLINT Option
                    ,SQLPOINTER   Value)
{
  UnicodeCall();
  WRITELOG("SQLGetConnectOptionW(%p,%d,%p)",ConnectionHandle,Option,Value);
  ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLGetConnectAttrW(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;
  int bufferLength = SQL_MAX_OPTION_STRING_LENGTH;
  switch(Option)
  {
    case SQL_ATTR_CURRENT_CATALOG:
    case SQL_ATTR_TRACEFILE:
    case SQL_ATTR_TRANSLATE_LIB:   ConvertingString<> ConnStrOut(bufferLength,(SQLWCHAR *)Value);
                                   return connection->SQLGetConnectOption(Option,ConnStrOut);
  }
  return connection->SQLGetConnectOption(Option,Value);
}

SQLRETURN SQL_API 
SQLGetInfoW(SQLHDBC      ConnectionHandle
           ,SQLUSMALLINT InfoType
           ,SQLPOINTER   InfoValue
           ,SQLSMALLINT  BufferLength
           ,SQLSMALLINT* StringLength)
{
  UnicodeCall();
  WRITELOG("SQLGetInfoW(%p,%d,%p,%d,%p)",ConnectionHandle,InfoType,InfoValue,BufferLength,StringLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLGetInfoW(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;

  switch(InfoType)
  {
    case SQL_ACCESSIBLE_PROCEDURES:
    case SQL_ACCESSIBLE_TABLES:
    case SQL_CATALOG_NAME:
    case SQL_CATALOG_NAME_SEPARATOR:
    case SQL_CATALOG_TERM:
    case SQL_COLLATION_SEQ:
    case SQL_COLUMN_ALIAS:
    case SQL_DATA_SOURCE_NAME:
    case SQL_DATA_SOURCE_READ_ONLY:
    case SQL_DATABASE_NAME:
    case SQL_DBMS_NAME:
    case SQL_DBMS_VER:
    case SQL_DESCRIBE_PARAMETER:
    case SQL_DM_VER:
    case SQL_DRIVER_NAME:
    case SQL_DRIVER_ODBC_VER:
    case SQL_DRIVER_VER:
    case SQL_EXPRESSIONS_IN_ORDERBY:
    case SQL_IDENTIFIER_QUOTE_CHAR:
    case SQL_INTEGRITY:
    case SQL_KEYWORDS:
    case SQL_LIKE_ESCAPE_CLAUSE:
    case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
    case SQL_MULT_RESULT_SETS:
    case SQL_MULTIPLE_ACTIVE_TXN:
    case SQL_NEED_LONG_DATA_LEN:
    case SQL_ODBC_VER:
    case SQL_ORDER_BY_COLUMNS_IN_SELECT:
    case SQL_OUTER_JOINS:
    case SQL_PROCEDURE_TERM:
    case SQL_PROCEDURES:
    case SQL_ROW_UPDATES:
    case SQL_SCHEMA_TERM:
    case SQL_SEARCH_PATTERN_ESCAPE:
    case SQL_SERVER_NAME:
    case SQL_SPECIAL_CHARACTERS:
    case SQL_TABLE_TERM:
    case SQL_USER_NAME:
    case SQL_XOPEN_CLI_YEAR:

    if(BufferLength > 0)
    {
      ConvertingString<> infoValue( BufferLength, (SQLWCHAR *)InfoValue, StringLength );
      return connection->SQLGetInfo(InfoType
                                   ,(SQLPOINTER)(SQLCHAR*)infoValue
                                   ,infoValue
                                   ,StringLength);
    }
    else
    {
      SQLRETURN ret = connection->SQLGetInfo(InfoType,InfoValue,BufferLength,StringLength);
      *StringLength *= 2;
      return ret;
    }
  }
  return connection->SQLGetInfo(InfoType,InfoValue,BufferLength,StringLength);
}

SQLRETURN SQL_API
SQLSetConnectOptionW(SQLHDBC      ConnectionHandle
                    ,SQLUSMALLINT Option
                    ,SQLULEN      Value)
{
  UnicodeCall();
  WRITELOG("SQLSetConnectOptionW(%p,%d,%p)",ConnectionHandle,Option,Value);
  ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLSetConnectOptionW(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;
  switch(Option)
  {
    case SQL_ATTR_CURRENT_CATALOG:
    case SQL_ATTR_TRACEFILE:
    case SQL_ATTR_TRANSLATE_LIB:
    {
      SQLINTEGER bufferLength = SQL_MAX_OPTION_STRING_LENGTH;
      ConvertingString<> valstring((SQLWCHAR *)Value,bufferLength);
      return connection->SQLSetConnectAttr(Option,(SQLPOINTER)(SQLCHAR*)valstring,valstring);
    }
  }
  return connection->SQLSetConnectOption(Option,Value);
}

SQLRETURN SQL_API
SQLSpecialColumnsW(SQLHSTMT     StatementHandle
                  ,SQLUSMALLINT IdentifierType
                  ,SQLWCHAR*    CatalogName
                  ,SQLSMALLINT  NameLength1
                  ,SQLWCHAR*    SchemaName
                  ,SQLSMALLINT  NameLength2
                  ,SQLWCHAR*    TableName
                  ,SQLSMALLINT  NameLength3
                  ,SQLUSMALLINT Scope
                  ,SQLUSMALLINT Nullable)
{
  UnicodeCall();
  WRITELOG("SQLSpecialColumnsW(%p,%d,%p,%d,%p,%d,%p,%d,%d,%d)"
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
    WRITELOG("SQLSpecialColumnsW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;

  ConvertingString<> catalogName(CatalogName,NameLength1);
  ConvertingString<> schemaName (SchemaName, NameLength2);
  ConvertingString<> tableName  (TableName,  NameLength3);

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
  return statement->SQLSpecialColumns(catalogName, catalogName
                                     ,schemaName,  schemaName
                                     ,tableName,   tableName);
}

SQLRETURN SQL_API
SQLStatisticsW(SQLHSTMT     StatementHandle
              ,SQLWCHAR*    CatalogName
              ,SQLSMALLINT  NameLength1
              ,SQLWCHAR*    SchemaName
              ,SQLSMALLINT  NameLength2
              ,SQLWCHAR*    TableName
              ,SQLSMALLINT  NameLength3
              ,SQLUSMALLINT Unique
              ,SQLUSMALLINT Reserved)
{
  UnicodeCall();
  WRITELOG("SQLStatisticsW(%p,%p,%d,%p,%d,%p,%d,%d,%d)"
          ,StatementHandle
          ,CatalogName,NameLength1
          ,SchemaName, NameLength2
          ,TableName,  NameLength3
          ,Unique,Reserved);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLStatisticsW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> catalog(CatalogName,NameLength1);
  ConvertingString<> schema (SchemaName, NameLength2);
  ConvertingString<> table  (TableName,  NameLength3);
  return statement->SQLStatistics(catalog,catalog,
                                  schema, schema,
                                  table,  table,
                                  Unique, Reserved);
}

SQLRETURN SQL_API
SQLTablesW(SQLHSTMT    StatementHandle
          ,SQLWCHAR*   CatalogName
          ,SQLSMALLINT NameLength1
          ,SQLWCHAR*   SchemaName
          ,SQLSMALLINT NameLength2
          ,SQLWCHAR*   TableName
          ,SQLSMALLINT NameLength3
          ,SQLWCHAR*   TableType
          ,SQLSMALLINT NameLength4)
{
  UnicodeCall();
  WRITELOG("SQLTablesW(%p,%p,%d,%p,%d,%p,%d,%p,%d)"
          ,StatementHandle
          ,CatalogName,NameLength1
          ,SchemaName ,NameLength2
          ,TableName  ,NameLength3
          ,TableType  ,NameLength4);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLTablesW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> catalog(CatalogName,NameLength1);
  ConvertingString<> schema (SchemaName, NameLength2);
  ConvertingString<> table  (TableName,  NameLength3);
  ConvertingString<> type   (TableType,  NameLength4);
  return statement->SQLTables(catalog,catalog
                             ,schema, schema
                             ,table,  table
                             ,type,   type);
}

SQLRETURN SQL_API
SQLForeignKeysW(SQLHSTMT    StatementHandle
               ,SQLWCHAR*   szPkCatalogName
               ,SQLSMALLINT cbPkCatalogName
               ,SQLWCHAR*   szPkSchemaName
               ,SQLSMALLINT cbPkSchemaName
               ,SQLWCHAR*   szPkTableName
               ,SQLSMALLINT cbPkTableName
               ,SQLWCHAR*   szFkCatalogName
               ,SQLSMALLINT cbFkCatalogName
               ,SQLWCHAR*   szFkSchemaName
               ,SQLSMALLINT cbFkSchemaName
               ,SQLWCHAR*   szFkTableName
               ,SQLSMALLINT cbFkTableName)
{
  UnicodeCall();
  WRITELOG("SQLForeignKeysW(%p,%p,%d,%p,%d,%p,%d,%p,%d,%p,%d,%p,%d"
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
    WRITELOG("SQLForeignKeysW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> pk_catalog(szPkCatalogName, cbPkCatalogName);
  ConvertingString<> pk_schema (szPkSchemaName,  cbPkSchemaName);
  ConvertingString<> pk_table  (szPkTableName,   cbPkTableName);
  ConvertingString<> fk_catalog(szFkCatalogName, cbFkCatalogName);
  ConvertingString<> fk_schema (szFkSchemaName,  cbFkSchemaName);
  ConvertingString<> fk_table  (szFkTableName,   cbFkTableName);
  return statement->SQLForeignKeys(pk_catalog,pk_catalog
                                  ,pk_schema, pk_schema
                                  ,pk_table,  pk_table
                                  ,fk_catalog,fk_catalog
                                  ,fk_schema, fk_schema
                                  ,fk_table,  fk_table);
}

SQLRETURN SQL_API
SQLNativeSqlW(SQLHDBC     ConnectionHandle
             ,SQLWCHAR*   InStatementText
             ,SQLINTEGER  TextLength1
             ,SQLWCHAR*   OutStatementText
             ,SQLINTEGER  BufferLength
             ,SQLINTEGER* TextLength2)
{
  UnicodeCall();
  WRITELOG("SQLNativeSQLW(%p,%p,%d,%p,%d,%p)"
          ,ConnectionHandle
          ,InStatementText, TextLength1
          ,OutStatementText,BufferLength,TextLength2);
  ODBCBaseObject* obj = (ODBCBaseObject*)ConnectionHandle;
  if(obj->ValidObject() != SQL_HANDLE_DBC)
  {
    WRITELOG("SQLNativeSqlW(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)obj;
  if(TextLength1 == SQL_NTS)
  {
    TextLength1 = (SQLINTEGER)wcslen( (const wchar_t*)InStatementText);
  }
  bool isByte = !( TextLength1 % 2);
  ConvertingString<> sql_in(InStatementText,TextLength1);
  ConvertingString<SQLINTEGER> sql_out(BufferLength,OutStatementText,TextLength2,isByte ? true : false);

  return connection->SQLNativeSql(sql_in,sql_in,sql_out,sql_out,TextLength2);
}

SQLRETURN SQL_API
SQLPrimaryKeysW(SQLHSTMT    StatementHandle
               ,SQLWCHAR*   szCatalogName
               ,SQLSMALLINT cbCatalogName
               ,SQLWCHAR*   szSchemaName
               ,SQLSMALLINT cbSchemaName
               ,SQLWCHAR*   szTableName
               ,SQLSMALLINT cbTableName)
{
  UnicodeCall();
  WRITELOG("SQLPrimaryKeysW(%p,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szCatalogName,cbCatalogName
          ,szSchemaName, cbSchemaName
          ,szTableName,  cbTableName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLPrimaryKeysW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> catalog(szCatalogName,cbCatalogName);
  ConvertingString<> schema (szSchemaName, cbSchemaName);
  ConvertingString<> table  (szTableName,  cbTableName);
  return statement->SQLPrimaryKeys(catalog,catalog
                                  ,schema, schema
                                  ,table,  table);
}

SQLRETURN SQL_API
SQLProcedureColumnsW(SQLHSTMT    StatementHandle
                    ,SQLWCHAR*   szCatalogName
                    ,SQLSMALLINT cbCatalogName
                    ,SQLWCHAR*   szSchemaName
                    ,SQLSMALLINT cbSchemaName
                    ,SQLWCHAR*   szProcName
                    ,SQLSMALLINT cbProcName
                    ,SQLWCHAR*   szColumnName
                    ,SQLSMALLINT cbColumnName)
{
  UnicodeCall();
  WRITELOG("SQLProcedureColumnsW(%p,%p,%d,%p,%d,%p,%d,%p,%d)"
           ,StatementHandle
           ,szCatalogName,cbCatalogName
           ,szSchemaName, cbSchemaName
           ,szProcName,   cbProcName
           ,szColumnName, cbColumnName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLProcedureColumnsW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> catalogs  (szCatalogName, cbCatalogName);
  ConvertingString<> schemas   (szSchemaName,  cbSchemaName);
  ConvertingString<> procedures(szProcName,    cbProcName);
  ConvertingString<> columns   (szColumnName,  cbColumnName);

  return statement->SQLProcedureColumns(catalogs,  catalogs
                                       ,schemas,   schemas
                                       ,procedures,procedures
                                       ,columns,   columns);
}

SQLRETURN SQL_API
SQLProceduresW(SQLHSTMT    StatementHandle
              ,SQLWCHAR*   szCatalogName
              ,SQLSMALLINT cbCatalogName
              ,SQLWCHAR*   szSchemaName
              ,SQLSMALLINT cbSchemaName
              ,SQLWCHAR*   szProcName
              ,SQLSMALLINT cbProcName)
{
  UnicodeCall();
  WRITELOG("SQLProceduresW(%p,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szCatalogName,cbCatalogName
          ,szSchemaName, cbSchemaName
          ,szProcName,   cbProcName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLProceduresW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> catalogs(szCatalogName,cbCatalogName);
  ConvertingString<> schemas (szSchemaName, cbSchemaName);
  ConvertingString<> procedures(szProcName, cbProcName);
  return statement->SQLProcedures(catalogs,  catalogs
                                 ,schemas,   schemas
                                 ,procedures,procedures);
}

SQLRETURN SQL_API
SQLTablePrivilegesW(SQLHSTMT    StatementHandle
                   ,SQLWCHAR*   szCatalogName
                   ,SQLSMALLINT cbCatalogName
                   ,SQLWCHAR*   szSchemaName
                   ,SQLSMALLINT cbSchemaName
                   ,SQLWCHAR*   szTableName
                   ,SQLSMALLINT cbTableName)
{
  UnicodeCall();
  WRITELOG("SQLTablePrivilegesW(%p,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szCatalogName,cbCatalogName
          ,szSchemaName ,cbSchemaName
          ,szTableName  ,cbTableName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLTablePrivilegesW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> catalog(szCatalogName,cbCatalogName);
  ConvertingString<> schema (szSchemaName, cbSchemaName);
  ConvertingString<> table  (szTableName,  cbTableName);
  return statement->SQLTablePrivileges(catalog,catalog,schema,schema,table,table);
}

SQLRETURN SQL_API
SQLColumnPrivilegesW(SQLHSTMT    StatementHandle
                    ,SQLWCHAR*   szCatalogName
                    ,SQLSMALLINT cbCatalogName
                    ,SQLWCHAR*   szSchemaName
                    ,SQLSMALLINT cbSchemaName
                    ,SQLWCHAR*   szTableName
                    ,SQLSMALLINT cbTableName
                    ,SQLWCHAR*   szColumnName
                    ,SQLSMALLINT cbColumnName)
{
  UnicodeCall();
  WRITELOG("SQLColumnPrivilegesW(%p,%p,%d,%p,%d,%p,%d,%p,%d"
          ,StatementHandle
          ,szColumnName,cbCatalogName
          ,szSchemaName,cbSchemaName
          ,szTableName ,cbTableName
          ,szColumnName,cbColumnName);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLColumnPrivilegesW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  ConvertingString<> catalog(szCatalogName,cbCatalogName);
  ConvertingString<> schema (szSchemaName, cbSchemaName);
  ConvertingString<> table  (szTableName,  cbTableName);
  ConvertingString<> column (szColumnName, cbColumnName);
  return statement->SQLColumnPrivileges(catalog,catalog
                                       ,schema, schema
                                       ,table,  table
                                       ,column, column);
}

SQLRETURN SQL_API
SQLColAttributeW(SQLHSTMT     StatementHandle
                ,SQLUSMALLINT ColumnNumber
                ,SQLUSMALLINT FieldIdentifier
                ,SQLPOINTER   CharacterAttribute
                ,SQLSMALLINT  BufferLength
                ,SQLSMALLINT* StringLength
#ifdef _WIN64
                ,SQLLEN*      NumericAttribute )
#else
                ,SQLPOINTER   NumericAttribute )
#endif
{
  UnicodeCall();
  WRITELOG("SQLColAttributeW(%p,%d,%d,%p,%d,%p,%p)"
    ,StatementHandle,ColumnNumber,FieldIdentifier,CharacterAttribute,BufferLength,StringLength,NumericAttribute);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLColAttributeW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  switch (FieldIdentifier)
  {
    case SQL_DESC_LABEL:
    case SQL_DESC_BASE_COLUMN_NAME:
    case SQL_COLUMN_NAME:
    case SQL_DESC_NAME:
    case SQL_DESC_TYPE_NAME:
    case SQL_DESC_BASE_TABLE_NAME:
    case SQL_DESC_TABLE_NAME:
    case SQL_DESC_SCHEMA_NAME:
    case SQL_DESC_CATALOG_NAME:
      if(BufferLength > 0)
      {
        ConvertingString<> characterAttribute(BufferLength,(SQLWCHAR *)CharacterAttribute,StringLength);
        return statement->SQLColAttribute(ColumnNumber
                                         ,FieldIdentifier
                                         ,(SQLCHAR*)characterAttribute
                                         ,characterAttribute
                                         ,StringLength
                                         ,(SQLLEN*)NumericAttribute);
      }
  }
  return statement->SQLColAttribute(ColumnNumber
                                   ,FieldIdentifier
                                   ,(SQLCHAR*)CharacterAttribute
                                   ,BufferLength
                                   ,StringLength
                                   ,(SQLLEN*)NumericAttribute);
}

SQLRETURN SQL_API 
SQLGetConnectAttrW(SQLHDBC     ConnectionHandle
                  ,SQLINTEGER  Attribute
                  ,SQLPOINTER  Value
                  ,SQLINTEGER  BufferLength
                  ,SQLINTEGER* StringLength)
{
  UnicodeCall();
  WRITELOG("SQLGetConnectAttrW(%p,%d,%p,%d,%p)",ConnectionHandle,Attribute,Value,BufferLength,StringLength);
  ODBCBaseObject *Obj = (ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_DBC)
  {
    WRITELOG("SQLGetConnectAttrW(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;
  switch(Attribute)
  {
    case SQL_ATTR_CURRENT_CATALOG:
    case SQL_ATTR_TRACEFILE:
    case SQL_ATTR_TRANSLATE_LIB:
      if(BufferLength > 0 || BufferLength == SQL_NTS )
      {
        ConvertingString<SQLINTEGER> value(BufferLength,(SQLWCHAR *)Value,StringLength);
        return connection->SQLGetConnectAttr(Attribute,(SQLPOINTER)(SQLCHAR*)value,value,StringLength);
      }
  }
  return connection->SQLGetConnectAttr(Attribute,Value,BufferLength,StringLength);
}

SQLRETURN SQL_API
SQLGetDescFieldW(SQLHDESC    DescriptorHandle
                ,SQLSMALLINT RecNumber
                ,SQLSMALLINT FieldIdentifier
                ,SQLPOINTER  Value
                ,SQLINTEGER  BufferLength
                ,SQLINTEGER* StringLength)
{
  UnicodeCall();
  WRITELOG("SQLGetDescFieldW(%p,%d,%d,%p,%d,%p)",DescriptorHandle,RecNumber,FieldIdentifier,Value,BufferLength,StringLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)DescriptorHandle;
  if(Obj->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLSetDescFieldW(%p) returns SQL_INVALID_HANDLE",DescriptorHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBC_ARD* descriptor = (ODBC_ARD*)Obj;
  switch(FieldIdentifier )
  {
    case SQL_DESC_BASE_COLUMN_NAME:
    case SQL_DESC_BASE_TABLE_NAME:
    case SQL_DESC_CATALOG_NAME:
    case SQL_DESC_LABEL:
    case SQL_DESC_LITERAL_PREFIX:
    case SQL_DESC_LITERAL_SUFFIX:
    case SQL_DESC_LOCAL_TYPE_NAME:
    case SQL_DESC_NAME:
    case SQL_DESC_SCHEMA_NAME:
    case SQL_DESC_TABLE_NAME:
    case SQL_DESC_TYPE_NAME:

      if(BufferLength > 0 || BufferLength == SQL_NTS)
      {
        //bool isByte = !(BufferLength % 2);
        ConvertingString<SQLINTEGER> value(BufferLength,(SQLWCHAR *)Value,StringLength);
        return descriptor->SQLGetDescField(RecNumber
                                          ,FieldIdentifier
                                          ,(SQLPOINTER)(SQLCHAR*)value
                                          ,value
                                          ,StringLength);
      }
  }
  return descriptor->SQLGetDescField(RecNumber,FieldIdentifier,Value,BufferLength,StringLength);
}

SQLRETURN SQL_API
SQLGetDescRecW(SQLHDESC     DescriptorHandle
              ,SQLSMALLINT  RecNumber
              ,SQLWCHAR*    Name
              ,SQLSMALLINT  BufferLength
              ,SQLSMALLINT* StringLength
              ,SQLSMALLINT* Type
              ,SQLSMALLINT* SubType
              ,SQLLEN*      Length
              ,SQLSMALLINT* Precision
              ,SQLSMALLINT* Scale
              ,SQLSMALLINT* Nullable)
{
  UnicodeCall();
  WRITELOG("SQLGetDescRecW(%p,%d,%p,%d,%p,%p,%p,%p,%p,%p,%p"
          ,DescriptorHandle,RecNumber,Name,BufferLength,StringLength
          ,Type,SubType,Length,Precision,Scale,Nullable);
  ODBCBaseObject* Obj = (ODBCBaseObject*)DescriptorHandle;
  if(Obj->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLGetDescRecW(%p) returns SQL_INVALID_HANDLE",DescriptorHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBC_ARD* descriptor = (ODBC_ARD*)Obj;
  ConvertingString<> name(BufferLength,Name,StringLength);
  return descriptor->SQLGetDescRec(RecNumber,name,name,StringLength,Type,SubType,Length,Precision,Scale,Nullable);
}

SQLRETURN SQL_API
SQLGetDiagFieldW(SQLSMALLINT   HandleType
                ,SQLHANDLE     Handle
                ,SQLSMALLINT   RecNumber
                ,SQLSMALLINT   DiagIdentifier
                ,SQLPOINTER    DiagInfo
                ,SQLSMALLINT   BufferLength
                ,SQLSMALLINT*  StringLength)
{
  UnicodeCall();
  WRITELOG("SQLGetDiagFieldW(%d,%p,%d,%d,%p,%d,%p)",HandleType,Handle,RecNumber,DiagIdentifier,DiagInfo,BufferLength,StringLength);
  ODBCBaseObject* obj = (ODBCBaseObject*) Handle;
  if(obj->ValidObject() != HandleType)
  {
    WRITELOG("SQLGetDiagFieldW(%p) returns SQL_INVALID_HANDLE",Handle);
    return SQL_INVALID_HANDLE;
  }
  switch(DiagIdentifier)
  {
    case SQL_DIAG_DYNAMIC_FUNCTION:
    case SQL_DIAG_CLASS_ORIGIN:
    case SQL_DIAG_CONNECTION_NAME:
    case SQL_DIAG_MESSAGE_TEXT:
    case SQL_DIAG_SERVER_NAME:
    case SQL_DIAG_SQLSTATE:
    case SQL_DIAG_SUBCLASS_ORIGIN:

      if(BufferLength > 0 || BufferLength == SQL_NTS)
      {
        ConvertingString<> diagInfo(BufferLength,(SQLWCHAR *)DiagInfo,StringLength);
        return obj->GetOCIErrorObject()->SQLGetDiagField(HandleType
                                                        ,obj
                                                        ,RecNumber
                                                        ,DiagIdentifier
                                                        ,(SQLPOINTER)(SQLCHAR*)diagInfo
                                                        ,diagInfo
                                                        ,StringLength);
      }
  }
  return obj->GetOCIErrorObject()->SQLGetDiagField(HandleType
                                                  ,obj
                                                  ,RecNumber
                                                  ,DiagIdentifier
                                                  ,DiagInfo
                                                  ,BufferLength
                                                  ,StringLength);
}

SQLRETURN SQL_API 
SQLGetDiagRecW(SQLSMALLINT  HandleType
              ,SQLHANDLE    Handle
              ,SQLSMALLINT  RecNumber
              ,SQLWCHAR*    Sqlstate
              ,SQLINTEGER*  NativeError
              ,SQLWCHAR*    MessageText
              ,SQLSMALLINT  BufferLength
              ,SQLSMALLINT* TextLength)
{
  UnicodeCall();
  WRITELOG("SQLGetDiagRecW(%s,%p,%d,%p,%p,%p,%d,%p)",GetHandleTypeDescription(HandleType),Handle,RecNumber,Sqlstate,NativeError,MessageText,BufferLength,TextLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)Handle;	
  if(Obj->ValidObject() != HandleType)
  {
    WRITELOG("SQLGetDiagRecW(%s,%p) returns SQL_INVALID_HANDLE",GetHandleTypeDescription(HandleType),Handle);
    return SQL_INVALID_HANDLE;
  }
  ConvertingString<> state(12,Sqlstate);
  // MSDN
  // DOC: ODBC Spec Incorrectly says SQLGetDiagRecW Takes in BufferLength as the Number of Bytes
  // PSS ID Number: 243526
  // Article Last Modified on 8/23/2001

  // should be Number of Bytes!!!
  // however ODBC.DLL wants Number of Character
  bool isByte = false;
  ConvertingString<> messageText(BufferLength,MessageText,TextLength,isByte);
  if(Obj == NULL)
  {
    // Special case for debugging of SQLAllocHandle
    return ODBCDriver.GetOCIErrorObject()->SQLGetDiagRec(RecNumber,state,NativeError,messageText,messageText,TextLength);    
  }
  return Obj->GetOCIErrorObject()->SQLGetDiagRec(RecNumber,state,NativeError,messageText,messageText,TextLength);    
}

SQLRETURN SQL_API
SQLSetStmtAttrW(SQLHSTMT   StatementHandle
               ,SQLINTEGER Attribute
               ,SQLPOINTER Value
               ,SQLINTEGER StringLength)
{
  UnicodeCall ();
  WRITELOG("SQLSetStmtAttrW(%p,%d,%p,%d)",StatementHandle,Attribute,Value,StringLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLSetStmtAttrW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLSetStmtAttr(Attribute,Value,StringLength);
}

SQLRETURN SQL_API 
SQLGetStmtAttrW(SQLHSTMT    StatementHandle
               ,SQLINTEGER  Attribute
               ,SQLPOINTER  Value
               ,SQLINTEGER  BufferLength
               ,SQLINTEGER* StringLength)
{
  UnicodeCall();
  WRITELOG("SQLGetStmtAttrW(%p,%d,%p,%d,%p)",StatementHandle,Attribute,Value,BufferLength,StringLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLGetStmtAttrW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLGetStmtAttr(Attribute,Value,BufferLength,StringLength);
}

SQLRETURN SQL_API 
SQLSetConnectAttrW(SQLHDBC    ConnectionHandle
                  ,SQLINTEGER Attribute
                  ,SQLPOINTER Value
                  ,SQLINTEGER StringLength)
{
  UnicodeCall();
  WRITELOG("SQLSetConnectAttrW(%p,%d,%p,%d)",ConnectionHandle,Attribute,Value,StringLength);
  ODBCBaseObject *Obj=(ODBCBaseObject*)ConnectionHandle;	
  if(Obj->ValidObject()!=SQL_HANDLE_DBC)
  {
    WRITELOG("SQLSetConnectAttrW(%p) returns SQL_INVALID_HANDLE",ConnectionHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCConnection* connection = (ODBCConnection*)Obj;
  switch(Attribute)
  {
    case SQL_ATTR_CURRENT_CATALOG:
    case SQL_ATTR_TRACEFILE:
    case SQL_ATTR_TRANSLATE_LIB:
    if(StringLength > 0 || StringLength == SQL_NTS)
    {
      ConvertingString<> value((SQLWCHAR *)Value,StringLength);
      return connection->SQLSetConnectAttr(Attribute,(SQLPOINTER)(SQLCHAR*)value,value);
    }
  }
  if(StringLength <= SQL_LEN_BINARY_ATTR_OFFSET)
  {
    StringLength = -StringLength + SQL_LEN_BINARY_ATTR_OFFSET;
  }
  return connection->SQLSetConnectAttr(Attribute,Value,StringLength);	
}

SQLRETURN SQL_API
SQLSetDescFieldW(SQLHDESC    DescriptorHandle
                ,SQLSMALLINT RecNumber
                ,SQLSMALLINT FieldIdentifier
                ,SQLPOINTER  Value
                ,SQLINTEGER  BufferLength)
{
  UnicodeCall ();
  WRITELOG("SQLSetDescFieldW(%p,%d,%d,%p,%d)",DescriptorHandle,RecNumber,FieldIdentifier,Value,BufferLength);
  ODBCBaseObject* Obj = (ODBCBaseObject*)DescriptorHandle;
  if(Obj->ValidObject() != SQL_HANDLE_DESC)
  {
    WRITELOG("SQLSetDescFieldW(%p) returns SQL_INVALID_HANDLE",DescriptorHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBC_ARD* descriptor = (ODBC_ARD*)Obj;
  if(descriptor->IsInternal())
  {
    descriptor->GetOCIErrorObject()->AddError("HY016");
    return SQL_ERROR;
  }
  switch(FieldIdentifier )
  {
    case SQL_DESC_BASE_COLUMN_NAME:
    case SQL_DESC_BASE_TABLE_NAME:
    case SQL_DESC_CATALOG_NAME:
    case SQL_DESC_LABEL:
    case SQL_DESC_LITERAL_PREFIX:
    case SQL_DESC_LITERAL_SUFFIX:
    case SQL_DESC_LOCAL_TYPE_NAME:
    case SQL_DESC_NAME:
    case SQL_DESC_SCHEMA_NAME:
    case SQL_DESC_TABLE_NAME:
    case SQL_DESC_TYPE_NAME:

    if(BufferLength > 0 || BufferLength == SQL_NTS)
    {
      int len;
      if (BufferLength == SQL_NTS)
      {
        len = (int)wcslen((const wchar_t*)Value);
      }
      else
      {
        len = BufferLength / 2;
      }
      ConvertingString<> value((SQLWCHAR *)Value,len);

      return descriptor->SQLSetDescField(RecNumber,FieldIdentifier,(SQLPOINTER)(SQLCHAR*)value,value);
    }
  }
  return descriptor->SQLSetDescField(RecNumber,FieldIdentifier,Value,BufferLength);
}

SQLRETURN SQL_API
SQLGetTypeInfoW(SQLHSTMT    StatementHandle
               ,SQLSMALLINT DataType)
{
  UnicodeCall();
  WRITELOG("SQLGetTypeInfoW(%p,%d)",StatementHandle,DataType);
  ODBCBaseObject* Obj = (ODBCBaseObject*)StatementHandle;	
  if(Obj->ValidObject() != SQL_HANDLE_STMT)
  {
    WRITELOG("SQLGetTypeInfoW(%p) returns SQL_INVALID_HANDLE",StatementHandle);
    return SQL_INVALID_HANDLE;
  }
  ODBCStatement* statement = (ODBCStatement*)Obj;
  return statement->SQLGetTypeInfo(DataType);
}

