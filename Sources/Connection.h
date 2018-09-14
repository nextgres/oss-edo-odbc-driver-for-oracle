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
// Contents:  Interface of the connection object
//            Holds a connection to a database for a user
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "Locking.h"
#include "BaseObject.h"
#include "ODBCList.h"
#include "Locking.h"
#include "EDO_Attributes.h"

// Max Statements in the connection cache
#define DB_STATMENT_CACHE_COUNT 5

// Values from SYS_CONTEXT('USERENV',<name>)
#define MAX_SESSION_PARAMNAME_LEN  32
#define MAX_SESSION_PARAMVALUE_LEN 62

class ODBCStatement;
class ODBCEnvironment;
class ODBC_ARD;

typedef BOOL (CALLBACK* TYPE_SQLDataSourceToDriver)(UDWORD,SWORD,SQLPOINTER,SDWORD,PTR,SDWORD,SDWORD*,UCHAR*,SWORD,SWORD*);
typedef BOOL (CALLBACK* TYPE_SQLDriverToDataSource)(UDWORD,SWORD,SQLPOINTER,SDWORD,PTR,SDWORD,SDWORD*,UCHAR*,SWORD,SWORD*);

// Base class for SQL_HANDLE_DBC type (SQLHDBC)
// Describe all base call for ODBC functions with SQLHDBC input descriptor
class ODBCConnection:public ODBCBaseObject
{
public:
  // Constructor		
  // lpEnv pointer to ODBCEnvironment to witch object will be connected
  ODBCConnection(ODBCEnvironment *lpEnv);
  // Destructor
  ~ODBCConnection();
  // Free object handler
  SQLRETURN FreeObjectHandler();
  // Validation of Connection object.
  // return SQL_INVALID_HANDLE (-2) if not valid object or SQL_HANDLE_DBC
  int ValidHandle();
  // Remove a statement object
  bool RemoveStatement(ODBCStatement* p_statement);
  // Get value of auto commit mode flag
  // return SQL_AUTOCOMMIT_ON if mode is on or SQL_AUTOCOMMIT_OFF if mode is off
  ODBCEnvironment* GetOwnerODBCEnv();
  OCISvcCtx*     GetOCIConnection();
  Mutex&         GetMutex();
  SQLRETURN      AllocDBStatement(ODBCStatement** lpObject);
  SQLRETURN      AllocDescriptor (ODBC_ARD**      lpObject);
  // Check whether this cursor name is unique in all statements
  SQLRETURN      UniqueCursor(SQLCHAR* p_cursorName);

  // General attribute GETTERS
  CString        GetServer();
  int            GetEnsureStatistics();
  bool           GetMetaDataID();
  SQLUINTEGER    GetAccessMode();
  SQLUINTEGER    GetAutoCommitMode();
  int            GetPrefetchCount();
  int            GetPrefetchMemory();
  int            GetFailoverCount();
  int            GetFailoverTime();
  int            GetMaxColumnLength();
  SQLUINTEGER*   GetTranslateOptions();
  SQLUINTEGER    GetAsyncMode();
  bool           GetIntegerSpeedup();

  // NLS Getters
  int            GetNLS();
  CString        GetNLSDateFormat();
  CString        GetNLSTimeFormat();
  CString        GetNLSTimestampFormat();
  CString        GetNLSTimestampFormatTZ();

  TYPE_SQLDataSourceToDriver* GetSQLDataSourceToDriver();
  TYPE_SQLDriverToDataSource* GetSQLDriverToDataSource();

  // THIS IS FOR THE ODBC Standard
  SQLRETURN SQLBrowseConnect (CString       connectString
                             ,SQLCHAR*      szConnStrOut
                             ,SQLSMALLINT   cbConnStrOutMax
                             ,SQLSMALLINT*  pcbConnStrOut);
  SQLRETURN SQLConnect       (CString&      p_dataSource
                             ,CString&      p_userName
                             ,CString&      p_authentication);
  SQLRETURN SQLDriverConnect (SQLHWND       hwnd
                             ,CString&      connectString
                             ,SQLCHAR*      szConnStrOut
                             ,SQLSMALLINT   cbConnStrOutMax
                             ,SQLSMALLINT*  pcbConnStrOut
                             ,SQLUSMALLINT  fDriverCompletion);
  SQLRETURN SQLDisconnect();
  SQLRETURN SQLEndTran       (SQLSMALLINT   CompletionType);
  SQLRETURN SQLGetConnectAttr(SQLINTEGER    Attribute
                             ,SQLPOINTER    Value
                             ,SQLINTEGER    BufferLength
                             ,SQLINTEGER*   StringLength);
  SQLRETURN SQLGetConnectOption(SQLUSMALLINT Option
                               ,SQLPOINTER   Value);
  SQLRETURN SQLGetFunctions  (SQLUSMALLINT  FunctionId
                             ,SQLUSMALLINT* Supported);
  SQLRETURN SQLGetInfo       (SQLUSMALLINT  InfoType
                             ,SQLPOINTER    InfoValue
                             ,SQLSMALLINT   BufferLength
                             ,SQLSMALLINT*  StringLength);	
  SQLRETURN SQLNativeSql     (SQLCHAR*      InStatementText
                             ,SQLINTEGER    TextLength1
                             ,SQLCHAR*      OutStatementText
                             ,SQLINTEGER    BufferLength
                             ,SQLINTEGER*   TextLength2);
  SQLRETURN SQLSetConnectAttr(SQLINTEGER    Attribute
                             ,SQLPOINTER    Value
                             ,SQLINTEGER    StringLength);
  SQLRETURN SQLSetConnectOption(SQLUSMALLINT Option
                               ,SQLULEN      Value);

protected:
  // Gets the oracle character set
  char*     GetCharset();
  // Gets the oracle version number from the server
  SQLRETURN GetOracleVersion();
  // Gets the driver options from the registry
  void      GetDriverOptions(CString& p_connectString);
  // Gets the connection status
  bool      GetConnectionDead();
  // Loads the translation DLL
  SQLRETURN SetTranslationDLL();
  // Loads the database name
  void      GetDatabaseName();
  // Loads the NLS parameters
  void      GetNLSParameters();

  Mutex   m_lock_mutex;       // Synchronization of Connection and all Statement object
  ODBCEnvironment* m_lpEnv;   // ODBC EnvObject (Parent of the connection)
  OCISvcCtx* m_hSvcCtx;       // OCI Connection object
  CString m_dataSource;       // The datasource we are connecting to
  CString m_server;           // The database server we are connecting to
  CString m_user;             // The logged in user
  CString m_oracleVersion;    // Version of the Oracle server
  ODBCList m_statements;      // All connections list array
  ODBCList m_descriptors;     // All application descriptors
  // Cache of statement allocated under creation
  ODBCStatement* m_lpCacheStmtObjects[DB_STATMENT_CACHE_COUNT];
  // Is currently transaction opened
  bool m_transactionBegin;
  // Number of iterations on SQLBrowseConnect
  int  m_browseConnect;
  // The translation library
  HMODULE m_translateLib;
  // Datasource conversions
  TYPE_SQLDataSourceToDriver m_SQLDataSourceToDriver;
  TYPE_SQLDriverToDataSource m_SQLDriverToDataSource;

  // Attributes of Connection object
  class ConnectionAttributes
  {
  public:
    ConnectionAttributes();                         // Constructor
    CString MakeConnectionString();                // Build resulting connect string
    void    ReadConnectionString(CString connect); // Read connection string
    void    ReadConnectionRegistry(const char* p_dataSource); // Read from the registry
    SQLUINTEGER m_accessMode;             // represent SQL_ATTR_ACCESS_MODE
    SQLUINTEGER m_asyncEnable;            // represent SQL_ATTR_ASYNC_ENABLE
    SQLUINTEGER m_autoCommit;             // represent SQL_ATTR_AUTOCOMMIT
    SQLUINTEGER m_connectionDead;         // represent SQL_ATTR_CONNECTION_DEAD
    SQLUINTEGER m_connectionTimeOut;      // represent SQL_ATTR_CONNECTION_TIMEOUT
    CString     m_currentCatalog;         // represent SQL_ATTR_CURRENT_CATALOG
    SQLUINTEGER m_loginTimeOut;           // represent SQL_ATTR_LOGIN_TIMEOUT
    int         m_maxColumnLength;        // represent SQL_ATTR_MAX_LENGTH  Default van een statement
    SQLUINTEGER m_metaDataID;             // represent SQL_ATTR_METADATA_ID
    SQLUINTEGER m_ODBCCursor;             // represent SQL_ATTR_ODBC_CURSORS
    SQLUINTEGER m_packetSize;             // represent SQL_ATTR_PACKET_SIZE
    SQLUINTEGER m_quietMode;              // represent SQL_ATTR_QUIET_MODE
    CString     m_translateLib;           // represent SQL_ATTR_TRANSLATE_LIB
    SQLUINTEGER m_translateOptions;       // represent SQL_ATTR_TRANSLATE_OPTION
    SQLUINTEGER m_TXNIsolation;           // represent SQL_ATTR_TXN_ISOLATION
    SQLUINTEGER m_ansiApplication;        // represent SQL_ATTR_ANSI_APP
    CString     m_driverName;             // represent SQL_ATTR_DRIVER
    // Driver specific
    int         m_statisticsEnsure;       // represent SQL_ATTR_EDO_ENSURE_STATISTICS  // DRIVER SPECIFIC!!
    int         m_prefetchCount;          // represent SQL_ATTR_EDO_PREFETCH_COUNT     // DRIVER SPECIFIC!!
    int         m_prefetchMemory;         // represent SQL_ATTR_EDO_PREFETCH_MEMORY    // DRIVER SPECIFIC!!
    int         m_failoverCount;          // represent SQL_ATTR_EDO_FAILOVER_COUNT     // DRIVER SPECIFIC!!
    int         m_failoverTime;           // represent SQL_ATTR_EDO_FAILOVER_TIME      // DRIVER SPECIFIC!!
    SQLUINTEGER m_trace;                  // represent SQL_ATTR_EDO_TRACE              // DRIVER SPECIFIC!!
    CString     m_traceFile;              // represent SQL_ATTR_EDO_TRACEFILE          // DRIVER SPECIFIC!!
    bool        m_integerSpeedup;         // represent SQL_ATTR_EDO_INTEGERSPEEDUP     // DRIVER SPECIFIC!!
    // NLS
    int         m_NLS_TYPE;
    CString     m_dbNlsDateFormat;        // DD-MM-YYYY HH24:MI:SS
    CString     m_dbNlsTimeFormat;        // HH24:MI:SSXFF
    CString     m_dbNlsTimestampFormat;   // DD-MM-RRRR HH24:MI:SSXFF
    CString     m_dbNlsTimestampTzFormat; // DD-MM-RRRR HH24:MI:SSXFF TZR
  }
  m_attributes;
};

inline ODBCEnvironment* 
ODBCConnection::GetOwnerODBCEnv()
{
  return m_lpEnv;
}

inline OCISvcCtx* 
ODBCConnection::GetOCIConnection()
{
  return m_hSvcCtx;
}	

inline Mutex& 
ODBCConnection::GetMutex()
{
  return m_lock_mutex;
}

inline SQLUINTEGER 
ODBCConnection::GetAutoCommitMode()
{
  return m_attributes.m_autoCommit;
}

inline CString
ODBCConnection::GetServer()
{
  return m_server;
}

inline int
ODBCConnection::GetEnsureStatistics()
{
  return m_attributes.m_statisticsEnsure;
}

inline bool
ODBCConnection::GetMetaDataID()
{
  return (m_attributes.m_metaDataID == 1);
}

inline SQLUINTEGER 
ODBCConnection::GetAccessMode()
{
  return m_attributes.m_accessMode;
}

inline int
ODBCConnection::GetPrefetchCount()
{
  return m_attributes.m_prefetchCount;
}

inline int
ODBCConnection::GetPrefetchMemory()
{
  return m_attributes.m_prefetchMemory;
}

inline int
ODBCConnection::GetFailoverCount()
{
  return m_attributes.m_failoverCount;
}

inline int
ODBCConnection::GetFailoverTime()
{
  return m_attributes.m_failoverTime;
}

inline int
ODBCConnection::GetMaxColumnLength()
{
  return m_attributes.m_maxColumnLength;
}

inline TYPE_SQLDataSourceToDriver*
ODBCConnection::GetSQLDataSourceToDriver()
{
  return &m_SQLDataSourceToDriver;
}

inline TYPE_SQLDriverToDataSource*
ODBCConnection::GetSQLDriverToDataSource()
{
  return &m_SQLDriverToDataSource;
}

inline SQLUINTEGER*
ODBCConnection::GetTranslateOptions()
{
  return &m_attributes.m_translateOptions;
}

inline SQLUINTEGER
ODBCConnection::GetAsyncMode()
{
  return m_attributes.m_asyncEnable;
}

inline bool
ODBCConnection::GetIntegerSpeedup()
{
  return m_attributes.m_integerSpeedup;
}

inline CString
ODBCConnection::GetNLSDateFormat()
{
  return m_attributes.m_dbNlsDateFormat;
}

inline CString
ODBCConnection::GetNLSTimestampFormat()
{
  return m_attributes.m_dbNlsTimestampFormat;
}

inline CString        
ODBCConnection::GetNLSTimestampFormatTZ()
{
  return m_attributes.m_dbNlsTimestampTzFormat;
}

inline CString
ODBCConnection::GetNLSTimeFormat()
{
  return m_attributes.m_dbNlsTimeFormat;
}

inline int
ODBCConnection::GetNLS()
{
  // 1 = ORACLE NLS
  // 2 = Standard ODBC
  // 3 = Specific ORACLE NLS Settings
  return (m_attributes.m_NLS_TYPE != 2);
}