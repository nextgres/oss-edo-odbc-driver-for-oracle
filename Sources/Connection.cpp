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
// Contents:  Implementation of the connection object
//            Holds a connection to a database for a user
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "Connection.h"
#include "MainObject.h"
#include "ErrorInfo.h"
#include "Environment.h"
#include "Statement.h"
#include "ODBCTypes.h"
#include "LogonDlg.h"
#include "DescriptorIRD.h"
#include "DescriptorARD.h"
#include "Parser.h"
#include "Utility.h"
#include <odbcinst.h>
#include <string.h>

#pragma warning (disable: 4996)
#pragma warning (disable: 4311)

#define STILL_EXECUTING if(m_lock_mutex.islocked() && GetAsyncMode())\
                        return SQL_STILL_EXECUTING

const char* ODBCINI = "ODBC.INI";
char* databaseKeywords = 
      "ACCESS,AUDIT,BFILE,BLOB,CLOB,CLUSTER,COMMENT,COMPRESS,EXCLUSIVE,FILE,"
      "IDENTIFIED,INCREMENT,INITIAL,LOCK,LONG,MAXEXTENTS,MINUS,MODE,MODIFY,NCLOB,NOAUDIT,"
      "NOCOMPRESS,NOWAIT,NUMBER,OFFLINE,ONLINE,PCTFREE,SYSDATE,SYNONYM,RAW,RENAME,RESOURCE,"
      "ROW,ROWID,ROWLABEL,ROWNUM,SHARE,START,SUCCESSFUL,TRIGGER,UID,VALIDATE,VARCHAR2";

// Used for validation of a connection: CONN
__int64 staticConnection = 0x67504F4F;

ODBCConnection::ConnectionAttributes::ConnectionAttributes()
{
	m_accessMode        = SQL_MODE_READ_WRITE;
	m_asyncEnable       = SQL_ASYNC_ENABLE_OFF;
	m_autoCommit        = SQL_AUTOCOMMIT_ON;
	m_connectionDead    = SQL_CD_FALSE;
	m_connectionTimeOut = 0;
	m_currentCatalog    = "";
	m_metaDataID        = SQL_FALSE;
	m_ODBCCursor        = SQL_CUR_USE_DRIVER;
	m_packetSize        = 1024*4;
	m_quietMode         = NULL;
	m_trace             = SQL_OPT_TRACE_OFF;
  m_traceFile         = "";
	m_translateLib      = "";
	m_translateOptions  = 0;
	m_TXNIsolation      = 0;
  m_statisticsEnsure  = SQL_QUICK;
  m_prefetchCount     = 100;        // Minimum of 100 rows
  m_prefetchMemory    = 64 * 1024;  // Minimum of 64 kilobyte
  m_failoverCount     = 0;
  m_failoverTime      = 0;
  m_maxColumnLength   = 0;
  m_ansiApplication   = 0;
  m_integerSpeedup    = false;
  m_NLS_TYPE          = 1;          // USE ORACLE NLS
  m_dbNlsDateFormat   = "";
  m_dbNlsTimeFormat   = "";
  m_dbNlsTimestampFormat   = "";
  m_dbNlsTimestampTzFormat = "";
}

// Build resulting connect string
CString
ODBCConnection::ConnectionAttributes::MakeConnectionString()
{
  CString conn;
  conn.Format("RDO=%d;ASY=%d;AUT=%d;OUT=%d;MID=%d;CUR=%d;PAS=%d;QUT=%d;TRC=%d;"
              "TRF=%s;TRL=%s;TRO=%d;STA=%d;PFC=%d;PFM=%d;FOC=%d;FOT=%d;MCL=%d;"
              "ISU=%d;"
              ,m_accessMode         // RDO = Read Only    1 = Readonly, 0 = Read/Write
              ,m_asyncEnable        // ASY = Async Enable 1 = Async on, 0 = Async off
              ,m_autoCommit         // AUT = Autocommit   1 = commit on,0 = autocommit off
              ,m_connectionTimeOut  // OUT = Timeout in seconds
              ,m_metaDataID         // MID = MetadataID   1 = on,       0 = off
              ,m_ODBCCursor         // CUR = 0 = ifneeded,1 = ODBC, 2 = Driver
              ,m_packetSize         // PAS = packet size in bytes
              ,m_quietMode          // QUT = Quiet mode   1 = quiet,    0 = messages
              ,m_trace              // TRC = Tracing      1 = on,       0 = off
              ,m_traceFile          // TRF = Tracefile (string)
              ,m_translateLib       // TRL = Translate lib (string)
              ,m_translateOptions   // TRO = Translate options (integer)
              ,m_TXNIsolation       // TXI = Transaction isolation (integer)
              ,m_statisticsEnsure   // STA = Statistics ensure 1 = on, 0 = off
              ,m_prefetchCount      // PFC = Prefetch count (integer)
              ,m_prefetchMemory     // PFM = Prefetch memory (integer)
              ,m_failoverCount      // FOC = Failover count (integer)
              ,m_failoverTime       // FOT = Failover time (integer)
              ,m_maxColumnLength    // MCL = Max Column Length (integer)
              ,m_integerSpeedup     // ISU = Integer Speed Up (bool)
              );
  return conn;
}

void    
ODBCConnection::ConnectionAttributes::ReadConnectionString(CString connect)
{
  char value[21];
  char filename[_MAX_PATH+1];
  if(GetVariableFromConnectionString(connect,"RDD=",value,20))
  {
    m_accessMode = (atoi(value) != 0);
  }
  if(GetVariableFromConnectionString(connect,"ASY=",value,20))
  {
    m_asyncEnable = (atoi(value) != 0);
  }
  if(GetVariableFromConnectionString(connect,"AUT=",value,20))
  {
    m_autoCommit = (atoi(value) != 0);
  }
  if(GetVariableFromConnectionString(connect,"OUT=",value,20))
  {
    m_connectionTimeOut = atoi(value);
    if(m_connectionTimeOut < 0)
    {
      m_connectionTimeOut = 0;
    }
  }
  if(GetVariableFromConnectionString(connect,"MID=",value,20))
  {
    m_metaDataID = (atoi(value) != 0);
  }
  if(GetVariableFromConnectionString(connect,"CUR=",value,20))
  {
    m_ODBCCursor = atoi(value);
    if(m_ODBCCursor < 0 || m_ODBCCursor > 2)
    {
      m_ODBCCursor = 0;
    }
  }
  if(GetVariableFromConnectionString(connect,"PAS=",value,20))
  {
    m_packetSize = atoi(value);
    if(m_packetSize < 0) m_packetSize = 0;
  }
  if(GetVariableFromConnectionString(connect,"QUT=",value,20))
  {
    m_quietMode = (atoi(value) != 0);
  }
  if(GetVariableFromConnectionString(connect,"TRC=",value,20))
  {
    m_trace = (atoi(value) != 0);
  }
  if(GetVariableFromConnectionString(connect,"TRF=",filename,_MAX_PATH))
  {
    m_traceFile = filename;
  }
  if(GetVariableFromConnectionString(connect,"TRL=",filename,_MAX_PATH))
  {
    m_translateLib = filename;
  }
  if(GetVariableFromConnectionString(connect,"TRO=",value,20))
  {
    m_translateOptions = atoi(value);
  }
  if(GetVariableFromConnectionString(connect,"TRI=",value,20))
  {
    m_TXNIsolation = atoi(value);
  }
  if(GetVariableFromConnectionString(connect,"STA=",value,20))
  {
    m_statisticsEnsure = (atoi(value) != 0);
  }
  if(GetVariableFromConnectionString(connect,"PFC=",value,20))
  {
    m_prefetchCount = atoi(value);
  }
  if(GetVariableFromConnectionString(connect,"PFM=",value,20))
  {
    m_prefetchMemory = atoi(value);
  }
  if(GetVariableFromConnectionString(connect,"FOC=",value,20))
  {
    m_failoverCount = atoi(value);
  }
  if(GetVariableFromConnectionString(connect,"FOT=",value,20))
  {
    m_failoverTime = atoi(value);
  }
  if(GetVariableFromConnectionString(connect,"MCL=",value,20))
  {
    m_maxColumnLength = atoi(value);
    if(m_maxColumnLength < 0) m_maxColumnLength = 0;
  }
  if(GetVariableFromConnectionString(connect,"ISU=",value,20))
  {
    m_integerSpeedup = atoi(value) != 0;
  }
}

void
ODBCConnection::ConnectionAttributes::ReadConnectionRegistry(const char* p_dataSource)
{
  char value[20];
  char file[_MAX_PATH+2];
  if(SQLGetPrivateProfileString(p_dataSource,"ReadOnly","",value,18,ODBCINI))
  {
    m_accessMode = (atoi(value) == SQL_MODE_READ_ONLY);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"AsyncEnable","",value,18,ODBCINI))
  {
    m_asyncEnable = (atoi(value) == SQL_TRUE);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"AutoCommit","",value,18,ODBCINI))
  {
    m_autoCommit = (atoi(value) == SQL_TRUE);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"ConnectionTimeout","",value,18,ODBCINI))
  {
    m_connectionTimeOut = atoi(value);
    if(m_connectionTimeOut < 0) m_connectionTimeOut = 0;
  }
  if(SQLGetPrivateProfileString(p_dataSource,"MetaDataID","",value,18,ODBCINI))
  {
    m_metaDataID = (atoi(value) != 0);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"OdbcCursor","",value,18,ODBCINI))
  {
    m_ODBCCursor = atoi(value);
    if(m_ODBCCursor < 0 || m_ODBCCursor > 2)
    {
      m_ODBCCursor = 0;
    }
  }
  if(SQLGetPrivateProfileString(p_dataSource,"PacketSize","",value,18,ODBCINI))
  {
    m_packetSize = atoi(value);
    if(m_packetSize < 0) m_packetSize = 0;
  }
  if(SQLGetPrivateProfileString(p_dataSource,"QuietMode","",value,18,ODBCINI))
  {
    m_quietMode = (atoi(value) == SQL_TRUE);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"TraceLevel","",value,18,ODBCINI))
  {
    m_trace = atoi(value);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"Tracefile","",file,_MAX_PATH,ODBCINI))
  {
    m_traceFile = file;
  }
  if(SQLGetPrivateProfileString(p_dataSource,"TranslateLib","",file,_MAX_PATH,ODBCINI))
  {
    m_translateLib = file;
  }
  if(SQLGetPrivateProfileString(p_dataSource,"TranslateOption","",value,18,ODBCINI))
  {
    m_translateOptions = atoi(value);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"TransactionIsolation","",value,18,ODBCINI))
  {
    m_TXNIsolation = atoi(value);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"StatisticsEnsure","",value,18,ODBCINI))
  {
    m_statisticsEnsure = (atoi(value) == SQL_ENSURE);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"PrefetchCount","",value,18,ODBCINI))
  {
    m_prefetchCount = atoi(value);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"PrefetchMemory","",value,18,ODBCINI))
  {
    m_prefetchMemory = atoi(value);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"FailoverCount","",value,18,ODBCINI))
  {
    m_failoverCount = atoi(value);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"FailoverTime","",value,18,ODBCINI))
  {
    m_failoverTime = atoi(value);
  }
  if(SQLGetPrivateProfileString(p_dataSource,"ColumnMaxLength","",value,18,ODBCINI))
  {
    m_maxColumnLength = atoi(value);
    if(m_maxColumnLength < 0) m_maxColumnLength = 0;
  }
  if(SQLGetPrivateProfileString(p_dataSource,"IntegerSpeedUp","",value,18,ODBCINI))
  {
    m_integerSpeedup = atoi(value) != 0;
  }
  if(SQLGetPrivateProfileString(p_dataSource,"NLS_TYPE","1",value,18,ODBCINI))
  {
    m_NLS_TYPE = atoi(value);
    if(m_NLS_TYPE < 1 || m_NLS_TYPE > 3)
    {
      m_NLS_TYPE = 1;
    }
  }
  if(m_NLS_TYPE != 1)
  {
    if(SQLGetPrivateProfileString(p_dataSource,"NLS_DATE_FORMAT","",file,_MAX_PATH,ODBCINI))
    {
      m_dbNlsDateFormat = file;
    }
    if(SQLGetPrivateProfileString(p_dataSource,"NLS_TIME_FORMAT","",file,_MAX_PATH,ODBCINI))
    {
      m_dbNlsTimeFormat = file;
    }
    if(SQLGetPrivateProfileString(p_dataSource,"NLS_TIMESTAMP_FORMAT","",file,_MAX_PATH,ODBCINI))
    {
      m_dbNlsTimestampFormat = file;
    }
    if(SQLGetPrivateProfileString(p_dataSource,"NLS_TIMESTAMP_TZ_FORMAT","",file,_MAX_PATH,ODBCINI))
    {
      m_dbNlsTimestampTzFormat = file;
    }
    if(m_NLS_TYPE == 3)
    {
      // Specifice settings from the setup dll
      sprintf_s(file,_MAX_PATH,"NLS_DATE_FORMAT=%s",m_dbNlsDateFormat);
      _putenv(file);
      sprintf_s(file,_MAX_PATH,"NLS_TIME_FORMAT=%s",m_dbNlsTimeFormat);
      _putenv(file);
      sprintf_s(file,_MAX_PATH,"NLS_TIMESTMP_FORMAT=%s",m_dbNlsTimestampFormat);
      _putenv(file);
      sprintf_s(file,_MAX_PATH,"NLS_TIMESTMP_TZ_FORMAT=%s",m_dbNlsTimestampTzFormat);
      _putenv(file);
    }
  }
  // Always load this static
  if(SQLGetPrivateProfileString(p_dataSource,"Driver","",file,_MAX_PATH,ODBCINI))
  {
    char driver[_MAX_PATH];
    char extens[_MAX_PATH];
    _splitpath(file,NULL,NULL,driver,extens);
    m_driverName.Format("%s%s",driver,extens);
    m_driverName.MakeUpper();
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  THE CONNECTION
//
//////////////////////////////////////////////////////////////////////////

ODBCConnection::ODBCConnection(ODBCEnvironment* lpEnvParam)
               :ODBCBaseObject(lpEnvParam->GetOwnerEnv())
               ,m_hSvcCtx(NULL)
               ,m_transactionBegin(false)
               ,m_lpEnv(lpEnvParam)
               ,m_browseConnect(0)
               ,m_translateLib(NULL)
               ,m_SQLDataSourceToDriver(NULL)
               ,m_SQLDriverToDataSource(NULL)
{	
	m_localSign = staticConnection;
	int i;
	for(i=0; i<DB_STATMENT_CACHE_COUNT; i++)
  {
    m_lpCacheStmtObjects[i] = NULL;
  }
}

ODBCConnection::~ODBCConnection()
{
  WRITELOG("ODBCConnection::~ODBCConnection(%p)",this);
  m_statements. RemoveAllObjects();
  m_descriptors.RemoveAllObjects();
  if(m_hSvcCtx)
  {
    OCILogoff(m_hSvcCtx,m_ociError.GetOwnerErrorObject());
    OCIHandleFree(m_hSvcCtx,OCI_HTYPE_SVCCTX);
    m_hSvcCtx = NULL;
  }
  if(m_translateLib)
  {
    FreeLibrary(m_translateLib);
    m_translateLib = NULL;
  }
  m_ociError.Done();
  // Remove myself from the environment
  m_lpEnv->RemoveConnection(this);
  m_lpEnv  = NULL;
  m_server = "";
  m_user   = "";
  m_dataSource = "";
  m_localSign  = 0;
}

SQLRETURN 
ODBCConnection::FreeObjectHandler()
{
  WRITELOG("ODBCConnection::FreeObjectHandler(%p)",this);
  if(m_statements.HasObjects())
  {
    return SQL_ERROR;
  }
  delete this;
  return SQL_SUCCESS;
}

int 
ODBCConnection::ValidHandle()
{
  // compare internal signification with global sign for this class
  if(m_localSign == staticConnection)
  {
    return SQL_HANDLE_DBC;
  }
  else
  {
    return SQL_INVALID_HANDLE;
  }
}

SQLRETURN
ODBCConnection::AllocDescriptor(ODBC_ARD** lpObject)
{
  // Create a new application descriptor. 
  ODBC_ARD* ard = new ODBC_ARD(this,NULL,SQL_DESC_ALLOC_USER); 
  if(ard)
  {
    if(m_descriptors.AddObject(ard))
    {
      *lpObject = ard;
      return SQL_SUCCESS;
    }
  }
  // Out of memory
  m_ociError.AddError("HY001");
  return SQL_ERROR;
}

SQLRETURN 
ODBCConnection::AllocDBStatement(ODBCStatement** lpObject)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();
	
	int i;
	for(i=0; i<DB_STATMENT_CACHE_COUNT; i++)
  {
		if(m_lpCacheStmtObjects[i])
		{
			if(m_lpCacheStmtObjects[i]->IsFreeInPool())
			{
				m_lpCacheStmtObjects[i]->BlockObjectInPool();
				*lpObject = m_lpCacheStmtObjects[i];
				return SQL_SUCCESS;
			}
		}
    else
		{
			break;
		}
  }
	OCIStmt *hStmtTemp;
	sword ociRes=OCI_SUCCESS;	
	
	ociRes = OCIHandleAlloc(m_lpEnv->GetOwnerEnv(),(dvoid**)&hStmtTemp,OCI_HTYPE_STMT,0,(dvoid**)NULL);
	if(ociRes!=OCI_SUCCESS)
	{		
		//ociError.GetErrorFromOCIHandle();
		if(ociRes!=OCI_SUCCESS_WITH_INFO)
    {
      return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
    }
	}
	*lpObject = new ODBCStatement(this,hStmtTemp);
  if(*lpObject)
  {
	  m_statements.AddObject(*lpObject);
  }
  else
  {
    // OUT OF MEMORY
    m_ociError.AddError("HY001");
    return SQL_ERROR;
  }
	if(i < DB_STATMENT_CACHE_COUNT)
	{
		// need to add object to statement pool
		m_lpCacheStmtObjects[i] = *lpObject;
		m_lpCacheStmtObjects[i]->SetPoolIndex(i);
		m_lpCacheStmtObjects[i]->BlockObjectInPool();
    m_lpCacheStmtObjects[i]->SetMetadataID(m_attributes.m_metaDataID == 1);
	}
	return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
}

bool
ODBCConnection::RemoveStatement(ODBCStatement* p_statement)
{
  int ind = p_statement->GetPoolIndex();
  if(ind >= 0)
  {
    // Statement was in pool. Clean up the statement pool
    m_lpCacheStmtObjects[ind] = NULL;
    // Move the rest down
    while((ind < (DB_STATMENT_CACHE_COUNT - 1)) &&
          m_lpCacheStmtObjects[ind +1])
    {
      m_lpCacheStmtObjects[ind] = m_lpCacheStmtObjects[ind+1];
      m_lpCacheStmtObjects[ind]->SetPoolIndex(ind);
      ++ind;
    }
    // Clean up the last in the queue (came down already)
    m_lpCacheStmtObjects[ind] = NULL;
  }
  return m_statements.RemoveObject((ODBCBaseObject*) p_statement);
}

SQLRETURN 
ODBCConnection::SQLEndTran(SQLSMALLINT CompletionType)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();
	
	sword ociRes = OCI_SUCCESS;
	
	if(CompletionType == SQL_COMMIT)
  {
    ociRes = OCITransCommit(m_hSvcCtx,m_ociError.GetOwnerErrorObject(),OCI_DEFAULT);
  }
	else if(CompletionType == SQL_ROLLBACK)
  {
    ociRes = OCITransRollback(m_hSvcCtx,m_ociError.GetOwnerErrorObject(),OCI_DEFAULT);
  }
  else
  {
    // Illegal completion type
    m_ociError.AddError("HY092");
    return SQL_ERROR;
  }
	if(ociRes != OCI_SUCCESS)
	{		
		m_ociError.GetErrorFromOCIHandle();
    return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
	}
	return SQL_SUCCESS;
}

bool
ODBCConnection::GetConnectionDead()
{
  ub4 status = OCI_SERVER_NOT_CONNECTED;

  // Get the server. Could be a different one!!
  OCIServer* server = NULL;
  if(OCIAttrGet(m_hSvcCtx
               ,OCI_HTYPE_SVCCTX
               ,&server
               ,0
               ,OCI_ATTR_SERVER
               ,m_ociError.GetOwnerErrorObject()) == OCI_SUCCESS)
  {
    if(OCIAttrGet(server
                 ,OCI_HTYPE_SERVER
                 ,&status
                 ,0
                 ,OCI_ATTR_SERVER_STATUS
                 ,m_ociError.GetOwnerErrorObject()) == OCI_SUCCESS)
    {
      if(status == OCI_SERVER_NORMAL)
      {
        // Server is still functioning.
        m_attributes.m_connectionDead = SQL_FALSE;
        return false;
      }
    }
  }
  // No server status: connection is dead
  m_attributes.m_connectionDead = SQL_TRUE;
  return true;
}

SQLRETURN 
ODBCConnection::SQLSetConnectAttr(SQLINTEGER Attribute
                                 ,SQLPOINTER Value
                                 ,SQLINTEGER StringLength)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  WRITELOG("Attribute: %s",ERROR_GetConnectAttribute(Attribute));
  SQLUINTEGER Val = (SQLUINTEGER)Value;
	switch(Attribute)
	{
  	case SQL_ATTR_ACCESS_MODE:          switch(Val)
			                                  {
  			                                  case SQL_MODE_READ_ONLY:				// Fall through
	  		                                  case SQL_MODE_READ_WRITE:  		  m_attributes.m_accessMode = Val;
				                                                                  break;
		  	                                  default:				                m_ociError.AddError("HY092");
				                                                                  return SQL_ERROR;
			                                  }
		                                    break;
	  case SQL_ATTR_ASYNC_ENABLE:         switch(Val)
			                                  {
			                                    case SQL_ASYNC_ENABLE_ON:				// Fall through
			                                    case SQL_ASYNC_ENABLE_OFF: 		  m_attributes.m_asyncEnable = Val;
				                                                                  break;
			                                    default:				                m_ociError.AddError("HY092");
				                                                                  return SQL_ERROR;
			                                  }
	  	                                  break;
    case SQL_ATTR_AUTO_IPD:             // Optional feature not implemented
                                        m_ociError.AddError("HYC00");
                                        return SQL_ERROR;
    case SQL_ATTR_AUTOCOMMIT:           switch(Val)
			                                  {
			                                    case SQL_AUTOCOMMIT_ON:				if(m_attributes.m_autoCommit == SQL_AUTOCOMMIT_OFF)
                                                                        {
                                                                          SQLEndTran(SQL_ROLLBACK);
                                                                        }
				                                                                m_attributes.m_autoCommit = SQL_AUTOCOMMIT_ON;			
				                                                                break;
			                                    case  SQL_AUTOCOMMIT_OFF: 		m_attributes.m_autoCommit = SQL_AUTOCOMMIT_OFF;
				                                                                break;
			                                    default:				              m_ociError.AddError("HY092");
				                                                                return SQL_ERROR;
			                                  }
  		                                  break;
	  case SQL_ATTR_CONNECTION_TIMEOUT:   m_attributes.m_connectionTimeOut = Val;
			                                  break;
  	case SQL_ATTR_CURRENT_CATALOG:			m_ociError.AddError("HY092");
			                                  return SQL_ERROR;
	  case SQL_ATTR_DISCONNECT_BEHAVIOR:  m_ociError.AddError("HY092");
			                                  return SQL_ERROR;
	  case SQL_ATTR_ENLIST_IN_DTC:			  m_ociError.AddError("HY092");
			                                  return SQL_ERROR;
	  case SQL_ATTR_ENLIST_IN_XA:			    m_ociError.AddError("HY092");
			                                  return SQL_ERROR;
  	case SQL_ATTR_LOGIN_TIMEOUT:			  m_attributes.m_loginTimeOut = Val;
			                                  break;
    case SQL_ATTR_METADATA_ID:          m_attributes.m_metaDataID = (Val == SQL_TRUE);
                                        break;
	  case SQL_ATTR_ODBC_CURSORS:         switch(Val)
			                                  {
			                                    case SQL_CUR_USE_IF_NEEDED:  m_ociError.AddError("HYC00");				
				                                                               return SQL_ERROR;
			                                    case SQL_CUR_USE_DRIVER:     m_attributes.m_ODBCCursor = SQL_CUR_USE_DRIVER;
				                                                               break;
			                                    case SQL_CUR_USE_ODBC:       m_ociError.AddError("HYC00");				
				                                                               return SQL_ERROR;
			                                    default:                     m_ociError.AddError("HY092");
				                                                               return SQL_ERROR;
		                                    }
		                                    break;
	  case SQL_ATTR_PACKET_SIZE: 			    m_attributes.m_packetSize = Val;
			                                  return SQL_ERROR;
	  case SQL_ATTR_QUIET_MODE:           // Main app HWND or NULL (Quiet)
                                        m_attributes.m_quietMode = Val;
			                                  break;
	  case SQL_ATTR_TRACE:                // DM does this!!
	  case SQL_ATTR_TRACEFILE:            // DM does this!!
                                        return SQL_ERROR;
	  case SQL_ATTR_TRANSLATE_LIB:        m_attributes.m_translateLib = MakeDriverString((SQLCHAR*)Value,StringLength);
                                        // Reload the library
                                        SetTranslationDLL();
                                        break;
	  case SQL_ATTR_TRANSLATE_OPTION:			m_attributes.m_translateOptions = Val;
			                                  break;
  	case SQL_ATTR_TXN_ISOLATION:				m_attributes.m_TXNIsolation = Val;
			                                  break;
  	case SQL_ATTR_CONNECTION_DEAD:      m_ociError.AddError("HY092");
			                                  return SQL_ERROR;
    case SQL_ATTR_ANSI_APP:             // ANSI and UNICODE are treated different in connection pooling!!
                                        m_attributes.m_ansiApplication = Val;
                                        break;
    // DRIVER SPECIFIC ATTRIBUTES
    case SQL_ATTR_EDO_ENSURE_STATISTICS:m_attributes.m_statisticsEnsure = Val;
                                        break;
    case SQL_ATTR_EDO_PREFECTH_COUNT:   m_attributes.m_prefetchCount  = (Val > 0) ? Val : 0;
                                        break;
    case SQL_ATTR_EDO_PREFETCH_MEMORY:  m_attributes.m_prefetchMemory = (Val > 0) ? Val : 0;
                                        break;
    case SQL_ATTR_EDO_FAILOVER_COUNT:   m_attributes.m_failoverCount  = (Val > 0) ? Val : 0;
                                        break;
    case SQL_ATTR_EDO_FAILOVER_TIME:    m_attributes.m_failoverTime   = (Val > 0) ? Val : 0;
                                        break;
    case SQL_ATTR_EDO_TRACE:            switch(Val)
                                        {
                                          case SQL_OPT_TRACE_OFF:	m_attributes.m_trace = SQL_OPT_TRACE_OFF;
                                                                  break;
                                          case SQL_OPT_TRACE_ON:  m_attributes.m_trace = SQL_OPT_TRACE_ON;
                                                                  break;
                                          default:                m_ociError.AddError("HY092");
                                                                  return SQL_ERROR;
                                        }
                                        break;
    case SQL_ATTR_EDO_TRACEFILE:        m_attributes.m_traceFile = MakeDriverString((SQLCHAR*)Value,StringLength);
                                        break;
    case SQL_ATTR_EDO_INTEGERSPEEDUP:   m_attributes.m_integerSpeedup = Val ? 1 : 0;
                                        break;
	  default:		                        m_ociError.AddError("HY092");
		                                    return SQL_ERROR;
	}
  WRITELOG("SetConnectAttr: OK");
  return SQL_SUCCESS;
}

SQLRETURN 
ODBCConnection::SQLGetConnectAttr(SQLINTEGER  Attribute
                                 ,SQLPOINTER  Value
                                 ,SQLINTEGER  BufferLength
                                 ,SQLINTEGER* StringLength)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();
  
  WRITELOG("SQLGetEnvAttr(%p,%s,%d,%p)",this,ERROR_GetConnectAttribute(Attribute),BufferLength,StringLength);
  SQLUINTEGER &Val=*((SQLUINTEGER*)Value);
  switch(Attribute)
	{
  	case SQL_ATTR_ACCESS_MODE:	      Val = m_attributes.m_accessMode;
			                                return SQL_SUCCESS;
	  case SQL_ATTR_ASYNC_ENABLE:		    Val = m_attributes.m_asyncEnable;
			                                return SQL_SUCCESS;
    case SQL_ATTR_AUTO_IPD:           Val = SQL_FALSE;
                                      return SQL_SUCCESS;
    case SQL_ATTR_AUTOCOMMIT:         Val = m_attributes.m_autoCommit;
			                                return SQL_SUCCESS;
	  case SQL_ATTR_CONNECTION_TIMEOUT: Val = m_attributes.m_connectionTimeOut;
			                                return SQL_SUCCESS;
    case SQL_ATTR_CURRENT_CATALOG:    CopyODBCStringToApp((SQLCHAR*)Value,(char*)m_attributes.m_currentCatalog.GetString(),BufferLength,(int*)StringLength,&m_ociError);
			                                return SQL_SUCCESS;
	  case SQL_ATTR_DISCONNECT_BEHAVIOR:// DISCONNECT ??
  	                                  m_ociError.AddError("HY092");
		                                  return SQL_ERROR;
	  case SQL_ATTR_ENLIST_IN_DTC:      Val = 0;
                                      // No driver support for connection pooling. Assume inexpensive
  	                                  return SQL_SUCCESS;
	  case SQL_ATTR_ENLIST_IN_XA:       // ENLIST_XA ??
		                                  m_ociError.AddError("HY092");
		                                  return SQL_ERROR;
	  case SQL_ATTR_LOGIN_TIMEOUT:      Val = m_attributes.m_loginTimeOut;
			                                return SQL_SUCCESS;
    case SQL_ATTR_METADATA_ID:        Val = m_attributes.m_metaDataID;
                                      return SQL_SUCCESS;
	  case SQL_ATTR_ODBC_CURSORS:       Val = m_attributes.m_ODBCCursor;
			                                return SQL_SUCCESS;
	  case SQL_ATTR_PACKET_SIZE:        Val = m_attributes.m_packetSize;
			                                return SQL_SUCCESS;
	  case SQL_ATTR_QUIET_MODE:         Val = m_attributes.m_quietMode;
			                                return SQL_SUCCESS;
	  case SQL_ATTR_TRACE:              // DM does this!!
	  case SQL_ATTR_TRACEFILE:          // DM does this!!
			                                return SQL_ERROR;
	  case SQL_ATTR_TRANSLATE_LIB:      CopyODBCStringToApp((SQLCHAR*)Value,(char*)m_attributes.m_translateLib.GetString(),BufferLength,(int*)StringLength,&m_ociError);
			                                return SQL_SUCCESS;
	  case SQL_ATTR_TRANSLATE_OPTION:   Val = m_attributes.m_translateOptions;
			                                return SQL_SUCCESS;
	  case SQL_ATTR_TXN_ISOLATION:      Val = m_attributes.m_TXNIsolation;
			                                return SQL_SUCCESS;
	  case SQL_ATTR_CONNECTION_DEAD:    GetConnectionDead();
                                      Val = m_attributes.m_connectionDead;
			                                return SQL_SUCCESS;
    case SQL_ATTR_ANSI_APP:           Val = m_attributes.m_ansiApplication;
                                      return SQL_SUCCESS;
    // DRIVER SPECIFIC ATTRIBUTES
    case SQL_ATTR_EDO_ENSURE_STATISTICS:Val = m_attributes.m_statisticsEnsure;
                                      return SQL_SUCCESS;
    case SQL_ATTR_EDO_PREFECTH_COUNT: Val = m_attributes.m_prefetchCount;
                                      return SQL_SUCCESS;
    case SQL_ATTR_EDO_PREFETCH_MEMORY:Val = m_attributes.m_prefetchMemory;
                                      return SQL_SUCCESS;
    case SQL_ATTR_EDO_FAILOVER_COUNT: Val = m_attributes.m_failoverCount;
                                      return SQL_SUCCESS;
    case SQL_ATTR_EDO_FAILOVER_TIME:  Val = m_attributes.m_failoverTime;
                                      return SQL_SUCCESS;
    case SQL_ATTR_EDO_TRACE:          Val = m_attributes.m_trace;
                                      return SQL_SUCCESS;
    case SQL_ATTR_EDO_TRACEFILE:      CopyODBCStringToApp((SQLCHAR*)Value,(char*)m_attributes.m_traceFile.GetString(),BufferLength,(int*)StringLength,&m_ociError);
                                      return SQL_SUCCESS;
    case SQL_ATTR_EDO_INTEGERSPEEDUP: Val = m_attributes.m_integerSpeedup;
                                      break;
	  default:		                      m_ociError.AddError("HY092",0,"Unknown Connection attribute");
		                                  break;
	}
  return SQL_ERROR;
}

SQLRETURN 
ODBCConnection::SQLGetConnectOption(SQLUSMALLINT Option
                                   ,SQLPOINTER   Value)
{
  switch(Option)
  {
    case SQL_ACCESS_MODE:       return SQLGetConnectAttr(SQL_ATTR_ACCESS_MODE,      Value,0,NULL);
    case SQL_AUTOCOMMIT:        return SQLGetConnectAttr(SQL_ATTR_AUTOCOMMIT,       Value,0,NULL);
    case SQL_ODBC_CURSORS:      return SQLGetConnectAttr(SQL_ATTR_ODBC_CURSORS,     Value,0,NULL);
    case SQL_OPT_TRACE:         return SQLGetConnectAttr(SQL_ATTR_TRACE,            Value,0,NULL);
    case SQL_OPT_TRACEFILE:     return SQLGetConnectAttr(SQL_ATTR_TRACEFILE,        Value,SQL_MAX_OPTION_STRING_LENGTH,NULL);
    case SQL_PACKET_SIZE:       return SQLGetConnectAttr(SQL_ATTR_PACKET_SIZE,      Value,0,NULL);
    case SQL_QUIET_MODE:        return SQLGetConnectAttr(SQL_ATTR_QUIET_MODE,       Value,0,NULL);
    case SQL_TRANSLATE_DLL:     return SQLGetConnectAttr(SQL_ATTR_TRANSLATE_LIB,    Value,SQL_MAX_OPTION_STRING_LENGTH,NULL);
    case SQL_TRANSLATE_OPTION:  return SQLGetConnectAttr(SQL_ATTR_TRANSLATE_OPTION, Value,0,NULL);
    default:                    m_ociError.AddError("HY024");
                                break;
  }
  return SQL_ERROR;
}

SQLRETURN
ODBCConnection::SQLSetConnectOption(SQLUSMALLINT Option ,SQLULEN Value)
{
  SQLPOINTER val = (SQLPOINTER)Value;
  switch(Option)
  {
    case SQL_ACCESS_MODE:       return SQLSetConnectAttr(SQL_ATTR_ACCESS_MODE,     val,0);
    case SQL_AUTOCOMMIT:        return SQLSetConnectAttr(SQL_ATTR_AUTOCOMMIT,      val,0);
    case SQL_ODBC_CURSORS:      return SQLSetConnectAttr(SQL_ATTR_ODBC_CURSORS,    val,0);
    case SQL_OPT_TRACE:         return SQLSetConnectAttr(SQL_ATTR_TRACE,           val,0);
    case SQL_OPT_TRACEFILE:     return SQLSetConnectAttr(SQL_ATTR_TRACEFILE,       val,SQL_NTS);
    case SQL_PACKET_SIZE:       return SQLSetConnectAttr(SQL_ATTR_PACKET_SIZE,     val,0);
    case SQL_QUIET_MODE:        return SQLSetConnectAttr(SQL_ATTR_QUIET_MODE,      val,0);
    case SQL_TRANSLATE_DLL:     return SQLSetConnectAttr(SQL_ATTR_TRANSLATE_LIB,   val,SQL_NTS);
    case SQL_TRANSLATE_OPTION:  return SQLSetConnectAttr(SQL_ATTR_TRANSLATE_OPTION,val,0);
    default:                    m_ociError.AddError("HY024");
    break;
  }
  return SQL_ERROR;
}

void
ODBCConnection::GetDriverOptions(CString& p_connectString)
{
  // Read our attributes from the registry
  m_attributes.ReadConnectionRegistry(m_dataSource.GetString());
  // The connection string is an override of the registry
  if(!p_connectString.IsEmpty())
  {
    m_attributes.ReadConnectionString(p_connectString);
  }
  // Now see to it that we use the new options
  // Load the translate library
  SetTranslationDLL();
  // Set the tracing file
  if(m_attributes.m_trace && !m_attributes.m_traceFile.IsEmpty())
  {
    ODBCDriver.OpenLog(m_attributes.m_traceFile.GetString());
  }
  else
  {
    ODBCDriver.CloseDriverLog();
  }
  GetDatabaseName();
  if(m_attributes.m_NLS_TYPE == 1)
  {
    GetNLSParameters();
  }
}

SQLRETURN 
ODBCConnection::SQLConnect(CString& p_dataSource
                          ,CString& p_userName
                          ,CString& p_authentication)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();
	
  char serverName[100] = "";
	m_ociError.SetDSNName(p_dataSource.GetString());

  if(!SQLGetPrivateProfileString(p_dataSource.GetString(),"ServerName","",serverName,100,ODBCINI))
  {
    m_ociError.AddError("HY000",156,"The Oracle server cannot be found for the selected DSN in the registry");
    return SQL_ERROR;
  }
  CWaitCursor TakeADeepSigh;
  sword ociRes = OCILogon(GetOwnerODBCEnv()->GetOwnerEnv()
                         ,m_ociError.GetOwnerErrorObject()
                         ,&m_hSvcCtx
			                   ,(OraText*)p_userName.      GetString(), p_userName.GetLength()
			                   ,(OraText*)p_authentication.GetString(), p_authentication.GetLength()
			                   ,(OraText*)serverName,     (int)strlen(serverName));
  if(ociRes == OCI_SUCCESS)
  {
    // Record our logged in server
    m_server     = serverName;
    m_user       = p_userName;
    m_dataSource = p_dataSource;
    // Now get our options
    CString nothing;
    GetDriverOptions(nothing);
  }
  else
  {
    m_ociError.GetErrorFromOCIHandle();
  }
  return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
}

SQLRETURN 
ODBCConnection::SQLDriverConnect(SQLHWND      hwnd
                                ,CString&     connectString
                                ,SQLCHAR*     szConnStrOut
                                ,SQLSMALLINT  cbConnStrOutMax
                                ,SQLSMALLINT* pcbConnStrOut
                                ,SQLUSMALLINT fDriverCompletion)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  char dataSource[SQL_MAX_DSN_LENGTH + 1] = "";
 	char serverName[100] = "";
  char userName  [100] = "";
  char passWord  [100] = "";
  bool prompt = false;

  if(!GetVariableFromConnectionString(connectString,"DSN=",dataSource,SQL_MAX_DSN_LENGTH))
  {
    m_ociError.AddError("HY000",156,"DSN not set");
    return SQL_ERROR;
  }
  m_ociError.SetDSNName(dataSource);

  if(!SQLGetPrivateProfileString(dataSource,"DSN","",dataSource,100,ODBCINI))
  {
    m_ociError.AddError("HY000",156,"Selected DSN cannot be founded as a defined datasource in the registry");
    return SQL_ERROR;
  }
  if(!SQLGetPrivateProfileString(dataSource,"ServerName","",serverName,100,ODBCINI))
  {
    m_ociError.AddError("HY000",156,"The Oracle server cannot be found for the selected DSN in the registry");
    return SQL_ERROR;
  }
  // Get UID
  if(!GetVariableFromConnectionString(connectString,"UID=",userName,100))
  {
    if(!SQLGetPrivateProfileString(dataSource,"UID","",userName,100,ODBCINI))
    {
       prompt = true;
    }
  }
  if(!GetVariableFromConnectionString(connectString ,"PWD=",passWord,100))
  {
    if(!SQLGetPrivateProfileString(dataSource,"PWD","",passWord,100,ODBCINI))
    {
      prompt = true;
    }
  }
  if(fDriverCompletion == SQL_DRIVER_PROMPT)
  {
    prompt = true;
  }
  if(prompt)
  {
    if(fDriverCompletion == SQL_DRIVER_NOPROMPT)
    {
      // Cannot/may not prompt 
      // Error: missing UID or PWD
      m_ociError.AddError("HY000",156,"Missing user/password combination");
      return SQL_ERROR;
    }
    // No special meaning for this driver
    // fDriverCompletion == SQL_DRIVER_COMPLETE 
    // fDriverCompletion == SQL_DRIVER_COMPLETE_REQUIRED
    CWnd* parent = CWnd::FromHandle(hwnd);
    LogonDlg dlg(parent,dataSource,userName,passWord);
    if(dlg.DoModal() == IDOK)
    {
      strncpy(userName,dlg.GetUser().GetString(),100);
      strncpy(passWord,dlg.GetPassword().GetString(),100);
    }
    else
    {
      m_ociError.AddError("HY000",156,"No user or password givven");
      return SQL_ERROR;
    }
  }
  // Result is empty
  if(szConnStrOut)
  {
    szConnStrOut[0] = 0;
  }
  if(pcbConnStrOut)
  {
    *pcbConnStrOut  = 0;
  }
  CWaitCursor TakeADeepSigh;
  // Do the logon for OCI
	sword ociRes = OCILogon(GetOwnerODBCEnv()->GetOwnerEnv()
                         ,m_ociError.GetOwnerErrorObject()
                         ,&m_hSvcCtx
		                     ,(OraText*)userName,  (int)strlen(userName)
		                     ,(OraText*)passWord,  (int)strlen(passWord)
			                   ,(OraText*)serverName,(int)strlen(serverName));
	if(ociRes == OCI_SUCCESS)
  {
    // Register our server
    m_dataSource = dataSource;
    m_server     = serverName;
    m_user       = userName;

    // Now get our options
    GetDriverOptions(connectString);

    if(szConnStrOut)
    {
      // Return a complete connection string with all resulting attributes
      CString resultString = m_attributes.MakeConnectionString();
      int len = 0;
      CopyODBCStringToApp(szConnStrOut,(char*)resultString.GetString(),cbConnStrOutMax,&len,&m_ociError);
      if(pcbConnStrOut)
      {
        *pcbConnStrOut = (SQLSMALLINT)len;
      }
    }
  }
  else
  {
    m_ociError.GetErrorFromOCIHandle();
  }
	return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
}

// MULTILEVEL DISCOVERY does not make a lot of sense on the Oracle platform :-(
// An instance and a database are one and the same. not a lot too choose between.
// Implement more than one TNS connection in the server string
SQLRETURN 
ODBCConnection::SQLBrowseConnect(CString      connectString
                                ,SQLCHAR*     szConnStrOut
                                ,SQLSMALLINT  cbConnStrOutMax
                                ,SQLSMALLINT* pcbConnStrOut)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  char dataSource[SQL_MAX_DSN_LENGTH + 1] = "";
  char serverName[100] = "";
  char userName  [100] = "";
  char passWord  [100] = "";

  if(m_browseConnect == 0)
  {
    // First time here. Must have DSN at least from the DM.
    ++m_browseConnect;
    if(!GetVariableFromConnectionString(connectString,"DSN=",dataSource,SQL_MAX_DSN_LENGTH))
    {
      m_ociError.AddError("HY000",156,"DSN not set");
      return SQL_ERROR;
    }
    if(!SQLGetPrivateProfileString(dataSource,"ServerName","",serverName,100,ODBCINI))
    {
      m_ociError.AddError("HY000",156,"The Oracle server cannot be found for the selected DSN in the registry");
      return SQL_ERROR;
    }
    int len = 0;
    CString outstring = CString("SERVER:Database=") + serverName + ";UID:User=?;PWD:Password=?;";
    CopyODBCStringToApp(szConnStrOut,outstring.GetBuffer(),cbConnStrOutMax,&len,&m_ociError);
    if(pcbConnStrOut)
    {
      *pcbConnStrOut = (SQLSMALLINT)len;
    }
    // Application: do your stuff!
    return SQL_NEED_DATA;
  }
  if(!GetVariableFromConnectionString(connectString,"SERVER=",serverName,100))
  {
    m_ociError.AddError("HY000",156,"Database server not set");
    return SQL_ERROR;
  }
  // Get UID
  if(!GetVariableFromConnectionString(connectString,"UID=",userName,100))
  {
    if(!SQLGetPrivateProfileString(dataSource,"UID","",userName,100,ODBCINI))
    {
      m_ociError.AddError("HY000",156,"User identity not set");
      return SQL_ERROR;
    }
  }
  if(!GetVariableFromConnectionString(connectString ,"PWD=",passWord,100))
  {
    if(!SQLGetPrivateProfileString(dataSource,"PWD","",passWord,100,ODBCINI))
    {
      m_ociError.AddError("HY000",156,"User password not set");
      return SQL_ERROR;
    }
  }
  // Do the logon for OCI
  CWaitCursor TakeADeepSigh;
  sword ociRes = OCILogon(GetOwnerODBCEnv()->GetOwnerEnv()
                         ,m_ociError.GetOwnerErrorObject()
                         ,&m_hSvcCtx
                         ,(OraText*)userName,  (int)strlen(userName)
                         ,(OraText*)passWord,  (int)strlen(passWord)
                         ,(OraText*)serverName,(int)strlen(serverName));
  if(ociRes == OCI_SUCCESS)
  {
    // Register our server
    m_dataSource = dataSource;
    m_server     = serverName;
    m_user       = userName;

    // Now get our options
    GetDriverOptions(connectString);

    if(szConnStrOut)
    {
      int len = 0;
      CString resultString = m_attributes.MakeConnectionString();
      CopyODBCStringToApp(szConnStrOut,(char*)resultString.GetString(),cbConnStrOutMax,&len,&m_ociError);
      if(pcbConnStrOut)
      {
        *pcbConnStrOut = (SQLSMALLINT)len;
      }
    }
  }
  else
  {
    m_ociError.GetErrorFromOCIHandle();
  }
  return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
}

SQLRETURN ODBCConnection::SQLDisconnect()
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();
	
  WRITELOG("SQLDisconnect(%p)",this);
	m_statements.RemoveAllObjects();
	
	int i;
	for(i=0; i<DB_STATMENT_CACHE_COUNT; i++)
  {
    m_lpCacheStmtObjects[i] = NULL;		
  }
	sword ociRes = OCI_SUCCESS;
	if(m_hSvcCtx)
	{
		ociRes = OCILogoff(m_hSvcCtx,m_ociError.GetOwnerErrorObject());
		if(ociRes != OCI_SUCCESS)
    {
      m_ociError.GetErrorFromOCIHandle();
    }
		else
    {
      m_user   = "";
      m_server = "";
      m_dataSource = "";
      m_ociError.SetDSNName("");
      OCIHandleFree(m_hSvcCtx,OCI_HTYPE_SVCCTX);
      m_hSvcCtx = NULL;
    }
	}
	return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
}

char*
ODBCConnection::GetCharset()
{
  ub2 charsetID = 0;
  static oratext buffer[32];
  buffer[0] = 0;
  if(OCIAttrGet(GetOwnerODBCEnv()->GetOwnerEnv()
               ,(ub4)OCI_HTYPE_ENV
               ,&charsetID
               ,(ub4*)0
               ,OCI_ATTR_ENV_CHARSET_ID
               ,m_ociError.GetOwnerErrorObject()) == OCI_SUCCESS)
  {
    OCINlsCharSetIdToName(GetOwnerODBCEnv()->GetOwnerEnv(),buffer,32,charsetID);
  }
  return (char*)buffer;
}

SQLRETURN
ODBCConnection::GetOracleVersion()
{
  OraText serverString[512];
  char versionBuffer[32];
  // Get the server version string, something like:
  // Oracle8i Enterprise Edition Release 8.1.5.0.0 - Production
  // With the Partitioning and Java options
  // PL/SQL Release 8.1.5.0.0 - Production
  if(OCIServerVersion(m_hSvcCtx,m_ociError.GetOwnerErrorObject()
                     ,serverString
                     ,(ub4)512
                     ,OCI_HTYPE_SVCCTX) == OCI_SUCCESS)
  {
    int   version  = 0;
    int   major    = 0;
    int   minor    = 0;
    int   patch    = 0;
    int   subpatch = 0;

    char* pnt = (char*)serverString;
    while(*pnt && *pnt != ' ') ++pnt;
    while(*pnt && !isdigit(*pnt)) ++pnt;
    version = atoi(pnt);
    while(*pnt && isdigit(*pnt)) ++pnt;
    if(*pnt == '.') ++pnt;
    major = atoi(pnt);
    while(*pnt && isdigit(*pnt)) ++pnt;
    if(*pnt == '.') ++pnt;
    minor = atoi(pnt);
    while(*pnt && isdigit(*pnt)) ++pnt;
    if(*pnt == '.') ++pnt;
    patch = atoi(pnt);
    while(*pnt && isdigit(*pnt)) ++pnt;
    if(*pnt == '.') ++pnt;
    subpatch = atoi(pnt);

    sprintf_s(versionBuffer,32,"%02.2d.%02.2d.0%d%d%d",version,major,minor,patch,subpatch);
    m_oracleVersion = versionBuffer;
    return SQL_SUCCESS;
  }
  return SQL_ERROR;
}

#define EDO_SQL_CVT_NUMMERIC SQL_CVT_BIGINT|SQL_CVT_BIT|SQL_CVT_CHAR|SQL_CVT_DECIMAL|\
                             SQL_CVT_DOUBLE|SQL_CVT_FLOAT|SQL_CVT_INTEGER|SQL_CVT_NUMERIC|\
                             SQL_CVT_REAL|SQL_CVT_SMALLINT|SQL_CVT_TINYINT|SQL_CVT_VARCHAR|SQL_CVT_CHAR
#define EDO_SQL_CVT_DATE     SQL_CVT_CHAR|SQL_CVT_DATE|SQL_CVT_TIMESTAMP|SQL_CVT_VARCHAR

SQLRETURN 
ODBCConnection::SQLGetInfo(SQLUSMALLINT InfoType
                          ,SQLPOINTER   InfoValue
                          ,SQLSMALLINT  BufferLength
                          ,SQLSMALLINT* StringLength)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  SQLRETURN   result = SQL_ERROR;
  char*       string = NULL;
  SQLUSMALLINT  tiny = 0xFF;
  SQLUINTEGER number = 0xFFFF;

  switch(InfoType)
	{		
    case SQL_ACCESSIBLE_PROCEDURES:           // Fall through
    case SQL_ACCESSIBLE_TABLES:               string = "Y";
                                              break;
    case SQL_ACTIVE_ENVIRONMENTS:             tiny = 1;
                                              break;
    case SQL_AGGREGATE_FUNCTIONS:             number = SQL_AF_ALL;
                                              break;
    case SQL_ALTER_DOMAIN:                    number = 0;
                                              break;
    case SQL_ALTER_TABLE:                     number = SQL_AT_ADD_COLUMN                      |
                                                       SQL_AT_DROP_COLUMN                     |
                                                     //SQL_AT_ADD_COLUMN_COLLATION 
                                                       SQL_AT_ADD_COLUMN_DEFAULT              | 
                                                       SQL_AT_ADD_COLUMN_SINGLE               |
                                                       SQL_AT_ADD_CONSTRAINT                  |
                                                       SQL_AT_ADD_TABLE_CONSTRAINT            |
                                                       SQL_AT_CONSTRAINT_NAME_DEFINITION      |
                                                     //SQL_AT_DROP_COLUMN_CASCADE 
                                                       SQL_AT_DROP_COLUMN_DEFAULT             |
                                                     //SQL_AT_DROP_COLUMN_RESTRICT 
                                                       SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE   |
                                                       SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT  |
                                                       SQL_AT_SET_COLUMN_DEFAULT              |
                                                       SQL_AT_CONSTRAINT_INITIALLY_DEFERRED   |
                                                       SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE  |
                                                       SQL_AT_CONSTRAINT_DEFERRABLE           |
                                                       SQL_AT_CONSTRAINT_NON_DEFERRABLE;
                                              break;
    case SQL_ASYNC_MODE:                      number = SQL_AM_NONE;
                                              break;
    case SQL_BATCH_ROW_COUNT:                 number = SQL_BRC_EXPLICIT;
                                              break;
    case SQL_BATCH_SUPPORT:                   number = SQL_BS_SELECT_EXPLICIT |
                                                       SQL_BS_SELECT_PROC;
    case SQL_BOOKMARK_PERSISTENCE:            number = SQL_BP_CLOSE | 
                                                       SQL_BP_DROP  | 
                                                       SQL_BP_TRANSACTION |
                                                       SQL_BP_UPDATE |
                                                       SQL_BP_OTHER_HSTMT;
                                              break;
    case SQL_CATALOG_LOCATION:                tiny = SQL_CL_END;
                                              break;
    case SQL_CATALOG_NAME:                    string = "Y";
                                              break;
    case SQL_CATALOG_NAME_SEPARATOR:          string = "@";
                                              break;
    case SQL_CATALOG_TERM:                    string = "Database link";
                                              break;
    case SQL_CATALOG_USAGE:                   number = SQL_CU_DML_STATEMENTS | 
                                                       SQL_CU_PROCEDURE_INVOCATION;
                                                    // SQL_CU_TABLE_DEFINITION
                                                    // SQL_CU_INDEX_DEFINITION 
                                                    // SQL_CU_PRIVILEGE_DEFINITION 
                                              break;
    case SQL_COLLATION_SEQ:                   string = GetCharset();
                                              break;
    case SQL_COLUMN_ALIAS:                    string = "Y";
                                              break;
    case SQL_CONCAT_NULL_BEHAVIOR:            tiny = SQL_CB_NON_NULL;
                                              break;
    case SQL_CONVERT_BINARY:                  number = 0;
                                              break;
    case SQL_CONVERT_CHAR:
    case SQL_CONVERT_VARCHAR:                 number = EDO_SQL_CVT_NUMMERIC | 
                                                       SQL_CVT_DATE         |
                                                       SQL_CVT_TIMESTAMP;
                                              break;
    case SQL_CONVERT_BIGINT:                  
    case SQL_CONVERT_BIT:                     
    case SQL_CONVERT_DECIMAL:
    case SQL_CONVERT_DOUBLE:
    case SQL_CONVERT_INTEGER:
    case SQL_CONVERT_LONGVARBINARY:
    case SQL_CONVERT_LONGVARCHAR:
    case SQL_CONVERT_NUMERIC:
    case SQL_CONVERT_REAL:
    case SQL_CONVERT_FLOAT:
    case SQL_CONVERT_SMALLINT:
    case SQL_CONVERT_TINYINT:
    case SQL_CONVERT_VARBINARY:               number = EDO_SQL_CVT_NUMMERIC;
                                              break;
    case SQL_CONVERT_DATE:
    case SQL_CONVERT_INTERVAL_YEAR_MONTH:
    case SQL_CONVERT_INTERVAL_DAY_TIME:
    case SQL_CONVERT_TIME:
    case SQL_CONVERT_TIMESTAMP:               number = EDO_SQL_CVT_DATE;
                                              break;
    case SQL_CONVERT_FUNCTIONS:               tiny = SQL_FN_CVT_CAST | SQL_FN_CVT_CONVERT;
                                              break;
    case SQL_CORRELATION_NAME:                tiny = SQL_CN_ANY;
                                              break;
    case SQL_CREATE_ASSERTION:                tiny = 0;
                                              break;
    case SQL_CREATE_CHARACTER_SET:            number = 0;
                                              break;
    case SQL_CREATE_COLLATION:                number = 0;
                                              break;
    case SQL_CREATE_DOMAIN:                   number = 0;
                                              break;
    case SQL_CREATE_SCHEMA:                   number = SQL_CS_CREATE_SCHEMA | 
                                                       SQL_CS_AUTHORIZATION;
                                                    // SQL_CS_DEFAULT_CHARACTER_SET
                                              break;
    case SQL_CREATE_TABLE:                    number =  SQL_CT_CREATE_TABLE                   |
                                                        SQL_CT_TABLE_CONSTRAINT               |
                                                        SQL_CT_CONSTRAINT_NAME_DEFINITION     |
                                                        SQL_CT_COMMIT_PRESERVE                |
                                                        SQL_CT_COMMIT_DELETE                  |
                                                        SQL_CT_GLOBAL_TEMPORARY               |
                                                      //SQL_CT_LOCAL_TEMPORARY
                                                        SQL_CT_COLUMN_CONSTRAINT              |
                                                        SQL_CT_COLUMN_DEFAULT                 |
                                                      //SQL_CT_COLUMN_COLLATION 
                                                        SQL_CT_CONSTRAINT_INITIALLY_DEFERRED  |
                                                        SQL_CT_CONSTRAINT_INITIALLY_IMMEDIATE |
                                                        SQL_CT_CONSTRAINT_DEFERRABLE          |
                                                        SQL_CT_CONSTRAINT_NON_DEFERRABLE ;
                                              break;
    case SQL_CREATE_TRANSLATION:              number = 0;
                                              break;
    case SQL_CREATE_VIEW:                     number = SQL_CV_CREATE_VIEW | SQL_CV_CHECK_OPTION;
                                              break;
	  case SQL_CURSOR_COMMIT_BEHAVIOR:			    tiny = SQL_CB_PRESERVE;
		                                          break;
    case SQL_CURSOR_ROLLBACK_BEHAVIOR:			  tiny = SQL_CB_PRESERVE;
		                                          break;			
    case SQL_CURSOR_SENSITIVITY:              number = SQL_INSENSITIVE;
                                              break;
    case SQL_DATA_SOURCE_NAME:                string = (char*)m_dataSource.GetString();
                                              break;
    case SQL_DATA_SOURCE_READ_ONLY:           string = "N";
                                              break;
    case SQL_DATABASE_NAME:                   string = (char*)m_server.GetString();
                                              break;
    case SQL_DATETIME_LITERALS:               number = SQL_DL_SQL92_DATE      |
                                                       SQL_DL_SQL92_TIME      |
                                                       SQL_DL_SQL92_TIMESTAMP |
                                                     //SQL_DL_SQL92_INTERVAL_YEAR
                                                     //SQL_DL_SQL92_INTERVAL_MONTH
                                                     //SQL_DL_SQL92_INTERVAL_DAY
                                                     //SQL_DL_SQL92_INTERVAL_HOUR
                                                     //SQL_DL_SQL92_INTERVAL_MINUTE
                                                     //SQL_DL_SQL92_INTERVAL_SECOND
                                                       SQL_DL_SQL92_INTERVAL_YEAR_TO_MONTH |
                                                     //SQL_DL_SQL92_INTERVAL_DAY_TO_HOUR
                                                     //SQL_DL_SQL92_INTERVAL_DAY_TO_MINUTE
                                                       SQL_DL_SQL92_INTERVAL_DAY_TO_SECOND;
                                                     //SQL_DL_SQL92_INTERVAL_HOUR_TO_MINUTE
                                                     //SQL_DL_SQL92_INTERVAL_HOUR_TO_SECOND
                                                     //SQL_DL_SQL92_INTERVAL_MINUTE_TO_SECOND
                                              break;
    case SQL_DBMS_NAME:                       string = "Oracle";
                                              break;
    case SQL_DBMS_VER:                        string = "";
                                              if(GetOracleVersion() == SQL_SUCCESS)
                                              {
                                                string = (char*)m_oracleVersion.GetString();
                                              }
                                              break;
    case SQL_DDL_INDEX:                       number = SQL_DI_CREATE_INDEX | SQL_DI_DROP_INDEX;
                                              break;
    case SQL_DEFAULT_TXN_ISOLATION:           number = SQL_TXN_READ_COMMITTED;
                                              break;
    case SQL_DESCRIBE_PARAMETER:              string = "Y";
                                              break;
  //case SQL_DM_VER:                          IMPLEMENTED BY THE DM !!
  //case SQL_DRIVER_HDBC:                     IMPLEMENTED BY THE DM !!
  //case SQL_DRIVER_HENV:                     IMPLEMENTED BY THE DM !!
  //case SQL_DRIVER_HDESC:                    IMPLEMENTED BY THE DM !!
  //case SQL_DRIVER_HLIB:                     IMPLEMENTED BY THE DM !!
  //case SQL_DRIVER_HSTMT:                    IMPLEMENTED BY THE DM !!
    case SQL_DRIVER_NAME:                     string = (char*)m_attributes.m_driverName.GetString();
                                              break;
    case SQL_DRIVER_ODBC_VER:                 string = "03.52";
                                              break;
    case SQL_DRIVER_VER:			                string = "01.00.0000";
		                                          break;
    case SQL_DROP_ASSERTION:                  // Fall through
    case SQL_DROP_CHARACTER_SET:              // Fall through
    case SQL_DROP_COLLATION:                  // Fall through
    case SQL_DROP_DOMAIN:                     number = 0;
                                              break;
    case SQL_DROP_SCHEMA:                     number = SQL_DS_DROP_SCHEMA | SQL_DS_CASCADE | SQL_DS_RESTRICT;
                                              break;
    case SQL_DROP_TABLE:                      number = SQL_DT_DROP_TABLE | SQL_DT_CASCADE | SQL_DT_RESTRICT;
                                              break;
    case SQL_DROP_TRANSLATION:                number = 0;
                                              break;
    case SQL_DROP_VIEW:                       number = SQL_DV_DROP_VIEW;
                                                     //SQL_DV_CASCADE 
                                                     //SQL_DV_RESTRICT;
                                              break;
    case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:      // EDO: TODO
    case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:      number = 0;
                                              break;
    case SQL_EXPRESSIONS_IN_ORDERBY:          string = "Y";
                                              break;
    case SQL_FILE_USAGE:                      tiny = SQL_FILE_NOT_SUPPORTED;
                                              break;
    case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1: // EDO: TODO
    case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2: number = 0;
                                              break;
    case SQL_GETDATA_EXTENSIONS:              number = SQL_GD_ANY_COLUMN |
                                                       SQL_GD_ANY_ORDER  |
                                                     //SQL_GD_BLOCK        // We do not have row blocks yet
                                                       SQL_GD_BOUND;
                                              break;
    case SQL_GROUP_BY:                        tiny = SQL_GB_GROUP_BY_EQUALS_SELECT;
                                              break;
    case SQL_IDENTIFIER_CASE:                 tiny = SQL_IC_UPPER;
                                              break;
    case SQL_IDENTIFIER_QUOTE_CHAR:           string = "\"";
                                              break;
    case SQL_INDEX_KEYWORDS:                  number = SQL_IK_ALL;
                                              break;
    case SQL_INFO_SCHEMA_VIEWS:               number =  // SQL_ISV_ASSERTIONS 
                                                        // SQL_ISV_CHARACTER_SETS 
                                                        // SQL_ISV_CHECK_CONSTRAINTS
                                                        // SQL_ISV_COLLATIONS 
                                                        // SQL_ISV_COLUMN_DOMAIN_USAGE 
                                                           SQL_ISV_COLUMN_PRIVILEGES |
                                                        // SQL_ISV_COLUMNS 
                                                        // SQL_ISV_CONSTRAINT_COLUMN_USAGE 
                                                        // SQL_ISV_CONSTRAINT_TABLE_USAGE 
                                                        // SQL_ISV_DOMAIN_CONSTRAINTS 
                                                        // SQL_ISV_DOMAINS 
                                                        // SQL_ISV_KEY_COLUMN_USAGE 
                                                        // SQL_ISV_REFERENTIAL_CONSTRAINTS 
                                                        // SQL_ISV_SCHEMATA 
                                                        // SQL_ISV_SQL_LANGUAGES 
                                                        // SQL_ISV_TABLE_CONSTRAINTS 
                                                           SQL_ISV_TABLE_PRIVILEGES ;
                                                        // SQL_ISV_TABLES 
                                                        // SQL_ISV_TRANSLATIONS 
                                                        // SQL_ISV_USAGE_PRIVILEGES 
                                                        // SQL_ISV_VIEW_COLUMN_USAGE
                                                        // SQL_ISV_VIEW_TABLE_USAGE 
                                                        // SQL_ISV_VIEWS 
                                              break;
    case SQL_INSERT_STATEMENT:                number = SQL_IS_INSERT_LITERALS |
                                                       SQL_IS_INSERT_SEARCHED |
                                                       SQL_IS_SELECT_INTO;
                                              break;
    case SQL_INTEGRITY:                       string = "Y";
                                              break;
    case SQL_KEYSET_CURSOR_ATTRIBUTES1:       // EDO: TODO
    case SQL_KEYSET_CURSOR_ATTRIBUTES2:       number = 0;
                                              break;
    case SQL_KEYWORDS:                        string = databaseKeywords;
                                              break;
    case SQL_LIKE_ESCAPE_CLAUSE:              string = "Y";
                                              break;
    case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS: number = 0;
                                              break;
    case SQL_MAX_CHAR_LITERAL_LEN:            number = 4000;
                                              break;
    // Max identifier lengths
    case SQL_MAX_CATALOG_NAME_LEN:            // Fall through
    case SQL_MAX_COLUMN_NAME_LEN:             // Fall through
    case SQL_MAX_CURSOR_NAME_LEN:             // Fall through
    case SQL_MAX_IDENTIFIER_LEN:              // Fall through
    case SQL_MAX_PROCEDURE_NAME_LEN:          // Fall through
    case SQL_MAX_SCHEMA_NAME_LEN:             // Fall through
    case SQL_MAX_TABLE_NAME_LEN:              // Fall through
    case SQL_MAX_USER_NAME_LEN:               tiny = 30;
                                              break;
    // No real implementation constraint
    case SQL_MAX_BINARY_LITERAL_LEN:          // Fall through
    case SQL_MAX_INDEX_SIZE:                  // Fall through
    case SQL_MAX_ROW_SIZE:                    // Fall through
    case SQL_MAX_STATEMENT_LEN:               number = 0;
                                              break;
    case SQL_MAX_COLUMNS_IN_SELECT:           // Fall through
    case SQL_MAX_COLUMNS_IN_TABLE:            tiny = 1000;
                                              break;
    case SQL_MAX_COLUMNS_IN_GROUP_BY:         // Fall through
    case SQL_MAX_COLUMNS_IN_INDEX:            // Fall through
    case SQL_MAX_COLUMNS_IN_ORDER_BY:         // Fall through
    case SQL_MAX_CONCURRENT_ACTIVITIES:       // Fall through
    case SQL_MAX_DRIVER_CONNECTIONS:          // Fall through
    case SQL_MAX_TABLES_IN_SELECT:            tiny = 0;
                                              break;
    case SQL_MAX_ROW_SIZE_INCLUDES_LONG:      string = "Y";
                                              break;
    case SQL_MULT_RESULT_SETS:                string = "Y";
                                              break;
    case SQL_MULTIPLE_ACTIVE_TXN:             string = "Y";
                                              break;
    case SQL_NEED_LONG_DATA_LEN:              string = "Y";
                                              break;
    case SQL_NON_NULLABLE_COLUMNS:            tiny = SQL_NNC_NON_NULL;
                                              break;
    case SQL_NULL_COLLATION:                  tiny = SQL_NC_HIGH;
                                              break;
    case SQL_NUMERIC_FUNCTIONS:               number = SQL_FN_NUM_ABS     |
                                                       SQL_FN_NUM_ACOS    |
                                                       SQL_FN_NUM_ASIN    |
                                                       SQL_FN_NUM_ATAN    |
                                                       SQL_FN_NUM_ATAN2   |
                                                       SQL_FN_NUM_CEILING |
                                                       SQL_FN_NUM_COS     |
                                                       SQL_FN_NUM_COT     |
                                                       SQL_FN_NUM_DEGREES | 
                                                       SQL_FN_NUM_EXP     |
                                                       SQL_FN_NUM_FLOOR   |
                                                       SQL_FN_NUM_LOG     |
                                                       SQL_FN_NUM_LOG10   |
                                                       SQL_FN_NUM_MOD     |
                                                       SQL_FN_NUM_PI      |
                                                       SQL_FN_NUM_POWER   |
                                                       SQL_FN_NUM_RADIANS |
                                                     //SQL_FN_NUM_RAND
                                                       SQL_FN_NUM_ROUND   |
                                                       SQL_FN_NUM_SIGN    |
                                                       SQL_FN_NUM_SIN     |
                                                       SQL_FN_NUM_SQRT    |
                                                       SQL_FN_NUM_TAN     |
                                                       SQL_FN_NUM_TRUNCATE;
                                              break;
    case SQL_ODBC_INTERFACE_CONFORMANCE:      number = SQL_OIC_LEVEL2;
                                              break;
  //case SQL_ODBC_VER:                        IMPLEMENTED BY THE DM !!
    case SQL_OJ_CAPABILITIES:                 number = SQL_OJ_LEFT         | // = Left outer joins are supported.
                                                       SQL_OJ_RIGHT        | // = Right outer joins are supported.
                                                       SQL_OJ_FULL         | // = Full outer joins are supported.
                                                       SQL_OJ_NESTED       | // = Nested outer joins are supported.
                                                       SQL_OJ_NOT_ORDERED  | // = The column names in the ON clause of the outer join do not have to be in the same order as their respective table names in the OUTER JOIN clause.
                                                       SQL_OJ_INNER        ; // = The inner table (the right table in a left outer join or the left table in a right outer join) can also be used in an inner join. This does not apply to full outer joins, which do not have an inner table.
                                                     //SQL_OJ_ALL_COMPARISON_OPS = The comparison operator in the ON clause can be any of the ODBC comparison operators. If this bit is not set, only the equals (=) comparison operator can be used in outer joins.
                                              break;
    case SQL_ORDER_BY_COLUMNS_IN_SELECT:      string = "N";
                                              break;
    case SQL_OUTER_JOINS:                     string = "Y";
                                              break;
    case SQL_PARAM_ARRAY_ROW_COUNTS:          number = SQL_PARC_NO_BATCH;
                                              break;
    case SQL_PARAM_ARRAY_SELECTS:             number = SQL_PAS_NO_BATCH;
                                              break;
    case SQL_PROCEDURE_TERM:                  string = "Procedure";
                                              break;
    case SQL_PROCEDURES:                      string = "Y";
                                              break;
    case SQL_QUOTED_IDENTIFIER_CASE:          tiny = SQL_IC_SENSITIVE;
                                              break;
    case SQL_ROW_UPDATES:                     string = "Y";
                                              break;
    case SQL_SCHEMA_TERM:                     string = "Owner";
                                              break;
    case SQL_SCHEMA_USAGE:                    number = SQL_SU_DML_STATEMENTS       |
                                                       SQL_SU_PROCEDURE_INVOCATION |
                                                       SQL_SU_TABLE_DEFINITION     |
                                                       SQL_SU_INDEX_DEFINITION     |
                                                       SQL_SU_PRIVILEGE_DEFINITION ;
                                              break;
    case SQL_SCROLL_OPTIONS:                  number = SQL_SO_FORWARD_ONLY;
                                              break;
    case SQL_SEARCH_PATTERN_ESCAPE:           string = "% and '_'";
                                              break;
    case SQL_SERVER_NAME:                     string = (char*)m_server.GetString();
                                              break;
    case SQL_SPECIAL_CHARACTERS:              string = "$#";
                                              break;
    case SQL_SQL_CONFORMANCE:                 number = SQL_SC_SQL92_INTERMEDIATE;
                                              break;
    case SQL_SQL92_DATETIME_FUNCTIONS:        number = SQL_SDF_CURRENT_DATE |
                                                       SQL_SDF_CURRENT_TIME |
                                                       SQL_SDF_CURRENT_TIMESTAMP;
                                              break;
    case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:   number = SQL_SFKD_CASCADE     |
                                                       SQL_SFKD_NO_ACTION   |
                                                       SQL_SFKD_SET_DEFAULT |
                                                       SQL_SFKD_SET_NULL    ;
                                              break;
    case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:   number = SQL_SFKU_CASCADE     |
                                                       SQL_SFKU_NO_ACTION   |
                                                       SQL_SFKU_SET_DEFAULT |
                                                       SQL_SFKU_SET_NULL    ;
                                              break;
    case SQL_SQL92_GRANT:                     number = SQL_SG_DELETE_TABLE      |
                                                       SQL_SG_INSERT_COLUMN     |
                                                       SQL_SG_INSERT_TABLE      |
                                                       SQL_SG_REFERENCES_TABLE  |
                                                       SQL_SG_REFERENCES_COLUMN |
                                                       SQL_SG_SELECT_TABLE      |
                                                       SQL_SG_UPDATE_COLUMN     |
                                                       SQL_SG_UPDATE_TABLE      |
                                                     //SQL_SG_USAGE_ON_DOMAIN (FIPS Transitional level)
                                                     //SQL_SG_USAGE_ON_CHARACTER_SET (FIPS Transitional level)
                                                     //SQL_SG_USAGE_ON_COLLATION (FIPS Transitional level)
                                                     //SQL_SG_USAGE_ON_TRANSLATION (FIPS Transitional level)
                                                       SQL_SG_WITH_GRANT_OPTION;
                                              break;
    case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:   number = //SQL_SNVF_BIT_LENGTH
                                                       //SQL_SNVF_CHAR_LENGTH
                                                       //SQL_SNVF_CHARACTER_LENGTH
                                                       SQL_SNVF_EXTRACT;
                                                       //SQL_SNVF_OCTET_LENGTH
                                                       //SQL_SNVF_POSITION
                                              break;
    case SQL_SQL92_PREDICATES:                number = SQL_SP_BETWEEN         |
                                                       SQL_SP_COMPARISON      |
                                                       SQL_SP_EXISTS          |
                                                       SQL_SP_IN              |
                                                       SQL_SP_ISNOTNULL       |
                                                       SQL_SP_ISNULL          |
                                                       SQL_SP_LIKE            |
                                                     //SQL_SP_MATCH_FULL 
                                                     //SQL_SP_MATCH_PARTIAL
                                                     //SQL_SP_MATCH_UNIQUE_FULL 
                                                     //SQL_SP_MATCH_UNIQUE_PARTIAL 
                                                     //SQL_SP_OVERLAPS 
                                                       SQL_SP_QUANTIFIED_COMPARISON;
                                                     //SQL_SP_UNIQUE 
                                              break;
    case SQL_SQL92_RELATIONAL_JOIN_OPERATORS: number = //SQL_SRJO_CORRESPONDING_CLAUSE (Intermediate level)
                                                         SQL_SRJO_CROSS_JOIN      |
                                                       //SQL_SRJO_EXCEPT_JOIN (Intermediate level)
                                                         SQL_SRJO_FULL_OUTER_JOIN |
                                                         SQL_SRJO_INNER_JOIN      |
                                                       //SQL_SRJO_INTERSECT_JOIN (Intermediate level)
                                                         SQL_SRJO_LEFT_OUTER_JOIN |
                                                         SQL_SRJO_NATURAL_JOIN    |
                                                         SQL_SRJO_RIGHT_OUTER_JOIN;
                                                       //SQL_SRJO_UNION_JOIN (Full level)
                                              break;
    case SQL_SQL92_REVOKE:                    number =  SQL_SR_CASCADE            |
                                                        SQL_SR_DELETE_TABLE       |
                                                        SQL_SR_GRANT_OPTION_FOR   |
                                                        SQL_SR_INSERT_COLUMN      |
                                                        SQL_SR_INSERT_TABLE       |
                                                        SQL_SR_REFERENCES_COLUMN  |
                                                        SQL_SR_REFERENCES_TABLE   |
                                                        SQL_SR_RESTRICT           |
                                                        SQL_SR_SELECT_TABLE       |
                                                        SQL_SR_UPDATE_COLUMN      |
                                                        SQL_SR_UPDATE_TABLE       ;
                                                      //SQL_SR_USAGE_ON_DOMAIN 
                                                      //SQL_SR_USAGE_ON_CHARACTER_SET 
                                                      //SQL_SR_USAGE_ON_COLLATION 
                                                      //SQL_SR_USAGE_ON_TRANSLATION
                                              break;
    case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:     number = 0;
                                                       //SQL_SRVC_VALUE_EXPRESSION |
                                                       //SQL_SRVC_NULL             |
                                                       //SQL_SRVC_DEFAULT          |
                                                       //SQL_SRVC_ROW_SUBQUERY     ;
                                              break;
    case SQL_SQL92_STRING_FUNCTIONS:          number = //SQL_SSF_CONVERT
                                                         SQL_SSF_LOWER          |
                                                         SQL_SSF_UPPER          |
                                                         SQL_SSF_SUBSTRING      |
                                                       //SQL_SSF_TRANSLATE
                                                         SQL_SSF_TRIM_BOTH      |
                                                         SQL_SSF_TRIM_LEADING   |
                                                         SQL_SSF_TRIM_TRAILING  ;
                                              break;
    case SQL_SQL92_VALUE_EXPRESSIONS:         number = SQL_SVE_CASE     |
                                                       SQL_SVE_CAST     |
                                                       SQL_SVE_COALESCE |
                                                       SQL_SVE_NULLIF   ;
                                              break;
    case SQL_STANDARD_CLI_CONFORMANCE:        number = 0;
                                              // NO INFORMATION. DON't KNOW
                                              break;
    case SQL_STATIC_CURSOR_ATTRIBUTES1:       number = 0;
    case SQL_STATIC_CURSOR_ATTRIBUTES2:       number = 0;
                                              break;
    case SQL_STRING_FUNCTIONS:                number = SQL_FN_STR_ASCII       |
                                                       SQL_FN_STR_BIT_LENGTH  |
                                                       SQL_FN_STR_CHAR        |
                                                       SQL_FN_STR_CHAR_LENGTH |
                                                       SQL_FN_STR_CHARACTER_LENGTH |
                                                       SQL_FN_STR_CONCAT |
                                                     //SQL_FN_STR_DIFFERENCE 
                                                       SQL_FN_STR_INSERT |
                                                       SQL_FN_STR_LCASE  |
                                                       SQL_FN_STR_LEFT   |
                                                       SQL_FN_STR_LENGTH |
                                                       SQL_FN_STR_LOCATE |
                                                       SQL_FN_STR_LTRIM  |
                                                       SQL_FN_STR_OCTET_LENGTH |
                                                       SQL_FN_STR_POSITION |
                                                       SQL_FN_STR_REPEAT |
                                                       SQL_FN_STR_REPLACE|
                                                       SQL_FN_STR_RIGHT  |
                                                       SQL_FN_STR_RTRIM  |
                                                       SQL_FN_STR_SOUNDEX|
                                                       SQL_FN_STR_SPACE  |
                                                       SQL_FN_STR_SUBSTRING |
                                                       SQL_FN_STR_UCASE  ;
                                              break;
    case SQL_SUBQUERIES:                      number = SQL_SQ_CORRELATED_SUBQUERIES |
                                                       SQL_SQ_COMPARISON  |
                                                       SQL_SQ_EXISTS      |
                                                       SQL_SQ_IN          |
                                                       SQL_SQ_QUANTIFIED  ;
                                              break;
    case SQL_SYSTEM_FUNCTIONS:                number = SQL_FN_SYS_DBNAME |
                                                       SQL_FN_SYS_IFNULL |
                                                       SQL_FN_SYS_USERNAME;
                                              break;
    case SQL_TABLE_TERM:                      string = "Table";
                                              break;
    case SQL_TIMEDATE_ADD_INTERVALS:          number = SQL_FN_TSI_FRAC_SECOND |
                                                       SQL_FN_TSI_SECOND  |
                                                       SQL_FN_TSI_MINUTE  |
                                                       SQL_FN_TSI_HOUR    |
                                                       SQL_FN_TSI_DAY     |
                                                       SQL_FN_TSI_WEEK    |
                                                       SQL_FN_TSI_MONTH   |
                                                       SQL_FN_TSI_QUARTER |
                                                       SQL_FN_TSI_YEAR    ;
                                              break;
    case SQL_TIMEDATE_DIFF_INTERVALS:         number = //SQL_FN_TSI_FRAC_SECOND |
                                                       SQL_FN_TSI_SECOND  |
                                                       SQL_FN_TSI_MINUTE  |
                                                       SQL_FN_TSI_HOUR    |
                                                       SQL_FN_TSI_DAY     |
                                                       SQL_FN_TSI_WEEK    |
                                                     //SQL_FN_TSI_MONTH   |
                                                       SQL_FN_TSI_QUARTER |
                                                       SQL_FN_TSI_YEAR    ;
                                              break;
    case SQL_TIMEDATE_FUNCTIONS:              number = SQL_FN_TD_CURRENT_DATE      |
                                                       SQL_FN_TD_CURRENT_TIME      |
                                                       SQL_FN_TD_CURRENT_TIMESTAMP |
                                                       SQL_FN_TD_CURDATE           |
                                                       SQL_FN_TD_CURTIME           |
                                                       SQL_FN_TD_DAYNAME           |
                                                       SQL_FN_TD_DAYOFMONTH        |
                                                       SQL_FN_TD_DAYOFWEEK         | 
                                                       SQL_FN_TD_DAYOFYEAR         | 
                                                       SQL_FN_TD_EXTRACT           |
                                                       SQL_FN_TD_HOUR              |
                                                       SQL_FN_TD_MINUTE            |
                                                       SQL_FN_TD_MONTH             |
                                                       SQL_FN_TD_MONTHNAME         |
                                                       SQL_FN_TD_NOW               |
                                                       SQL_FN_TD_QUARTER           |
                                                       SQL_FN_TD_SECOND            |
                                                     //SQL_FN_TD_TIMESTAMPADD      |
                                                       SQL_FN_TD_TIMESTAMPDIFF     |
                                                       SQL_FN_TD_WEEK              |
                                                       SQL_FN_TD_YEAR;
                                              break;
    case SQL_TXN_CAPABLE:                     tiny = SQL_TC_DML | SQL_TC_DDL_COMMIT;
                                              break;
    case SQL_TXN_ISOLATION_OPTION:            number = SQL_TXN_READ_COMMITTED;
                                              break;
    case SQL_UNION:                           number = SQL_U_UNION | SQL_U_UNION_ALL;
                                              break;
    case SQL_USER_NAME:                       string = (char*) m_user.GetString();
                                              break;
    case SQL_XOPEN_CLI_YEAR:                  string = "1995";
                                              break;
    default:                                  m_ociError.AddError("HY092");
                                              break;
	}
  if(string)
  {
    int len = 0;
    result = CopyODBCStringToApp((SQLCHAR*)InfoValue,string,BufferLength,&len,&m_ociError);
    if(StringLength)
    {
      *StringLength = (SQLSMALLINT)len;
    }
    return result;
  }
  if(tiny != 0xFF)
  {
    *((SQLUSMALLINT*)InfoValue) = tiny;
    if(StringLength)
    {
      *StringLength = sizeof(SQLUSMALLINT);
    }
    result = SQL_SUCCESS;
  }
  if(number != 0xFFFF)
  {
    *((SQLUINTEGER*)InfoValue) = number;
    if(StringLength)
    {
      *StringLength = sizeof(SQLUINTEGER);
    }
    result = SQL_SUCCESS;
  }
	return result;
}

SQLRETURN 
ODBCConnection::SQLGetFunctions(SQLUSMALLINT  FunctionId
                               ,SQLUSMALLINT* Supported)
{
  if(FunctionId == SQL_API_ALL_FUNCTIONS)
  {
    memcpy(Supported,ODBCFunctions_2,sizeof(ODBCFunctions_2));
    return SQL_SUCCESS;
  }
  if(FunctionId == SQL_API_ODBC3_ALL_FUNCTIONS)
  {
    memcpy(Supported,ODBCFunctions_3,sizeof(ODBCFunctions_3));
    return SQL_SUCCESS;
  }
  if(FunctionId > 0 && FunctionId < 100)
  {
    *Supported = ODBCFunctions_2[FunctionId];
    return SQL_SUCCESS;
  }
  if(FunctionId >= 100 && FunctionId <= (SQL_API_ODBC3_ALL_FUNCTIONS_SIZE * 8))
  {
    *Supported = SQL_FUNC_EXISTS(ODBCFunctions_3,FunctionId) ? SQL_TRUE : SQL_FALSE;
    return SQL_SUCCESS;
  }
  m_ociError.AddError("HY095");
  return SQL_ERROR;
}

SQLRETURN
ODBCConnection::UniqueCursor(SQLCHAR* p_cursorName)
{
  ODBCBaseObject* obj = m_statements.GetFirst();
  while(obj)
  {
    ODBCStatement* statement = (ODBCStatement*)obj;
    if(statement->GetCursor().CompareNoCase((const char*)p_cursorName) == 0)
    {
      return SQL_FALSE;
    }
    // Next statement
    obj = obj->GetNext();
  }
  return SQL_TRUE;
}

SQLRETURN 
ODBCConnection::SQLNativeSql(SQLCHAR*    InStatementText
                            ,SQLINTEGER  TextLength1
                            ,SQLCHAR*    OutStatementText
                            ,SQLINTEGER  BufferLength
                            ,SQLINTEGER* TextLength2)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  CString statement = MakeDriverString(InStatementText,TextLength1);
  CString parsed;

  ODBCParser parser;
  parser.Parse(statement,parsed);
  char* SQLstate = parser.GetSQLState();
  if(SQLstate[0])
  {
    m_ociError.AddError(SQLstate);
    return SQL_ERROR;
  }
  return CopyODBCStringToApp(OutStatementText,(char*)parsed.GetString(),BufferLength,(int*)TextLength2,&m_ociError);
}

SQLRETURN
ODBCConnection::SetTranslationDLL()
{
  if(m_attributes.m_translateLib.IsEmpty())
  {
    return SQL_FALSE;
  }
  // Reset
  m_SQLDataSourceToDriver = NULL;
  m_SQLDriverToDataSource = NULL;
  if(m_translateLib)
  {
    FreeLibrary(m_translateLib);
  }
  HMODULE m_translateLib = LoadLibrary(m_attributes.m_translateLib.GetString());
  if(m_translateLib)
  {
    m_SQLDataSourceToDriver = (TYPE_SQLDataSourceToDriver) GetProcAddress(m_translateLib,"SQLDataSourceToDriver");
    m_SQLDriverToDataSource = (TYPE_SQLDriverToDataSource) GetProcAddress(m_translateLib,"SQLDriverToDataSource");
    if(m_SQLDataSourceToDriver && m_SQLDriverToDataSource)
    {
      return SQL_TRUE;
    }
    // Translation library does not contain all functions
    m_ociError.AddError("R1002");
    return SQL_FALSE;
  }
  // Cannot load the translation library
  m_ociError.AddError("R1001");
  return SQL_FALSE;
}

void
ODBCConnection::GetNLSParameters()
{
  WRITELOG("Getting NLS parameters from database");
  try
  {
    char* query = "SELECT parameter\n"
                  "      ,value\n"
                  "  FROM sys.nls_session_parameters";
    SQLHSTMT statement;
    AllocDBStatement((ODBCStatement**)&statement);

    if(SQLPrepare(statement,(SQLCHAR*)query,SQL_NTS) == SQL_SUCCESS)
    {
      if(SQLExecute(statement) == SQL_SUCCESS)
      {
        char parameter[MAX_SESSION_PARAMNAME_LEN+1];
        char value   [MAX_SESSION_PARAMVALUE_LEN+1];
        SQLLEN parLen,valLen;
        while(SQLFetch(statement) == SQL_SUCCESS)
        {
          SQLGetData(statement,1,SQL_CHAR,parameter,MAX_SESSION_PARAMNAME_LEN, &parLen);
          SQLGetData(statement,2,SQL_CHAR,value,    MAX_SESSION_PARAMVALUE_LEN,&valLen);
          parameter[parLen] = 0;
          value    [valLen] = 0;
               if (strcmp(parameter,"NLS_DATE_FORMAT")         == 0) m_attributes.m_dbNlsDateFormat        = value;
          else if (strcmp(parameter,"NLS_TIME_FORMAT")         == 0) m_attributes.m_dbNlsTimeFormat        = value;
          else if (strcmp(parameter,"NLS_TIMESTAMP_FORMAT")    == 0) m_attributes.m_dbNlsTimestampFormat   = value;
          else if (strcmp(parameter,"NLS_TIMESTAMP_TZ_FORMAT") == 0) m_attributes.m_dbNlsTimestampTzFormat = value;
        }
      }
    }
    SQLFreeStmt(statement,SQL_DROP);
  }
  catch(...)
  {
    WRITELOG("Error getting NLS parameters" );
  }
  WRITELOG("Ready getting NLS parameters");
}


void
ODBCConnection::GetDatabaseName()
{
  WRITELOG("Getting Database name from the database");
  try
  {
    char* query = "SELECT sys_context('USERENV','db_name') FROM dual";
    SQLHSTMT statement;
    AllocDBStatement((ODBCStatement**)&statement);

    if(SQLPrepare(statement,(SQLCHAR*)query,SQL_NTS) == SQL_SUCCESS)
    {
      if(SQLExecute(statement) == SQL_SUCCESS)
      {
        char dbname[129];
        SQLLEN nameLen;
        while(SQLFetch(statement) == SQL_SUCCESS)
        {
          SQLGetData(statement,1,SQL_CHAR,dbname,128,&nameLen);
          dbname[nameLen] = 0;
          if(dbname[0])
          {
            m_server = dbname;
          }
        }
      }
    }
    SQLFreeStmt(statement,SQL_DROP);
  }
  catch(...)
  {
    WRITELOG("Error getting database name" );
  }
  WRITELOG("Ready getting database name: %s",m_server);
}
