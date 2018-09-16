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
// Contents:  Interface of the statement object
//            Holds a statement and result set of the statement
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "BaseObject.h"
#include "Connection.h"
#include <oci.h>
#include "Descriptor.h"
#include "ErrorInfo.h"

#define FETCHTYPE_NOTSET   0
#define FETCHTYPE_FETCH    1
#define FETCHTYPE_SCROLL   2
#define FETCHTYPE_EXTENDED 3

class ODBC_IRD;
class ODBC_ARD;

// Base class for SQL_HANDLE_STMT type (SQLHSTMT)
// Describe all base call for ODBC functions with SQLHSTMT input descriptor

class ODBCStatement:public ODBCBaseObject
{
public:
  // Constructor		
  ODBCStatement(ODBCConnection *lpConn,OCIStmt *hStmtTemp);	
  // Destructor
  ~ODBCStatement();	
  // Free object handler
  SQLRETURN FreeObjectHandler();
  // Out In Log file parameters information (App buffer & Driver Buffer)
  void LogParamInformation();
  // Out In Log file columns information (App buffer & Driver Buffer)
  void LogColumnInformation();
  // Validation of Statement
  // return SQL_INVALID_HANDLE (-2) if not valid object or SQL_HANDLE_STMT
  int ValidHandle();
  // OCI Statement handle
  OCIStmt* GetOCIStatement();
  // The ODBC connection object
  ODBCConnection* GetDBConnection();
  // Is the statement free in the pool
  bool IsFreeInPool();
  // Block the object in the bool
  void BlockObjectInPool();
  // Set the pool index
  void    SetPoolIndex(int p_index);
  // Set metadata ID as a result of a new statement
  void    SetMetadataID(bool p_metaID);
  // Get the cached statement string
  CString GetCachedSQL();
  // Get cached statement type
  int     GetCachedSQLType();
  // Get the cursor name
  CString GetCursor();
  // Get max length of data copy
  int     GetMaxLength();
  // Get integer speedup setting
  bool    GetIntegerSpeedup();
  // Get the pool index
  int     GetPoolIndex();


  // ODBC Standard functions
  SQLRETURN SQLBindCol        (SQLUSMALLINT ColumnNumber
                              ,SQLSMALLINT  TargetType
                              ,SQLPOINTER   TargetValue
                              ,SQLLEN       BufferLength
                              ,SQLLEN*      StrLen_or_Ind);
  SQLRETURN SQLBindParameter  (SQLUSMALLINT ParameterNumber
                              ,SQLSMALLINT  InputOutputType
                              ,SQLSMALLINT  ValueType
                              ,SQLSMALLINT  ParameterType
                              ,SQLLEN       ColumnSize
                              ,SQLSMALLINT  DecimalDigits
                              ,SQLPOINTER   ParameterValuePtr
                              ,SQLLEN       BufferLength
                              ,SQLLEN*      StrLen_or_IndPtr);
  SQLRETURN SQLCancel         ();
  SQLRETURN SQLCloseCursor    ();
  SQLRETURN SQLColAttribute   (SQLUSMALLINT ColumnNumber
                              ,SQLUSMALLINT FieldIdentifier
                              ,SQLCHAR*     CharacterAttribute
                              ,SQLSMALLINT  BufferLength
                              ,SQLSMALLINT* StringLength
                              ,SQLLEN*      NumericAttribute);
  SQLRETURN SQLColumnPrivileges(SQLCHAR*    szCatalogName
                              ,SQLSMALLINT  cbCatalogName
                              ,SQLCHAR*     szSchemaName
                              ,SQLSMALLINT  cbSchemaName
                              ,SQLCHAR*     szTableName
                              ,SQLSMALLINT  cbTableName
                              ,SQLCHAR*     szColumnName
                              ,SQLSMALLINT  cbColumnName);
  SQLRETURN SQLColumns        (SQLCHAR*     CatalogName
                              ,SQLSMALLINT  NameLength1
                              ,SQLCHAR*     SchemaName
                              ,SQLSMALLINT  NameLength2
                              ,SQLCHAR*     TableName
                              ,SQLSMALLINT  NameLength3
                              ,SQLCHAR*     ColumnName
                              ,SQLSMALLINT  NameLength4);
    SQLRETURN SQLDescribeCol  (SQLUSMALLINT ColumnNumber
                              ,SQLCHAR*     ColumnName
                              ,SQLSMALLINT  BufferLength
                              ,SQLSMALLINT* NameLength
                              ,SQLSMALLINT* DataType
                              ,SQLULEN*     ColumnSize
                              ,SQLSMALLINT* DecimalDigits
                              ,SQLSMALLINT* Nullable);
  SQLRETURN SQLDescribeParam  (SQLUSMALLINT ipar
                              ,SQLSMALLINT* pfSqlType
                              ,SQLULEN*     pcbParamDef
                              ,SQLSMALLINT* pibScale
                              ,SQLSMALLINT* pfNullable);
  SQLRETURN SQLExecute        ();
  SQLRETURN SQLExtendedFetch  (SQLUSMALLINT fFetchType
                              ,SQLLEN       irow
                              ,SQLULEN*     pcrow
                              ,SQLUSMALLINT* rgfRowStatus);
  SQLRETURN SQLFetch          ();
  SQLRETURN SQLFetchScroll    (SQLSMALLINT  FetchOrientation
                              ,SQLLEN       FetchOffset);
  SQLRETURN SQLForeignKeys    (SQLCHAR*     szPkCatalogName
                              ,SQLSMALLINT  cbPkCatalogName
                              ,SQLCHAR*     szPkSchemaName
                              ,SQLSMALLINT  cbPkSchemaName
                              ,SQLCHAR*     szPkTableName
                              ,SQLSMALLINT  cbPkTableName
                              ,SQLCHAR*     szFkCatalogName
                              ,SQLSMALLINT  cbFkCatalogName
                              ,SQLCHAR*     szFkSchemaName
                              ,SQLSMALLINT  cbFkSchemaName
                              ,SQLCHAR*     szFkTableName
                              ,SQLSMALLINT  cbFkTableName);
  SQLRETURN SQLFreeStmt       (SQLUSMALLINT Option);
  SQLRETURN SQLGetCursorName  (SQLCHAR*     CursorName
                              ,SQLSMALLINT  BufferLength
                              ,SQLSMALLINT* NameLength);
  SQLRETURN SQLGetData        (SQLUSMALLINT ColumnNumber
                              ,SQLSMALLINT  TargetType
                              ,SQLPOINTER   TargetValue
                              ,SQLLEN       BufferLength
                              ,SQLLEN*      StrLen_or_Ind);
  SQLRETURN SQLGetStmtAttr    (SQLINTEGER   Attribute
                              ,SQLPOINTER   Value
                              ,SQLINTEGER   BufferLength
                              ,SQLINTEGER*  StringLength);
  SQLRETURN SQLGetStmtOption  (SQLUSMALLINT Option
                              ,SQLPOINTER   Value);
  SQLRETURN SQLGetTypeInfo    (SQLSMALLINT  DataType);
  SQLRETURN SQLMoreResults    ();
  SQLRETURN SQLNumParams      (SQLSMALLINT* ParameterCountPtr);
  SQLRETURN SQLNumResultCols  (SQLSMALLINT* ColumnCount);
  SQLRETURN SQLParamData      (SQLPOINTER*  Value);
  SQLRETURN SQLPrepare        (CString&     p_sql);
  SQLRETURN SQLPrimaryKeys    (SQLCHAR*     szCatalogName
                              ,SQLSMALLINT  cbCatalogName
                              ,SQLCHAR*     szSchemaName
                              ,SQLSMALLINT  cbSchemaName
                              ,SQLCHAR*     szTableName
                              ,SQLSMALLINT  cbTableName);
  SQLRETURN SQLProcedureColumns(SQLCHAR*    szCatalogName
                              ,SQLSMALLINT  cbCatalogName
                              ,SQLCHAR*     szSchemaName
                              ,SQLSMALLINT  cbSchemaName
                              ,SQLCHAR*     szProcName
                              ,SQLSMALLINT  cbProcName
                              ,SQLCHAR*     szColumnName
                              ,SQLSMALLINT  cbColumnName);
  SQLRETURN SQLProcedures     (SQLCHAR*     szCatalogName
                              ,SQLSMALLINT  cbCatalogName
                              ,SQLCHAR*     szSchemaName
                              ,SQLSMALLINT  cbSchemaName
                              ,SQLCHAR*     szProcName
                              ,SQLSMALLINT  cbProcName);
  SQLRETURN SQLPutData        (SQLPOINTER   Data
                              ,SQLLEN       StrLen_or_Ind);
  SQLRETURN SQLRowCount       (SQLLEN*      RowCount);
  SQLRETURN SQLSetCursorName  (SQLCHAR*     p_cursorName);
  SQLRETURN SQLSetPos         (SQLSETPOSIROW irow
                              ,SQLUSMALLINT fOption
                              ,SQLUSMALLINT fLock);
  SQLRETURN SQLSetScrollOptions(SQLUSMALLINT fConcurrency
                              ,SQLLEN       crowKeyset
                              ,SQLUSMALLINT crowRowset);
  SQLRETURN SQLSetStmtAttr    (SQLINTEGER   Attribute
                              ,SQLPOINTER   Value
                              ,SQLINTEGER   StringLength);
  SQLRETURN SQLSetStmtOption  (SQLUSMALLINT Option
                              ,SQLULEN      Value);
  SQLRETURN SQLSpecialColumns (SQLCHAR*     CatalogName
                              ,SQLSMALLINT  NameLength1
                              ,SQLCHAR*     SchemaName
                              ,SQLSMALLINT  NameLength2
                              ,SQLCHAR*     TableName
                              ,SQLSMALLINT  NameLength3);
  SQLRETURN SQLStatistics     (SQLCHAR*     CatalogName
                              ,SQLSMALLINT  NameLength1
                              ,SQLCHAR*     SchemaName
                              ,SQLSMALLINT  NameLength2
                              ,SQLCHAR*     TableName
                              ,SQLSMALLINT  NameLength3
                              ,SQLUSMALLINT Unique
                              ,SQLUSMALLINT Reserved);
  SQLRETURN SQLTablePrivileges(SQLCHAR*     szCatalogName
                              ,SQLSMALLINT  cbCatalogName
                              ,SQLCHAR*     szSchemaName
                              ,SQLSMALLINT  cbSchemaName
                              ,SQLCHAR*     szTableName
                              ,SQLSMALLINT  cbTableName);
  SQLRETURN SQLTables         (SQLCHAR*     CatalogName
                              ,SQLSMALLINT  NameLength1
                              ,SQLCHAR*     SchemaName
                              ,SQLSMALLINT  NameLength2
                              ,SQLCHAR*     TableName
                              ,SQLSMALLINT  NameLength3
                              ,SQLCHAR*     TableType
                              ,SQLSMALLINT  NameLength4);

  // TRANSLATE LIBRARY addresses
  TYPE_SQLDataSourceToDriver* m_SQLDataSourceToDriver;
  TYPE_SQLDriverToDataSource* m_SQLDriverToDataSource;
  SQLUINTEGER* m_translateOptions;

protected:
  // Reprint the catalog.schema.table combination for this DBMS
  CString    MakeObjectName(CString catalog,CString schema,CString table);
  // Register an application row descriptor
  SQLRETURN  SetNewARD(SQLPOINTER p_ard);
  // Register an application parameter descriptor
  SQLRETURN  SetNewAPD(SQLPOINTER p_apd);
  // Close the cursor of the statement
  SQLRETURN  CloseCursor();
  // Test failover server
  void       TestServerConnection();
  // Set the prefetch parameters of the OCI for this statement
  void       SetPrefetch();
  // Do the fetch dance
  SQLRETURN  InternalFetch(SQLINTEGER p_direction,SQLLEN p_irow,SQLULEN* p_gotten,SQLUSMALLINT* p_status_ptr);

  ODBCConnection* m_connection;     // Connection owner of this statement
  OCIStmt*    m_hStmt;              // OCI Statement 
  bool        m_noData;             // tell for fetch operation that there is SQL_NO_DATA in the statement
  bool        m_inNeedForData;      // Execute stopped at SQL_NEED_DATA
  int         m_needDataColumn;     // Column that needs data in SQL_NEED_DATA
  ub1         m_needPiece;          // Piece (FIRST/NEXT/LAST) for SQL_NEED_DATA
  ODBC_IRD*   m_IRD;                // Internal Record Descriptors (columns)
  ODBC_IRD*   m_IPD;                // Internal Parameter Descriptors (params)
  ODBC_ARD*   m_ARD;                // Application Record Descriptors (columns)
  ODBC_ARD*   m_APD;                // Application Parameter Descriptors (params)
  bool        m_newSQL;             // Is new Prepare on Statement was Executed

  int   m_connectionPoolIndex;      // index in DB connection statement pool, if not in pool simply -1
  bool  m_isObjectFreeInPool;       // if object is in pool, is it free	?
  Mutex m_lock_mutex;               // Synchronization of Connection and all Statements

  bool  m_cancel;                   // Is in canceled state?
  int   m_fetchType;                // Statement is used with SQLExtendedFetch instead of SQLFetch

  class StmtAttributes
  {
  public:
    StmtAttributes();
    bool      m_metaDataID;             // For SCHEMA functions (Tables, Columns, Procedures, Privileges, ForeignKeys etc)
    bool      m_async;                  // Asynchronous statement execution
    bool      m_noscan;                 // Do not scan for { escape sequences
    int       m_statementType;          // Statement type according to the OCI
    CString   m_cachedStatement;        // Cached SQL string of this statement (for GetDiagField)
    int       m_maxLength;              // Max length of data in SQLGetData
    CString   m_cursorName;             // Name of scrolling cursor
    bool      m_cursorScrollable;       // Cursor is scrollable cursor
    int       m_maxRows;                // Max rows in the cursor
    int       m_queryTimeout;           // Query timeout in seconds
    bool      m_retrieveData;           // Directly retrieve data after Fetch
    bool      m_integerSpeedup;         // Use undefined numbers are integers
  }
  m_attributes;
};

inline OCIStmt* 
ODBCStatement::GetOCIStatement()
{
  return m_hStmt;
}

inline ODBCConnection* 
ODBCStatement::GetDBConnection()
{ 
  return m_connection;
}

inline bool 
ODBCStatement::IsFreeInPool()
{
  return m_isObjectFreeInPool;
}

inline void 
ODBCStatement::BlockObjectInPool()
{
  m_isObjectFreeInPool = false;
}

inline void 
ODBCStatement::SetPoolIndex(int p_index)
{
  m_connectionPoolIndex = p_index;
}

inline CString 
ODBCStatement::GetCachedSQL()
{
  return m_attributes.m_cachedStatement;
}

inline int
ODBCStatement::GetCachedSQLType()
{
  return m_attributes.m_statementType;
}

inline CString
ODBCStatement::GetCursor()
{
  return m_attributes.m_cursorName;
}

inline int
ODBCStatement::GetMaxLength()
{
  return m_attributes.m_maxLength;
}

inline void
ODBCStatement::SetMetadataID(bool p_metaID)
{
  m_attributes.m_metaDataID = p_metaID;
}

inline bool
ODBCStatement::GetIntegerSpeedup()
{
  return m_attributes.m_integerSpeedup;
}

inline int
ODBCStatement::GetPoolIndex()
{
  return m_connectionPoolIndex;
}
