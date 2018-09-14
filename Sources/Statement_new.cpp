////////////////////////////////////////////////////////////////////////
//
// File:      $Archive: $
// Authors:   W.E. Huisman
// Contents:  Implementation of the statement object
//            Holds a statement and result set of the statement
//
// EDO : Edwig's Driver for Oracle
// ORACLE ODBC DRIVER for ODBC 3.51
//
// Copyright (c) 2008 - 2008 ir. W.E. Huisman
// All rights reserved
//
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "MainObject.h"
#include "Statement.h"
#include "Environment.h"
#include "ODBCTypes.h"
#include "ErrorInfo.h"
#include "MetaData.h"
#include "DescriptorIRD.h"
#include "DescriptorARD.h"
#include "Parser.h"
#include "Utility.h"

#pragma warning (disable: 4996)
// #pragma warning (disable: 4355)

#define STILL_EXECUTING if(m_lock_mutex.islocked() && m_connection->GetAsyncMode())\
                        return SQL_STILL_EXECUTING


ODBCStatement::StmtAttributes::StmtAttributes()
{
  m_metaDataID       = false;    // INFORMATION SCHEMA queries take "%" patterns instead of identifiers
  m_noscan           = false;    // Do scan for { escape sequences
  m_async            = false;
  m_cursorScrollable = false;    // Default one direction scrollable
  m_statementType    = 0;        // Type of last statement (SELECT, INSERT etc)
  m_cachedStatement  = "";       // Last performed query to RDBMS
  m_cursorName       = "";       // Name of scrolling cursor
  m_maxLength        = 0;        // Default no knowledge about network
  m_maxRows          = 0;        // Maximum rows in the cursor
  m_queryTimeout     = 0;        // Timeout of query in seconds
  m_retrieveData     = true;     // Immediately retrieve data after fetch
  m_integerSpeedup   = false;    // Defaults to numbers are (38,38)
}

// For validation of a statement: STMT
__int64 staticStatement = 0x54554E55;

ODBCStatement::ODBCStatement(ODBCConnection* p_connection,OCIStmt *hStmtTemp)
              :ODBCBaseObject(p_connection->GetOwnerODBCEnv()->GetOwnerEnv())
              ,m_connectionPoolIndex(-1)
              ,m_isObjectFreeInPool(false)
              ,m_noData(true)
              ,m_newSQL(false)
              ,m_connection(p_connection)
              ,m_hStmt(hStmtTemp)
              ,m_cancel(false)
              ,m_fetchType(FETCHTYPE_NOTSET)
              ,m_inNeedForData(false)
              ,m_needDataColumn(0)
{
  WRITELOG("ODBCStatement CTOR (%p)",this);
	m_localSign = staticStatement;

  m_IRD = new ODBC_IRD(p_connection,this);  // Internal ROW Descriptor
  m_IPD = new ODBC_IRD(p_connection,this);  // Internal Parameter Descriptor
  m_ARD = new ODBC_ARD(p_connection,this);  // Application ROW Descriptor
  m_APD = new ODBC_ARD(p_connection,this);  // Application Parameter Descriptor

  if(!m_IRD || !m_IPD || !m_ARD || !m_APD)
  {
    // Out of memory
    m_ociError.AddError("HY001");
  }
	m_ociError.SetDSNName(p_connection->GetOCIErrorObject()->GetDSNName());

  // Take defaults from the connection
  m_SQLDataSourceToDriver       = p_connection->GetSQLDataSourceToDriver();
  m_SQLDriverToDataSource       = p_connection->GetSQLDriverToDataSource();
  m_translateOptions            = p_connection->GetTranslateOptions();
  m_attributes.m_maxLength      = p_connection->GetMaxColumnLength();
  m_attributes.m_metaDataID     = p_connection->GetMetaDataID();
  m_attributes.m_integerSpeedup = p_connection->GetIntegerSpeedup();
}

ODBCStatement::~ODBCStatement()
{
  WRITELOG("ODBCStatement DTOR (%p)",this);
	if(m_hStmt)
	{
		OCIHandleFree((dvoid*)m_hStmt,OCI_HTYPE_STMT);
		m_hStmt = NULL;
	}
  m_IRD->DoneObject();	
  m_IPD->DoneObject();
  m_ARD->DoneObject();
  m_APD->DoneObject();
  delete m_IRD;
  delete m_IPD;
  if(m_ARD->IsInternal())
  {
    delete m_ARD;
  }
  if(m_APD->IsInternal())
  {
    delete m_APD;
  }
  m_connection = NULL;

  m_localSign = 0;	
  WRITELOG("ODBCStatement DTOR OK");
}

int 
ODBCStatement::ValidHandle()
{
  if(m_localSign == staticStatement)
  {
    return SQL_HANDLE_STMT;
  }
  else
  {
    return SQL_INVALID_HANDLE;
  }
}

SQLRETURN 
ODBCStatement::CloseCursor()
{
  WRITELOG("ODBCStatement::CloseCursor");
  // Drop old OCI statement handle
//   if(m_hStmt)
//   {
//     OCIHandleFree((dvoid*)m_hStmt,OCI_HTYPE_STMT);
//     m_hStmt = NULL;
//   }
//   // Allocate new OCI statement handle
//   sword ociRes = OCI_ERROR;	
//   ociRes = OCIHandleAlloc(GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
//                          ,(dvoid**)&m_hStmt,OCI_HTYPE_STMT,0,(dvoid**)NULL);
//   if(ociRes != OCI_SUCCESS)
//   {		
//     if(ociRes != OCI_SUCCESS_WITH_INFO)
//     {
//       return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
//     }
//   }

  // RESET the IRD/IPD/ARD/APD descriptors
  m_IRD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);
  m_IPD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);
  if(m_ARD->IsInternal())
  {
    m_ARD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);
  }
  if(m_APD)
  {
    m_APD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);
  }
  // Drop the last statement text
  m_attributes.m_cachedStatement = "";
  m_attributes.m_statementType   = 0;
  // Statement may be reused from now on
  m_cancel         = false;
  m_inNeedForData  = false;
  m_fetchType      = FETCHTYPE_NOTSET;
  m_needDataColumn = 0;
  m_attributes.m_metaDataID     = m_connection->GetMetaDataID();
  m_attributes.m_maxLength      = m_connection->GetMaxColumnLength();
  m_attributes.m_integerSpeedup = m_connection->GetIntegerSpeedup();

  return SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLFreeStmt (SQLUSMALLINT Option)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  switch(Option)
  {
    case SQL_CLOSE:		    return CloseCursor();
  //case SQL_DROP:        Already in ODBCMAin!!!
    case SQL_UNBIND:      m_ARD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);
                          return SQL_SUCCESS;
    case SQL_RESET_PARAMS:m_APD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);
                          return SQL_SUCCESS;
  }
  m_ociError.AddError("HY092",0,NULL);
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SetNewARD(SQLPOINTER p_ard)
{
  ODBC_ARD* newARD = (ODBC_ARD*)p_ard;
  if(newARD->ValidObject() == SQL_HANDLE_DESC)
  {
    if(m_ARD->IsInternal())
    {
      delete m_ARD;
    }
    m_ARD = newARD;
    m_ARD->SetStatement(this);
    return SQL_SUCCESS;
  }
  m_ociError.AddError("HY024");
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SetNewAPD(SQLPOINTER p_apd)
{
  ODBC_ARD* newAPD = (ODBC_ARD*)p_apd;
  if(newAPD->ValidObject() == SQL_HANDLE_DESC)
  {
    if(m_APD->IsInternal())
    {
      delete m_APD;
    }
    m_APD = newAPD;
    m_APD->SetStatement(this);
    return SQL_SUCCESS;
  }
  m_ociError.AddError("HY024");
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::FreeObjectHandler()
{
  WRITELOG("ODBCStatement::FreeObjectHandler(%p)",this);
	if(m_connectionPoolIndex < 0)
	{
		m_connection->RemoveStatement(this);
		delete this;
	}
  else
	{
    CloseCursor();
		m_isObjectFreeInPool = true;
	}
  return SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLGetStmtAttr(SQLINTEGER  Attribute
                             ,SQLPOINTER  Value
                             ,SQLINTEGER  /*BufferLength*/
                             ,SQLINTEGER* StringLength)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  WRITELOG("Attribute %s",ERROR_GetStmtAttribute(Attribute));
  SQLINTEGER val = 0;
  switch(Attribute)
	{		
	  case SQL_ATTR_APP_ROW_DESC:			    val = (SQLINTEGER) m_ARD;        break;
	  case SQL_ATTR_APP_PARAM_DESC:		    val = (SQLINTEGER) m_APD;	       break;
    case SQL_ATTR_ASYNC_ENABLE:         val = m_attributes.m_async;      break;
    case SQL_ATTR_CONCURRENCY:          val = SQL_CONCUR_ROWVER;         break;
    case SQL_ATTR_CURSOR_SCROLLABLE:    val = m_attributes.m_cursorScrollable;
                                        break;
    case SQL_ATTR_CURSOR_SENSITIVITY:   val = SQL_INSENSITIVE;           break;
    case SQL_ATTR_CURSOR_TYPE:          val = SQL_CURSOR_STATIC;         break;
    case SQL_ATTR_ENABLE_AUTO_IPD:      val = SQL_FALSE;                 break;
    case SQL_ATTR_FETCH_BOOKMARK_PTR:   val = 0;                         break;
    case SQL_ATTR_IMP_PARAM_DESC:		    val = (SQLINTEGER) m_IPD;	       break;
	  case SQL_ATTR_IMP_ROW_DESC:			    val = (SQLINTEGER) m_IRD;	       break;
    case SQL_ATTR_KEYSET_SIZE:          val = 0;                         break;
    case SQL_ATTR_MAX_LENGTH:           val = m_attributes.m_maxLength;  break;
    case SQL_ATTR_MAX_ROWS:             val = m_attributes.m_maxRows;    break;
    case SQL_ATTR_METADATA_ID:          val = m_attributes.m_metaDataID; break;
    case SQL_ATTR_NOSCAN:               val = m_attributes.m_noscan;     break;
    case SQL_ATTR_PARAM_BIND_OFFSET_PTR:m_APD->SQLGetDescField(0,SQL_DESC_BIND_OFFSET_PTR,   &val,0,0);  break;
    case SQL_ATTR_PARAM_BIND_TYPE:      m_APD->SQLGetDescField(0,SQL_DESC_BIND_TYPE,         &val,0,0);  break;
    case SQL_ATTR_PARAM_OPERATION_PTR:  m_APD->SQLGetDescField(0,SQL_DESC_ARRAY_STATUS_PTR,  &val,0,0);  break;
    case SQL_ATTR_PARAM_STATUS_PTR:     m_IPD->SQLGetDescField(0,SQL_DESC_ARRAY_STATUS_PTR,  &val,0,0);  break;
    case SQL_ATTR_PARAMS_PROCESSED_PTR: m_IPD->SQLGetDescField(0,SQL_DESC_ROWS_PROCESSED_PTR,&val,0,0);  break;
    case SQL_ATTR_PARAMSET_SIZE:        m_APD->SQLGetDescField(0,SQL_DESC_ARRAY_SIZE,        &val,0,0);  break;
    case SQL_ATTR_QUERY_TIMEOUT:        val = m_attributes.m_queryTimeout; break;
    case SQL_ATTR_RETRIEVE_DATA:        val = m_attributes.m_retrieveData; break;
    case SQL_ATTR_ROW_ARRAY_SIZE:       m_ARD->SQLGetDescField(0,SQL_DESC_ARRAY_SIZE,        &val,0,0);  break;
    case SQL_ATTR_ROW_BIND_OFFSET_PTR:  m_ARD->SQLGetDescField(0,SQL_DESC_BIND_OFFSET_PTR,   &val,0,0);  break;
    case SQL_ATTR_ROW_BIND_TYPE:        m_ARD->SQLGetDescField(0,SQL_DESC_BIND_TYPE,         &val,0,0);  break;
    case SQL_ATTR_ROW_NUMBER:           val = m_IRD->GetRowNumber(); break;
    case SQL_ATTR_ROW_OPERATION_PTR:    m_ARD->SQLGetDescField(0,SQL_DESC_ARRAY_STATUS_PTR,  &val,0,0);  break;
    case SQL_ATTR_ROW_STATUS_PTR:       m_IRD->SQLGetDescField(0,SQL_DESC_ARRAY_STATUS_PTR,  &val,0,0);  break;
    case SQL_ATTR_ROWS_FETCHED_PTR:     m_IRD->SQLGetDescField(0,SQL_DESC_ROWS_PROCESSED_PTR,&val,0,0);  break;
    case SQL_ATTR_SIMULATE_CURSOR:      val = SQL_SC_NON_UNIQUE; break;
    case SQL_ATTR_USE_BOOKMARKS:        val = SQL_UB_OFF;        break;
	  default:		                        m_ociError.AddError("HY092");
		                                    return SQL_ERROR;
	}
  *((SQLINTEGER*) Value) = val;
  if(StringLength)
  {
    *StringLength = sizeof(SQLINTEGER);
  }
	return SQL_SUCCESS;
}

#pragma warning (disable: 4800)

SQLRETURN 
ODBCStatement::SQLSetStmtAttr(SQLINTEGER Attribute
                             ,SQLPOINTER Value
                             ,SQLINTEGER /*StringLength*/)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  WRITELOG("Attribute %s",ERROR_GetStmtAttribute(Attribute));
  SQLUINTEGER intval = (SQLUINTEGER)Value;
  switch(Attribute)
  {
    case SQL_ATTR_APP_ROW_DESC:			    return SetNewARD(Value);
    case SQL_ATTR_APP_PARAM_DESC:		    return SetNewAPD(Value);
    case SQL_ATTR_ASYNC_ENABLE:         m_attributes.m_async = intval;
                                        break;
    case SQL_ATTR_CONCURRENCY:          // Cannot be set (Oracle always uses SQL_CONCUR_ROWVER)
                                        m_ociError.AddError("HY011");
                                        return SQL_ERROR;
    case SQL_ATTR_CURSOR_SCROLLABLE:    m_attributes.m_cursorScrollable = (intval == SQL_SCROLLABLE);
                                        break;
    case SQL_ATTR_CURSOR_SENSITIVITY:   // Oracle does not support this feature
                                        m_ociError.AddError("HY011");
                                        return SQL_ERROR;
    case SQL_ATTR_CURSOR_TYPE:          if(intval != SQL_CURSOR_FORWARD_ONLY || intval == SQL_CURSOR_STATIC)
                                        {
                                          intval = SQL_CURSOR_FORWARD_ONLY;
                                          m_ociError.AddError("01S02");
                                          return SQL_SUCCESS_WITH_INFO;
                                        }
                                        break;
    case SQL_ATTR_ENABLE_AUTO_IPD:      // Cannot set the auto IPD. Optional feature not implemented
                                        m_ociError.AddError("HYC00");
                                        return SQL_ERROR;
    case SQL_ATTR_FETCH_BOOKMARK_PTR:   // Cannot set bookmark pointer. Optional feature not implemented
                                        m_ociError.AddError("HYC00");
                                        return SQL_ERROR;
    case SQL_ATTR_KEYSET_SIZE:          // Cannot set keyset size. Optional feature not implemented
                                        m_ociError.AddError("HYC00");
                                        return SQL_ERROR;
    case SQL_ATTR_MAX_LENGTH:           if(intval >= 0)
                                        {
                                          m_attributes.m_maxLength = intval;
                                        }
                                        break;
    case SQL_ATTR_MAX_ROWS:             m_attributes.m_maxRows = intval;
                                        break;
    case SQL_ATTR_METADATA_ID:          m_attributes.m_metaDataID = intval;
                                        break;
    case SQL_ATTR_NOSCAN:               m_attributes.m_noscan = intval;
                                        break;
    case SQL_ATTR_PARAM_BIND_OFFSET_PTR:m_APD->SQLSetDescField(0,SQL_DESC_BIND_OFFSET_PTR,Value,0);
                                        break;
    case SQL_ATTR_PARAM_BIND_TYPE:      m_APD->SQLSetDescField(0,SQL_DESC_BIND_TYPE,Value,0);
                                        break;
    case SQL_ATTR_PARAM_OPERATION_PTR:  m_APD->SQLSetDescField(0,SQL_DESC_ARRAY_STATUS_PTR,Value,0);
                                        break;
    case SQL_ATTR_PARAM_STATUS_PTR:     m_IPD->SQLSetDescField(0,SQL_DESC_ARRAY_STATUS_PTR,Value,0);
                                        break;
    case SQL_ATTR_PARAMS_PROCESSED_PTR: m_IPD->SQLSetDescField(0,SQL_DESC_ROWS_PROCESSED_PTR,Value,0);
                                        break;
    case SQL_ATTR_PARAMSET_SIZE:        m_APD->SQLSetDescField(0,SQL_DESC_ARRAY_SIZE,Value,0);
                                        break;
    case SQL_ATTR_QUERY_TIMEOUT:        m_attributes.m_queryTimeout = intval;
                                        break;
    case SQL_ATTR_RETRIEVE_DATA:        m_attributes.m_retrieveData = (intval == SQL_RD_ON);
                                        break;
    case SQL_ATTR_ROW_ARRAY_SIZE:       m_ARD->SQLSetDescField(0,SQL_DESC_ARRAY_SIZE,Value,0);
                                        break;
    case SQL_ATTR_ROW_BIND_OFFSET_PTR:  m_ARD->SQLSetDescField(0,SQL_DESC_BIND_OFFSET_PTR,Value,0);
                                        break;
    case SQL_ATTR_ROW_BIND_TYPE:        m_ARD->SQLSetDescField(0,SQL_DESC_BIND_TYPE,Value,0);
                                        break;
    case SQL_ATTR_ROW_OPERATION_PTR:    m_ARD->SQLSetDescField(0,SQL_DESC_ARRAY_STATUS_PTR,Value,0);
                                        break;
    case SQL_ATTR_ROW_STATUS_PTR:       m_IRD->SQLSetDescField(0,SQL_DESC_ARRAY_STATUS_PTR,Value,0);
                                        break;
    case SQL_ATTR_ROWS_FETCHED_PTR:     m_IRD->SQLSetDescField(0,SQL_DESC_ROWS_PROCESSED_PTR,Value,0);
                                        break;
    case SQL_ATTR_SIMULATE_CURSOR:      if(m_attributes.m_statementType)
                                        {
                                          m_ociError.AddError("HY011");
                                          return SQL_ERROR;
                                        }
                                        // Fall through
    case SQL_ATTR_USE_BOOKMARKS:        // Bookmarks are not implemented
    case SQL_ROW_NUMBER:                // Cannot be set
    case SQL_ATTR_IMP_ROW_DESC:			    // IPD's cannot be set
    case SQL_ATTR_IMP_PARAM_DESC:		    // IPD's cannot be set
    default:                            m_ociError.AddError("HY092");
                                        return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

#pragma warning (default: 4800)

SQLRETURN
ODBCStatement::SQLGetStmtOption(SQLUSMALLINT Option
                               ,SQLPOINTER   Value)
{
  SQLGetStmtAttr(Option,Value,SQL_ATTR_MAX_LENGTH,NULL);
  return SQL_ERROR;
}

SQLRETURN 
ODBCStatement::SQLSetStmtOption(SQLUSMALLINT Option
                               ,SQLULEN      Value)
{
  SQLSetStmtAttr(Option,(SQLPOINTER)Value,SQL_ATTR_MAX_LENGTH);
  return SQL_ERROR;
}

SQLRETURN 
ODBCStatement::SQLCloseCursor()
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  if(m_attributes.m_statementType == 0)
  {
    m_ociError.AddError("24000",0,0);
    return SQL_ERROR;
  }
  return CloseCursor();
}

void
ODBCStatement::TestServerConnection()
{
  ub4 status = OCI_SERVER_NOT_CONNECTED;

  int count = m_connection->GetFailoverCount();
  int time  = m_connection->GetFailoverTime();
  if(count < 0 || count > 10) count = 10;
  if(time  < 0 || time  >  5) time  =  5;

  while(count && status == OCI_SERVER_NOT_CONNECTED)
  {
    // Get the server. Could be a different one!!
    OCIServer* server = NULL;
    if(OCIAttrGet(m_connection->GetOCIConnection()
                 ,OCI_HTYPE_SVCCTX
                 ,&server
                 ,0
                 ,OCI_ATTR_SERVER
                 ,m_ociError.GetOwnerErrorObject()) != OCI_SUCCESS)
    {
      // Could not get the server status. 
      // Try waiting a cycle and retry it!
      Sleep(time * 1000);
      // Next cycle for failover service
      --count;
      continue;
    }
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
        return;
      }
      if(status == OCI_SERVER_NOT_CONNECTED)
      {
        // Sleep as many seconds as we are presumed to wait.
        Sleep(time * 1000);
        // Next cycle for failover service
        --count;
      }
    }
    else
    {
      // Could net get the server status. Presume it's there!!
      m_ociError.GetErrorFromOCIHandle();
      return;
    }
  }
}

void
ODBCStatement::SetPrefetch()
{
  ub4 prefetchrows = m_connection->GetPrefetchCount();
  if(OCIAttrSet(m_hStmt,OCI_HTYPE_STMT,&prefetchrows,0,OCI_ATTR_PREFETCH_ROWS,m_ociError.GetOwnerErrorObject()) == OCI_SUCCESS)
  {
    WRITELOG("OCI Prefetch rows set to: %d",prefetchrows);
  }
  ub4 prefetch = m_connection->GetPrefetchMemory();
  if(OCIAttrSet(m_hStmt,OCI_HTYPE_STMT,&prefetch,0,OCI_ATTR_PREFETCH_MEMORY,m_ociError.GetOwnerErrorObject()) == OCI_SUCCESS)
  {
    WRITELOG("OCI Prefetch memory set to: %dKByte",prefetch / 1024);
  }
}

SQLRETURN 
ODBCStatement::SQLPrepare(CString& p_sql)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  int count_esc = 0;
  int count_par = 0;
  ODBCParser parser;

  // Count '?' and '{' markers in the SQL text
  parser.MustParse(p_sql,count_esc,count_par);
  if(count_esc && m_attributes.m_noscan == false)
  {
    CString parsed;
    parser.Parse(p_sql,parsed);
    char* SQLstate = parser.GetSQLState();
    if(SQLstate[0])
    {
      m_ociError.AddError(SQLstate);
      return SQL_ERROR;
    }
    p_sql = parsed;
  }
  if(count_par)
  {
	  // need to realize changing char '?' to Oracle ":par_xx" string, 
    // but we have some trouble when we have LIKE '*k??rt' function, 
    // we can't translate this ?? mark to params so don`t code 
    // char ? in '' text section mark to Oracle params
	  int i;
    int len    = p_sql.GetLength(); // int strlen((char*)StatementText);
    int NowPos = 0;
    const char* StatementText = p_sql.GetString();
	  CString SQLStr;
    // 8 means that strlen("par_xxxx")==8, 10 for some more space
    char* buf = SQLStr.GetBufferSetLength(len + count_par*8 + 10);
	  //now change '?' to ":par_xx" excepts '?' in such marks ''
	  count_par=0;
	  for(i=0;i<len;i++)// for until end of income string
	  {
		  if(StatementText[i]=='\'') //we detect begin of text '' section
		  {
			  buf[NowPos++] = StatementText[i];// copy begin of text section mark	
			  i++;
			  for(;i<len;i++)//for until end of income string
        {
				  if(StatementText[i]=='\'') //we detect end of text '' section
				  {
					  buf[NowPos++] = StatementText[i];//copy end of text section mark
					  break;// we find end of text section, so break our for
				  }
          else
          {
            buf[NowPos++] = StatementText[i];// simply copy char to internal buffer
          }
        }
			  continue;
		  }
		  if(StatementText[i]=='?')
		  {
			  // we cant use SQLStr+=":par_"; because we doesnt setup \0 end of string and strcat operation need detection of end of string
			  buf[NowPos++] = ':';
        buf[NowPos++] = 'p';
        buf[NowPos++] = 'a';
        buf[NowPos++] = 'r';
        buf[NowPos++] = '_';
			  char TempBuff[15];
        itoa(count_par,TempBuff,10);
			  int i1;
			  for(i1=0; TempBuff[i1] != 0; ++i1)//until end of TempBuff string
        {
          buf[NowPos++] = TempBuff[i1];// simply copy char to internal buffer
        }
			  ++count_par;
			  continue;
		  }
		  buf[NowPos++] = StatementText[i];// simply copy char to internal buffer
	  }
	  buf[NowPos] = 0;//set end of string mark

    // Release SQLStr to the p_sql.
    SQLStr.ReleaseBuffer();
    p_sql = SQLStr;
  }
  // Change status. We now have a new SQL string
  m_attributes.m_cachedStatement = p_sql;
  m_newSQL         = true;
  m_inNeedForData  = false;
  m_needDataColumn = 0;

  // Write statement to log again. (Could be altered by binding or ODBC escape sequences)
  WRITELOG("SQLPrepare(%p) Param count=%d ODBC escapes=%d\n%s",this,count_par,count_esc,p_sql.GetString());

  m_IRD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);
  m_IPD->SetItemsCount(count_par);
	m_ARD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);
	m_APD->SetItemsCount(RESET_IRDARD_DESCRIPTOR);

  // Test Server connection
  TestServerConnection();

  // Prepare OCI memory for prefetch
  SetPrefetch();
  // DO THE PREPARE
	sword ociRes = OCIStmtPrepare(m_hStmt
                               ,m_ociError.GetOwnerErrorObject()
                               ,(OraText*)p_sql.GetString()
                               ,p_sql.GetLength()
                               ,OCI_NTV_SYNTAX
                               ,OCI_DEFAULT);
	if(ociRes != OCI_SUCCESS)
  {
    m_ociError.GetErrorFromOCIHandle();
  }
	return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
}

SQLRETURN 
ODBCStatement::SQLNumParams(SQLSMALLINT* ParameterCountPtr)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  *ParameterCountPtr = (SQLSMALLINT)m_IPD->GetRecCount();
	return SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLNumResultCols(SQLSMALLINT* ColumnCount)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  *ColumnCount = (SQLSMALLINT)m_IRD->GetRecCount();
	return SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLRowCount(SQLINTEGER * RowCount)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  *RowCount = 0;
	ub4 RowAffected = 0;//Type of SQL command
	sword ociRes;
	ociRes = OCIAttrGet(m_hStmt,OCI_HTYPE_STMT,&RowAffected,NULL,OCI_ATTR_ROW_COUNT,m_ociError.GetOwnerErrorObject());
	*RowCount = RowAffected;
	if(ociRes != OCI_SUCCESS)
	{
		m_ociError.GetErrorFromOCIHandle();		
	}
	return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
}

SQLRETURN 
ODBCStatement::SQLExecute()
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();
  WRITELOG("SQLExecute(%p)",this);

  sword         ociRes;
  SQLRETURN     sqlRes;
	SQLRETURN     retCode  = SQL_SUCCESS;
  int           recCount = m_IPD->GetRecCount();
	ODBCItems*    lpAPColumn;
	OracleItems*  lpIPColumn;
	OraTypesEnumeration OraDataType;
  int           column = 0;
  bool          needData = false;

  // STEP 1: Create all the IPD columns out of the bound APD
  if(!m_inNeedForData)
  {
    for(column = 0; column < recCount;++column)
    {
      lpIPColumn = m_IPD->GetItem((short)column);
      lpAPColumn = m_APD->GetItem((short)column);

      if(!lpAPColumn)
      {
        // Hey: We've got a '?' marker, but no inbound parameter from SQLBindParameter
        // But our check is loose. More SQLBindParameter's than '?' markers are allowed
        m_ociError.AddError("07002");
        return SQL_ERROR;
      }
      OraDataType = m_APD->ConvertODBCTypeToInternal(lpAPColumn->m_desc_param_type);

      if(OraDataType == UNKNOWN_TYPE)
      {
        // Data type attribute violation
        m_ociError.AddError("07006");
        return SQL_ERROR;
      }
      SQLRETURN res = lpIPColumn->AllocType(OraDataType,lpAPColumn->m_desc_precision,this);
      if(res != OCI_SUCCESS)
      {
        return res;
      }
    }
    // STEP 2: Now push the IPD to the OCI datasource
    retCode = m_IPD->BindAllParams();
    if(retCode!=SQL_SUCCESS)
    {
      return retCode;	
    }	
  }
  // STEP 3: In case of SQLPutData cycle, recalculate our current column
  column = 0;
  if(m_inNeedForData)
  {
    // At re-entry of SQLExecute: always take next column
    // First check that any open LOB will be closed
    lpIPColumn = m_IPD->GetItem(m_needDataColumn);
    if(lpIPColumn->m_lobOpen)
    {
      lpIPColumn->LobClose(GetDBConnection()->GetOCIConnection(),m_ociError);
    }
    // Goto next column
    column = m_needDataColumn + 1;
  }
  else
  {

  // STEP 4: Push all data for the bound columns to the IPD bound locations
  // for Oracle to use them in the BindAllColumns step
  // Basically this is the APD to IPD move
  for(; column < recCount; ++column)
  {
    lpIPColumn = m_IPD->GetItem((short)column);
    lpAPColumn = m_APD->GetItem((short)column);

    if(*lpAPColumn->m_desc_indicator_ptr == SQL_DATA_AT_EXEC ||
       *lpAPColumn->m_desc_indicator_ptr <= SQL_LEN_DATA_AT_EXEC_OFFSET)
    {
      needData = true;
      if(*lpAPColumn->m_desc_indicator_ptr < SQL_LEN_DATA_AT_EXEC_OFFSET)
      {
        // Recalculate the total pending length from the indicator of the APD
        lpIPColumn->m_pendingLength = -((*lpAPColumn->m_desc_indicator_ptr) - SQL_LEN_DATA_AT_EXEC_OFFSET);
      }
      else
      {
        // Without knowing the length beforehand, we cannot do anything.
        // Besides the SQL_NEED_LONG_DATA_LEN attribute of the connections requests so!!
        // Length mismatch error
        m_ociError.AddError("22026");
        return SQL_ERROR;
      }
    }
    else
    {
 		  sqlRes = DriverToDatasource(lpAPColumn->m_desc_type
                                 ,lpAPColumn->m_desc_data_ptr
                                 ,lpAPColumn->m_desc_length
                                 ,lpAPColumn->m_desc_indicator_ptr
                                 ,*lpIPColumn
                                 ,m_ociError
                                 ,this
                                 ,false);
		  if(sqlRes != SQL_SUCCESS &&
         sqlRes != SQL_SUCCESS_WITH_INFO)
		  {
			  return sqlRes;
		  }
    }
	}
  // STEP 5: Debug information for programmers
#ifdef EDO_COLUMN_LOGGING
  if(ODBCDriver.LogSystemOn())
  {
    LogParamInformation();
  }
#endif
  // STEP 6: Type of SQL command from SQLPrepare
	ub2 StmtType;
	ociRes = OCIAttrGet(m_hStmt,OCI_HTYPE_STMT,&StmtType,NULL,OCI_ATTR_STMT_TYPE,m_ociError.GetOwnerErrorObject());
	if(ociRes != OCI_SUCCESS)
	{
		m_ociError.GetErrorFromOCIHandle();
		return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
	}
  // Record for GetDiagField
  m_attributes.m_statementType = (int)StmtType;

  // STEP 7: Check for read-only database option
  if((m_connection->GetAccessMode() == SQL_MODE_READ_ONLY) && (StmtType != OCI_STMT_SELECT))
  {
    // SELECT statement is the only type of executed statement allowed on a read-only connection
    m_ociError.AddError("R0000");
    return SQL_ERROR;
  }
  // STEP 8: Calculate diverse modes for execution
  // If AutoCommit is on then OCI_COMMIT_ON_SUCCESS else OCI_DEFAULT (stay in transaction)
  ub4 transMode = 0; 
  if(m_attributes.m_cursorScrollable == SQL_SCROLLABLE && StmtType == OCI_STMT_SELECT)
  {
    // Do scrollable cursors for SQLFetchScroll and SQLExtendedFetch
    transMode = OCI_STMT_SCROLLABLE_READONLY;
  }
  // For all others: default or AUTOCOMMIT_ON
  transMode |= (m_connection->GetAutoCommitMode() != SQL_AUTOCOMMIT_ON) ? OCI_DEFAULT :  OCI_DEFAULT; // OCI_COMMIT_ON_SUCCESS;

  // STEP 9: Now do the real EXECUTE !!!
	ociRes = OCIStmtExecute(GetDBConnection()->GetOCIConnection()
                         ,m_hStmt
	                       ,m_ociError.GetOwnerErrorObject()
	                       ,StmtType != OCI_STMT_SELECT ? 1 : 0 // for select SQL command iters==0, for others iters==1
	                       ,0
                         ,NULL
                         ,NULL
	                       ,transMode);		
  // STEP 10: Check and process the return value
	if(ociRes == OCI_NO_DATA)	
	{
		m_noData = true;
		ociRes   = OCI_SUCCESS;
	}
  else
	{
    m_noData = (StmtType == OCI_STMT_SELECT) ? false : true;
	}
	if(ociRes != OCI_SUCCESS)
	{
		m_ociError.GetErrorFromOCIHandle();
		if(ociRes != OCI_SUCCESS_WITH_INFO)
    {
      return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
    }
	}

  }

  // NEW

  if(needData || m_inNeedForData)
  {
    column = 0;
    if(m_inNeedForData)
    {
      column = m_needDataColumn + 1;
    }
    for(; column < recCount; ++column)
    {
      lpIPColumn = m_IPD->GetItem((short)column);
      lpAPColumn = m_APD->GetItem((short)column);

      if(*lpAPColumn->m_desc_indicator_ptr == SQL_DATA_AT_EXEC ||
         *lpAPColumn->m_desc_indicator_ptr <= SQL_LEN_DATA_AT_EXEC_OFFSET)
      {
        m_inNeedForData  = true;
        m_needDataColumn = column;
        // Go for SQLParamData / SQLPutData cycle !!
        return SQL_NEED_DATA;
      }
    }
  }




  // STEP 11: If we did a second exec of the same prepared statement
  // don't describe the column set on the IRD again, we are ready!
	if(!m_newSQL)
  {
    return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
  }
  // STEP 12: Get the count of columns for this executed statement
  // We need the number to set the IRD Descriptor columns size and to 
  // read the column info for all the columns
	ub4 ColCount;
	ociRes=OCIAttrGet(m_hStmt,OCI_HTYPE_STMT,&ColCount,NULL,OCI_ATTR_PARAM_COUNT,m_ociError.GetOwnerErrorObject());	
	if(ociRes!=OCI_SUCCESS)
	{
		m_ociError.GetErrorFromOCIHandle();
		if(ociRes!=OCI_SUCCESS_WITH_INFO)
    {
      return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
    }
	}
	m_IRD->SetItemsCount(ColCount);
	
  // STEP 13: READ column info for all columns in the executed statement
	OCIParam* hColDesc;
	for(unsigned int i=1; i <= ColCount;i++)
	{
		if(OCIParamGet(m_hStmt,OCI_HTYPE_STMT,m_ociError.GetOwnerErrorObject(),(dvoid**)&hColDesc,i)==OCI_SUCCESS)
		{
			retCode = m_IRD->GetItem(i-1)->ReadColumnInfo(m_IRD,hColDesc,m_ociError,m_attributes.m_maxLength);
			OCIDescriptorFree(hColDesc,OCI_DTYPE_PARAM);
			
			if(retCode != SQL_SUCCESS)
      {
        return retCode;
      }
		}
	}
  // Set binding for step 11 on next execution
	m_newSQL = false;	

  // STEP 14: Bind all IRD columns to the OCI
	return m_IRD->DefineAllColumns();
}

// Do the real fetch things
SQLRETURN
ODBCStatement::InternalFetch(SQLINTEGER    p_direction
                            ,SQLINTEGER    p_irow
                            ,SQLULEN*      p_gotten
                            ,SQLUSMALLINT* p_status_ptr)
{
  // STEP 1: Basic status checks
  if(m_noData)
  {
    return SQL_NO_DATA;
  }
  if(m_cancel)
  {
    // Fetch, FetchScroll, ExtendedFetch canceled
    m_ociError.AddError("S1008");
    return SQL_ERROR;
  }
  if(p_direction != OCI_FETCH_NEXT && m_attributes.m_cursorScrollable == false)
  {
    // Fetch type out of range
    m_ociError.AddError("S1106");
    return SQL_ERROR;
  }
  // STEP 2: NOW REALLY DO THE FETCH
  sword ociRes = OCIStmtFetch2(m_hStmt
                              ,m_ociError.GetOwnerErrorObject()
                              ,1                  // number of rows to process
                              ,(ub2)p_direction
                              ,p_irow             // only: OCI_FETCH_ABSOLUTE || OCI_FETCH_RELATIVE
                              ,OCI_DEFAULT);
  // No data, at end of cursor
  if(ociRes == OCI_NO_DATA)	
	{
    WRITELOG("SQLFetch no more data");
    if(p_status_ptr)
    {
      *p_status_ptr = (SQLUSMALLINT) SQL_ROW_NOROW;
    }
		m_noData = true;
		return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
	}
  // Any errors after fetch
	if(ociRes != OCI_SUCCESS && ociRes != OCI_SUCCESS_WITH_INFO)
  {
    WRITELOG("SQLFetch error %d",ociRes);
    if(p_status_ptr)
    {
      *p_status_ptr = (SQLUSMALLINT) SQL_ROW_ERROR;
    }
    m_ociError.GetErrorFromOCIHandle();
    return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
  }
  // STEP 3: Upgrade rows processed
  m_IRD->ProcessRow();
  if(p_gotten)
  {
    // TODO: Calculate per row
    *p_gotten = 1;
  }
  if(p_status_ptr)
  {
    *p_status_ptr = (SQLUSMALLINT) (ociRes == OCI_SUCCESS_WITH_INFO) ? SQL_ROW_SUCCESS_WITH_INFO : SQL_ROW_SUCCESS;
  }
  // STEP 4: Resetting the GetData status
  for(int i=0; i<m_IRD->GetRecCount(); ++i)
  {
    m_IRD->GetItem(i)->ResetGetData();
  }

  // STEP 5: Optionally fetch the data
  if(m_attributes.m_retrieveData)
  {
    // Only retrieve data if SQL_ATTR_RETRIEVE_DATA is set to SQL_RD_ON
    // If we don not do this, application should call GetData on **all** columns
	  int i,recCount = m_IRD->GetRecCount();
	  ODBCItems*   lpARColumn = NULL;
	  OracleItems* lpIRColumn = NULL;
	  SQLRETURN    sqlRes = SQL_ERROR;

    WRITELOG("SQLFetch fetching %d columns",recCount);
    for(i=0;i<recCount;++i)
	  {
		  lpIRColumn = m_IRD->GetItem((short)i);
		  lpARColumn = m_ARD->GetItem((short)i);
		  if(lpIRColumn && lpARColumn && lpARColumn->m_desc_type && lpARColumn->m_desc_data_ptr)
      {
        // Only do the bound columns!!
		    sqlRes = DatasourceToDriver(lpARColumn->m_desc_type
                                   ,lpARColumn->m_desc_data_ptr
                                   ,lpARColumn->m_desc_length
                                   ,lpARColumn->m_desc_indicator_ptr
                                   ,*lpIRColumn
                                   ,m_ociError
                                   ,this
                                   ,false);
		    if(sqlRes != SQL_SUCCESS)
		    {
			    return sqlRes;
		    }
		  }
      else
		  {
			  if(lpARColumn && lpARColumn->m_desc_indicator_ptr)
			  {
				  *lpARColumn->m_desc_indicator_ptr = SQL_NULL_DATA;
			  }
		  }		
	  }
  }
#ifdef EDO_COLUMN_LOGGING
  if(ODBCDriver.LogSystemOn())
  {
  	LogColumnInformation();
  }
#endif
	return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
}

SQLRETURN 
ODBCStatement::SQLFetch()
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  // Check type of fetch on this prepared statement
  if(m_fetchType == FETCHTYPE_SCROLL || m_fetchType == FETCHTYPE_EXTENDED)
  {
    // Function sequence error
    m_ociError.AddError("S1010");
  }
  m_fetchType = FETCHTYPE_FETCH;
  // SQLFetch is always 1 FETCH NEXT
  return InternalFetch(OCI_FETCH_NEXT,0,NULL,m_ARD->GetArrayStatusPointer());
}

SQLRETURN
ODBCStatement::SQLFetchScroll(SQLSMALLINT FetchOrientation
                             ,SQLLEN      FetchOffset)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();
  
  // Check type of fetch on this prepared statement
  if(m_fetchType == FETCHTYPE_FETCH || m_fetchType == FETCHTYPE_EXTENDED)
  {
    // Function sequence error
    m_ociError.AddError("S1010");
  }
  m_fetchType = FETCHTYPE_SCROLL;

  // Calculate OCI Fetch
  int direction;
  switch(FetchOrientation)
  {
    case SQL_FETCH_NEXT:      direction = OCI_FETCH_NEXT;     break;
    case SQL_FETCH_FIRST:     direction = OCI_FETCH_FIRST;    break;
    case SQL_FETCH_LAST:      direction = OCI_FETCH_LAST;     break;
    case SQL_FETCH_PRIOR:     direction = OCI_FETCH_PRIOR;    break;
    case SQL_FETCH_ABSOLUTE:  direction = OCI_FETCH_ABSOLUTE; break;
    case SQL_FETCH_RELATIVE:  direction = OCI_FETCH_RELATIVE; break;
    default:                  m_ociError.AddError("S1106");
                              return SQL_ERROR;
  }
  return InternalFetch(direction,FetchOffset,NULL,m_ARD->GetArrayStatusPointer());
}

SQLRETURN 
ODBCStatement::SQLExtendedFetch(SQLUSMALLINT   FetchOrientation
                               ,SQLLEN         irow
                               ,SQLULEN*       pcrow
                               ,SQLUSMALLINT*  rgfRowStatus)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  // Check type of fetch on this prepared statement
  if(m_fetchType == FETCHTYPE_FETCH || m_fetchType == FETCHTYPE_SCROLL)
  {
    // Function sequence error
    m_ociError.AddError("S1010");
  }
  m_fetchType = FETCHTYPE_EXTENDED;

  // Calculate OCI FETCH type
  int direction = 0;
  switch(FetchOrientation)
  {
    case SQL_FETCH_NEXT:      direction = OCI_FETCH_NEXT;     break;
    case SQL_FETCH_FIRST:     direction = OCI_FETCH_FIRST;    break;
    case SQL_FETCH_LAST:      direction = OCI_FETCH_LAST;     break;
    case SQL_FETCH_PRIOR:     direction = OCI_FETCH_PRIOR;    break;
    case SQL_FETCH_ABSOLUTE:  direction = OCI_FETCH_ABSOLUTE; break;
    case SQL_FETCH_RELATIVE:  direction = OCI_FETCH_RELATIVE; break;
    default:                  m_ociError.AddError("S1106");
                              break;
  }
  return InternalFetch(direction,irow,pcrow,rgfRowStatus);
}

SQLRETURN
ODBCStatement::SQLSetPos(SQLSETPOSIROW /*irow    */
                        ,SQLUSMALLINT  /*fOption */
                        ,SQLUSMALLINT  /*fLock   */)
{
  // irow -> 0 = every row
  // 1 <= irow <= SQL_ATTR_ROWSET_SIZE
  // foption -> SQL_POSITION / SQL_REFRESH / SQL_UPDATE / SQL_DELETE
  // flock   -> SQL_LOCK_NO_CHANGE / SQL_LOCK_EXCLUSIVE / SQL_LOCK_UNLOCK
  return SQL_ERROR;
}

SQLRETURN 
ODBCStatement::SQLMoreResults()
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  return m_noData ? SQL_NO_DATA : SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLDescribeCol(SQLUSMALLINT ColumnNumber
                             ,SQLCHAR*     ColumnName
                             ,SQLSMALLINT  BufferLength
                             ,SQLSMALLINT* NameLength
                             ,SQLSMALLINT* DataType
                             ,SQLUINTEGER* ColumnSize
                             ,SQLSMALLINT* DecimalDigits
                             ,SQLSMALLINT* Nullable)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();
	
	if(ColumnNumber <1 && BufferLength < 0)
  {
    return SQL_ERROR;
  }
  int ColCount = m_IRD->GetRecCount();
	if(ColumnNumber > ColCount)
  {
    return SQL_ERROR;
  }
  char* name = (char*)m_IRD->GetItem(ColumnNumber-1)->m_desc_name;
  int   len  = 0;
  CopyODBCStringToApp(ColumnName,name,BufferLength,&len,&m_ociError);
  if(NameLength)
  {
    *NameLength = (SQLSMALLINT)len;
  }
  if(DataType)
  {
		*DataType = m_IRD->ConvertInternalToODBCType(m_IRD->GetItem(ColumnNumber-1)->m_valueType);		
  }
	if(ColumnSize)
  {
    //m_desc_precision;
    *ColumnSize = m_IRD->GetItem(ColumnNumber-1)->m_desc_length;
  }
	if(DecimalDigits)
  {
    *DecimalDigits = m_IRD->GetItem(ColumnNumber-1)->m_desc_scale;
  }
	if(Nullable)
	{
    *Nullable = m_IRD->GetItem(ColumnNumber-1)->m_desc_nullable;
	}
	return SQL_SUCCESS;	
}

SQLRETURN 
ODBCStatement::SQLGetData(SQLUSMALLINT ColumnNumber
                         ,SQLSMALLINT  TargetType
                         ,SQLPOINTER   TargetValue
                         ,SQLINTEGER   BufferLength
                         ,SQLINTEGER*  StrLen_or_Ind)
{
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  if(ColumnNumber <= 0 || ColumnNumber > m_IRD->GetRecCount())
  {
    // Descriptor out of range
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  OracleItems* record = m_IRD->GetItem(ColumnNumber - 1);
  if(record->AllDataGotten())
  {
    // This field completely gotten after last fetch
    record->m_pendingLength = 0;
    record->m_pendingOffset = 0;
  }
  if(record->GetPendingLength())
  {
    if(record->IsLOBvalue())
    {
      return CopyODBCLOBToApp(GetDBConnection()->GetOCIConnection()
                             ,TargetValue
                             ,record->Value.lob_val
                             ,BufferLength
                             ,(int*)StrLen_or_Ind
                             ,m_ociError
                             ,record
                             ,m_attributes.m_maxLength
                             ,true);
    }
    if(record->IsBinaryValue())
    {
      return CopyODBCRAWToApp(GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                             ,record->Value.raw_val
                             ,TargetValue
                             ,BufferLength
                             ,(int*)StrLen_or_Ind
                             ,&m_ociError
                             ,record
                             ,m_attributes.m_maxLength
                             ,true);
    }
    // Get the rest of the pending data for a string
    return CopyODBCStringToApp((SQLCHAR*)TargetValue
                              ,(char*)record->m_pendingOffset
                              ,BufferLength
                              ,(int*)StrLen_or_Ind
                              ,&m_ociError
                              ,record
                              ,m_attributes.m_maxLength
                              ,true);
  }
  // Just get the sucker!
  return DatasourceToDriver(TargetType
                           ,TargetValue
                           ,BufferLength
                           ,StrLen_or_Ind
                           ,*record
                           ,m_ociError
                           ,this
                           ,true);
}

SQLRETURN 
ODBCStatement::SQLBindCol(SQLUSMALLINT ColumnNumber
                         ,SQLSMALLINT  TargetType
                         ,SQLPOINTER   TargetValue
                         ,SQLINTEGER   BufferLength
                         ,SQLINTEGER*  StrLen_or_Ind)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();
	
  if(ColumnNumber<1)
  {
    return SQL_ERROR;
  }
	ODBCItems *lpColumn = m_ARD->AllocItem(ColumnNumber-1);
	
	lpColumn->m_desc_type           = TargetType;
	lpColumn->m_desc_data_ptr       = TargetValue;
	lpColumn->m_desc_indicator_ptr  = StrLen_or_Ind;
	lpColumn->m_desc_length         = BufferLength;
	lpColumn->m_desc_parameter_type = 0;
	lpColumn->m_desc_param_type     = 0;
	lpColumn->m_desc_precision      = 0;
	lpColumn->m_desc_scale          = 0;
	return SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLBindParameter(SQLUSMALLINT ParameterNumber
                               ,SQLSMALLINT  InputOutputType
                               ,SQLSMALLINT  ValueType
                               ,SQLSMALLINT  ParameterType
                               ,SQLUINTEGER  ColumnSize
                               ,SQLSMALLINT  DecimalDigits
                               ,SQLPOINTER   ParameterValuePtr
                               ,SQLINTEGER   BufferLength
                               ,SQLINTEGER*  StrLen_or_IndPtr)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
	m_ociError.Clear();
	
  if(ParameterNumber < 1)
  {
    return SQL_ERROR;
  }
	OraTypesEnumeration OraDataType = m_APD->ConvertODBCTypeToInternal(ParameterType);
	if(OraDataType == UNKNOWN_TYPE)
	{
		m_ociError.AddError("HY004");
		return SQL_ERROR;
	}
	ODBCItems* lpColumn = m_APD->AllocItem(ParameterNumber-1);
	
  if(InputOutputType == SQL_RETURN_VALUE ||
     InputOutputType == SQL_RESULT_COL    )
  {
    // Oracle doesn't support SQL_RETURN_VALUE and SQL_RESULT_COL. 
    // For Oracle it's just an output parameter.
    InputOutputType = SQL_PARAM_OUTPUT;
  }
	lpColumn->m_desc_type           = ValueType;
	lpColumn->m_desc_data_ptr       = ParameterValuePtr;
	lpColumn->m_desc_indicator_ptr  = StrLen_or_IndPtr;
	lpColumn->m_desc_length         = BufferLength;
	lpColumn->m_desc_parameter_type = InputOutputType;
	lpColumn->m_desc_param_type     = ParameterType;
	lpColumn->m_desc_precision      = ColumnSize;
	lpColumn->m_desc_scale          = DecimalDigits;
		
	return SQL_SUCCESS;
}

#ifdef EDO_LOGGING

void 
ODBCStatement::LogParamInformation()
{
	int i,recCount = m_IPD->GetRecCount();
	ODBCItems *lpAPColumn;
	OracleItems *lpIPColumn;
	
	CString Str;
	char Buff[200];
  Str.Preallocate(100);

	for(i=0;i<recCount;i++)
	{
		lpIPColumn = m_IPD->GetItem((short)i);
		lpAPColumn = m_APD->GetItem((short)i);
		sprintf_s(Buff,200,"Param:%d[",i+1);
		Str  = Buff;
		Str += GetOraTypesEnumerationInString(lpIPColumn->m_valueType);
		Str += ",";
		if(lpIPColumn->m_indicator == SQL_NULL_DATA)//NULL value
		{
			Str += "NULL";
		}
    else
		{
      if(lpIPColumn->IsLOBvalue())
      {
        Str += "BINARY LARGE OBJECT";
      }
      else
      {
			  Buff[0]=0;
			  DatasourceToDriver(SQL_C_CHAR,Buff,200,NULL,*lpIPColumn,m_ociError,this,false);//some trick to convert in string
			  Str += Buff;
      }
		}
		Str += "][";
		if(lpAPColumn)
		{
			Str += ConvertSQL_C_TypeToString(lpAPColumn->m_desc_type);
			Str += ",";
			Str += ConvertSQL_TypeToString(lpAPColumn->m_desc_param_type);
			if(lpAPColumn->m_desc_indicator_ptr && *lpAPColumn->m_desc_indicator_ptr == SQL_NULL_DATA)
			{
				Str += ",NULL";
			}					
		}
    else
    {
      Str += "NO DATA";
    }
		Str += "]";
		ODBCDriver.PrintLog(Str);
	}
}

void 
ODBCStatement::LogColumnInformation()
{
	int i,recCount = m_IRD->GetRecCount();
	ODBCItems *lpAPColumn;
	OracleItems *lpIPColumn;
	
	CString Str;
  Str.Preallocate(100);

	for(i=0;i<recCount;i++)
	{
    int   len  = m_IRD->GetItem((short)i)->m_desc_length + 100;
    char* Buff = (char*) malloc(len);

    lpIPColumn = m_IRD->GetItem((short)i);
		lpAPColumn = m_ARD->GetItem((short)i);
		sprintf(Buff,"Column:%d[",i+1);
		Str  = Buff;
		Str += GetOraTypesEnumerationInString(lpIPColumn->m_valueType);
		Str += ",";
		if(lpIPColumn->m_indicator == SQL_NULL_DATA)//NULL value
		{
			Str += "NULL value";
		}
    else
		{
			Buff[0] = 0;
      //some trick to convert in string
      if(lpIPColumn->IsLOBvalue())
      {
        Str += "BINARY LARGE OBJECT";
      }
      else
      {
			  DatasourceToDriver(SQL_C_CHAR,Buff,len-1,NULL,*lpIPColumn,m_ociError,this,false);
			  Str += Buff;
      }
		}
		Str += "][";
		if(lpAPColumn)
		{
			Str += ConvertSQL_C_TypeToString(lpAPColumn->m_desc_type);				
			if(lpAPColumn->m_desc_indicator_ptr && *lpAPColumn->m_desc_indicator_ptr == SQL_NULL_DATA)
			{
				Str += ",NULL";
			}
		}
    else
    {
      Str += "NO DATA";
    }
		Str += "]";
		ODBCDriver.PrintLog(Str);

    free(Buff);
	}
}

#endif // EDO_LOGGING

SQLRETURN 
ODBCStatement::SQLColAttribute(SQLUSMALLINT ColumnNumber
                              ,SQLUSMALLINT FieldIdentifier
                              ,SQLCHAR*     CharacterAttribute
                              ,SQLSMALLINT  BufferLength
                              ,SQLSMALLINT* StringLength
                              ,SQLINTEGER*  NumericAttribute)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  if(ColumnNumber < 1 || BufferLength < 0)
  {
    WRITELOG("SQLColAttribute. ColumnNumber or BufferLength error (%d,%d)",ColumnNumber,BufferLength);
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  int ColCount = m_IRD->GetRecCount();
  if(ColumnNumber > ColCount)
  {
    WRITELOG("SQLAttribute. ColumnNumber greater than number of columns (%d,%d)",ColumnNumber,ColCount);
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  // Simple cases without an OCI column descriptor
  switch(FieldIdentifier)
  {
    case SQL_DESC_AUTO_UNIQUE_VALUE:  // Oracle does not have auto-incrementing columns
                                      // Oracle has the SEQUENCE object instead
                                      *NumericAttribute = SQL_FALSE;
                                      break;
    case SQL_DESC_BASE_TABLE_NAME:    // Oracle does not support this
                                      CharacterAttribute[0] = 0;
                                      break;
    case SQL_DESC_BASE_COLUMN_NAME:   // Base column name of the column
    case SQL_DESC_LABEL:              // Get display column name (alias)
    case SQL_DESC_NAME:               // Label or base column name
                                      {
                                        int   len = 0;
                                        char* name = m_IRD->GetItem(ColumnNumber-1)->m_desc_name;
                                        CopyODBCStringToApp(CharacterAttribute,name,BufferLength,&len,&m_ociError);
                                        *StringLength = (SQLSMALLINT)len;
                                      }
                                      break;
    case SQL_DESC_CASE_SENSITIVE:     // Column case sensitive
                                      switch(m_IRD->GetItem(ColumnNumber-1)->m_valueType)
                                      {
                                        case RAW_TYPE:
                                        case LONGRAW_TYPE:
                                        case LONG_TYPE:
                                        case BLOB_TYPE:
                                        case DATE_TYPE:
                                        case NUMBER_TYPE:
                                        case INTEGER_TYPE:
                                        case DOUBLE_TYPE:
                                        case DATETIME_TYPE:
                                        case ANSI_DATE_TYPE:
                                        case DATETIMEWTZ_TYPE:
                                        case DATETIMEWLTZ_TYPE:
                                        case INTERVALYM_TYPE:
                                        case INTERVALDS_TYPE:
                                        default:                // NOT CASE SENSITIVE
                                                                *NumericAttribute = SQL_FALSE;
                                                                break;
                                        case UNKNOWN_TYPE:      // CLOB? LOB?
                                        case ROWID_TYPE:
                                        case CLOB_TYPE:
                                        case STRING_TYPE:       // CASE SENSITIVE
                                                                *NumericAttribute = SQL_TRUE;
                                                                break;
                                      }
                                      break;
    case SQL_DESC_CATALOG_NAME:       // Not supported by Oracle
                                      CharacterAttribute[0] = 0;
                                      break;
    case SQL_DESC_CONCISE_TYPE:       // Special concise for date/time types
                                      // Oracle does not much of these!!
                                      switch(m_IRD->GetItem(ColumnNumber-1)->m_valueType)
                                      {
                                        case DATE_TYPE:         
                                        case DATETIME_TYPE:     
                                        case ANSI_DATE_TYPE:
                                        case DATETIMEWTZ_TYPE:
                                        case DATETIMEWLTZ_TYPE: *NumericAttribute = SQL_TYPE_TIMESTAMP; 
                                                                break;
                                        case INTERVALYM_TYPE:   *NumericAttribute = SQL_INTERVAL_YEAR_TO_MONTH; 
                                                                break;
                                        case INTERVALDS_TYPE:   *NumericAttribute = SQL_INTERVAL_DAY_TO_SECOND;
                                                                break;
                                        default:                *NumericAttribute = 0;
                                                                break;
                                      }
                                      break;
    case SQL_DESC_COUNT:              // Number of columns in the result set
                                      *NumericAttribute = ColCount;
                                      break;
    case SQL_DESC_DISPLAY_SIZE:       // Get display size
                                      switch(m_IRD->GetItem(ColumnNumber-1)->m_valueType)
                                      {
                                        case UNKNOWN_TYPE:
                                        case BLOB_TYPE:
                                        case LONG_TYPE:
                                        case RAW_TYPE:
                                        case LONGRAW_TYPE:
                                        case CLOB_TYPE:
                                        case STRING_TYPE:       *NumericAttribute = m_IRD->GetItem(ColumnNumber-1)->m_desc_length;
                                                                break;
                                        case NUMBER_TYPE:      
                                        case INTEGER_TYPE:
                                        case DOUBLE_TYPE:       *NumericAttribute = 65;
                                                                break;
                                        case DATE_TYPE:         
                                        case DATETIME_TYPE:
                                        case ANSI_DATE_TYPE:    *NumericAttribute = 25;
                                                                break;
                                        case DATETIMEWTZ_TYPE:
                                        case DATETIMEWLTZ_TYPE:
                                        case INTERVALYM_TYPE:
                                        case INTERVALDS_TYPE:   *NumericAttribute = 25;
                                                                break;
                                        case ROWID_TYPE:        *NumericAttribute = 20;
                                                                break;
                                      }
                                      break;
    case SQL_DESC_FIXED_PREC_SCALE:   // Explicit defined precision or scale
                                      *NumericAttribute = m_IRD->GetItem(ColumnNumber-1)->m_desc_fixed_prec_scale;
                                      break;
    case SQL_DESC_LENGTH:             *NumericAttribute = m_IRD->GetItem(ColumnNumber-1)->m_desc_length;
                                      break;
    case SQL_DESC_LITERAL_PREFIX:     strcpy((char*)CharacterAttribute,ODBC_Prefixes[m_IRD->GetItem(ColumnNumber-1)->m_valueType]);
                                      break;
    case SQL_DESC_LITERAL_SUFFIX:     strcpy((char*)CharacterAttribute,ODBC_Suffixes[m_IRD->GetItem(ColumnNumber-1)->m_valueType]);
                                      break;
    case SQL_DESC_LOCAL_TYPE_NAME:    CharacterAttribute[0] = 0;
                                      break;
    case SQL_DESC_NULLABLE:           *NumericAttribute = m_IRD->GetItem(ColumnNumber-1)->m_desc_nullable;
                                      break;
    case SQL_DESC_NUM_PREC_RADIX:     if(m_IRD->GetItem(ColumnNumber-1)->m_valueType == INTEGER_TYPE)
                                      {
                                        *NumericAttribute = 10;
                                      }
                                      else if(m_IRD->GetItem(ColumnNumber-1)->m_valueType == DOUBLE_TYPE ||
                                              m_IRD->GetItem(ColumnNumber-1)->m_valueType == NUMBER_TYPE )
                                      {
                                        *NumericAttribute = 2;
                                      }
                                      else
                                      {
                                        *NumericAttribute = 0;
                                      }
                                      break;
    case SQL_DESC_OCTET_LENGTH:       // Get display size as octets
                                      *NumericAttribute = m_IRD->GetItem(ColumnNumber-1)->m_desc_octet_length;
                                      break; 
    case SQL_DESC_PRECISION:          // Get the precision
                                      *NumericAttribute = m_IRD->GetItem(ColumnNumber-1)->m_desc_precision;
                                      break;
    case SQL_DESC_SCALE:              // Get the scale
                                      *NumericAttribute = m_IRD->GetItem(ColumnNumber-1)->m_desc_scale;
                                      break;
    case SQL_DESC_SEARCHABLE:         // Type of searchability
                                      *NumericAttribute  = (m_IRD->GetItem(ColumnNumber-1)->m_valueType == STRING_TYPE) ? SQL_PRED_CHAR : SQL_PRED_BASIC;
                                      break;
    case SQL_DESC_SCHEMA_NAME:        // Schema of base table. Not supported
                                      CharacterAttribute[0] = 0;
                                      break;
    case SQL_DESC_TABLE_NAME:         // Table name of this column. Not supported
                                      CharacterAttribute[0] = 0;
                                      break;
    case SQL_DESC_TYPE:               // real data type
                                      *NumericAttribute = m_IRD->ConvertInternalToODBCType(m_IRD->GetItem(ColumnNumber-1)->m_valueType);
                                      break;
    case SQL_DESC_TYPE_NAME:          // Real type name
                                      {
                                        int   len = 0;
                                        char* name = m_IRD->GetItem(ColumnNumber-1)->m_desc_type_name;
                                        CopyODBCStringToApp(CharacterAttribute,name,BufferLength,&len,&m_ociError);
                                        *StringLength = (SQLSMALLINT)len;
                                      }
                                      break;
    case SQL_DESC_UNNAMED:            // Columns are always named in Oracle
                                      *NumericAttribute = SQL_NAMED;
                                      break;
    case SQL_DESC_UNSIGNED:           // Signed types
                                      switch(m_IRD->GetItem(ColumnNumber-1)->m_valueType)
                                      {
                                        case BLOB_TYPE:
                                        case CLOB_TYPE:
                                        case LONG_TYPE:
                                        case RAW_TYPE:
                                        case LONGRAW_TYPE:
                                        case STRING_TYPE:
                                        case UNKNOWN_TYPE:
                                        case DATETIME_TYPE:
                                        case ANSI_DATE_TYPE:
                                        case DATETIMEWTZ_TYPE:
                                        case DATETIMEWLTZ_TYPE:
                                        case ROWID_TYPE:
                                        case DATE_TYPE:         
                                        default:                // UNSIGNED
                                                                *NumericAttribute = SQL_TRUE;
                                                                break;
                                        case INTEGER_TYPE:
                                        case DOUBLE_TYPE:
                                        case INTERVALYM_TYPE:
                                        case INTERVALDS_TYPE:
                                        case NUMBER_TYPE:       // SIGNED
                                                                *NumericAttribute = SQL_FALSE;
                                                                break;
                                      }
                                      break;
    case SQL_DESC_UPDATABLE:          // OCI not capable
                                      *NumericAttribute = SQL_ATTR_READWRITE_UNKNOWN;
                                      break;
    default:                          m_ociError.AddError("HY092");
                                      return SQL_ERROR;
  }
  return SQL_SUCCESS;	
}

SQLRETURN 
ODBCStatement::SQLGetTypeInfo(SQLSMALLINT DataType)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  // SQL_ALL_TYPES = 0 !!
  CString query = GetMetaQuery(META_TYPE,DataType);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

// Reprint the catalog.schema.table combination for this DBMS
CString
ODBCStatement::MakeObjectName(CString catalog
                                   ,CString schema
                                   ,CString table)
{
  CString objectName;

  if(!schema.IsEmpty() && schema != SQL_ALL_CATALOGS)
  {
    objectName += CString(schema);
    objectName += ".";
  }
  if(!table.IsEmpty() && table != "%")
  {
    objectName += table;
  }
  if(!catalog.IsEmpty() && catalog != SQL_ALL_CATALOGS)
  {
    objectName += "@" + catalog;
  }
  return objectName;
}

SQLRETURN
ODBCStatement::SQLTables(SQLCHAR*    CatalogName
                        ,SQLSMALLINT NameLength1
                        ,SQLCHAR*    SchemaName
                        ,SQLSMALLINT NameLength2
                        ,SQLCHAR*    TableName
                        ,SQLSMALLINT NameLength3
                        ,SQLCHAR*    TableType
                        ,SQLSMALLINT NameLength4)
{
  CString query;
  CString server   = m_connection->GetServer();
  server  .MakeUpper();

  CString catalogs = MakeDriverString(CatalogName,NameLength1);
  CString schemas  = MakeDriverString(SchemaName, NameLength2);
  CString tables   = MakeDriverString(TableName,  NameLength3);
  CString types    = MakeDriverString(TableType,  NameLength4);
  catalogs.MakeUpper();
  schemas .MakeUpper();
  tables  .MakeUpper();
  types   .MakeUpper();

  if(strcmp(catalogs,SQL_ALL_CATALOGS) == 0 && schemas.IsEmpty() && tables.IsEmpty())
  {
    query = GetMetaQuery(META_CATALOGS,0);
  }
  else if(strcmp(schemas,SQL_ALL_SCHEMAS) == 0 && catalogs.IsEmpty() && tables.IsEmpty())
  {
    query = GetMetaQuery(META_SCHEMAS,0);
  }
  else if(strcmp(types,SQL_ALL_TABLE_TYPES) == 0 && catalogs.IsEmpty() && schemas.IsEmpty() && tables.IsEmpty())
  {
    query = GetMetaQuery(META_TABLETYPES,0);
  }
  else
  {
    query = GetMetaQueryTables(META_TABLE,m_attributes.m_metaDataID,server,catalogs,schemas,tables,types);
  }
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SQLColumns(SQLCHAR*    CatalogName
                         ,SQLSMALLINT NameLength1
                         ,SQLCHAR*    SchemaName
                         ,SQLSMALLINT NameLength2
                         ,SQLCHAR*    TableName
                         ,SQLSMALLINT NameLength3
                         ,SQLCHAR*    ColumnName
                         ,SQLSMALLINT NameLength4)
{
  CString query;
  CString server   = m_connection->GetServer();

  CString catalog = MakeDriverString(CatalogName,NameLength1);
  CString schema  = MakeDriverString(SchemaName, NameLength2);
  CString table   = MakeDriverString(TableName,  NameLength3);
  CString column  = MakeDriverString(ColumnName, NameLength4);

  server .MakeUpper();
  catalog.MakeUpper();
  schema. MakeUpper();
  table.  MakeUpper();
  column. MakeUpper();

  query = GetMetaQueryColumns(META_COLUMNS,m_attributes.m_metaDataID,server,catalog,schema,table,column);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SQLPrimaryKeys(SQLCHAR*    szCatalogName
                             ,SQLSMALLINT cbCatalogName
                             ,SQLCHAR*    szSchemaName
                             ,SQLSMALLINT cbSchemaName
                             ,SQLCHAR*    szTableName
                             ,SQLSMALLINT cbTableName)
{
  CString query;
  CString server   = m_connection->GetServer();
  server  .MakeUpper();

  CString catalogs = MakeDriverString(szCatalogName,cbCatalogName);
  CString schemas  = MakeDriverString(szSchemaName, cbSchemaName);
  CString tables   = MakeDriverString(szTableName,  cbTableName);
  catalogs.MakeUpper();
  schemas. MakeUpper();
  tables.  MakeUpper();

  CString types;
  query = GetMetaQueryTables(META_PRIMARYS,m_attributes.m_metaDataID,server,catalogs,schemas,tables,types);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SQLForeignKeys(SQLCHAR*    szPkCatalogName
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
  CString query;
  CString server = m_connection->GetServer();
  server     .MakeUpper();

  CString pk_catalogs = MakeDriverString(szPkCatalogName,cbPkCatalogName);
  CString pk_schemas  = MakeDriverString(szPkSchemaName, cbPkSchemaName);
  CString pk_tables   = MakeDriverString(szPkTableName,  cbPkTableName);
  CString fk_catalogs = MakeDriverString(szFkCatalogName,cbFkCatalogName);
  CString fk_schemas  = MakeDriverString(szFkSchemaName, cbFkSchemaName);
  CString fk_tables   = MakeDriverString(szFkTableName,  cbFkTableName);
  pk_catalogs.MakeUpper();
  pk_schemas. MakeUpper();
  pk_tables.  MakeUpper();
  fk_catalogs.MakeUpper();
  fk_schemas. MakeUpper();
  fk_tables.  MakeUpper();

  query = GetMetaQueryForeign(m_attributes.m_metaDataID,server
                             ,pk_catalogs,pk_schemas,pk_tables
                             ,fk_catalogs,fk_schemas,fk_tables);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN 
ODBCStatement::SQLStatistics(SQLCHAR*     CatalogName
                            ,SQLSMALLINT  NameLength1
                            ,SQLCHAR*     SchemaName
                            ,SQLSMALLINT  NameLength2
                            ,SQLCHAR*     TableName
                            ,SQLSMALLINT  NameLength3
                            ,SQLUSMALLINT Unique
                            ,SQLUSMALLINT Reserved)
{
  CString query;
  CString server   = m_connection->GetServer();
  server.  MakeUpper();

  CString catalogs = MakeDriverString(CatalogName,NameLength1);
  CString schemas  = MakeDriverString(SchemaName, NameLength2);
  CString tables   = MakeDriverString(TableName,  NameLength3);
  catalogs.MakeUpper();
  schemas. MakeUpper();
  tables.  MakeUpper();

  query = GetMetaQueryStatistics(m_attributes.m_metaDataID,server,catalogs,schemas,tables,Unique);

  // SQL_QUICK or SQL_ENSURE?
  // Only if the connection is in ENSURE mode!
  if(Reserved == SQL_ENSURE && GetDBConnection()->GetEnsureStatistics() == SQL_ENSURE) 
  {
    WRITELOG("SLOW SQLStatstics with SQL_ENSURE!");
    // We should analyze the table first to recompute the statistics
    // ANALYZE TABLE schema.table@catalog COMPUTE STATISTICS;
    CString  analyze = "ANALYZE TABLE " + MakeObjectName(catalogs,schemas,tables) + " COMPUTE STATISTICS";
    SQLRETURN result = SQLPrepare(analyze);
    if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
    {
      SQLExecute();
    }
  }
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN 
ODBCStatement::SQLSpecialColumns(SQLCHAR*     CatalogName
                                ,SQLSMALLINT  NameLength1
                                ,SQLCHAR*     SchemaName
                                ,SQLSMALLINT  NameLength2
                                ,SQLCHAR*     TableName
                                ,SQLSMALLINT  NameLength3)
{
  CString query;
  CString server   = m_connection->GetServer();
  server.  MakeUpper();

  CString catalogs = MakeDriverString(CatalogName,NameLength1);
  CString schemas  = MakeDriverString(SchemaName, NameLength2);
  CString tables   = MakeDriverString(TableName,  NameLength3);
  catalogs.MakeUpper();
  schemas. MakeUpper();
  tables.  MakeUpper();

  query = GetMetaQuerySpecials(server,catalogs,schemas,tables);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SQLTablePrivileges(SQLCHAR*    szCatalogName
                                 ,SQLSMALLINT cbCatalogName
                                 ,SQLCHAR*    szSchemaName
                                 ,SQLSMALLINT cbSchemaName
                                 ,SQLCHAR*    szTableName
                                 ,SQLSMALLINT cbTableName)
{
  CString query;
  CString server   = m_connection->GetServer();
  server.  MakeUpper();

  CString catalogs = MakeDriverString(szCatalogName,cbCatalogName);
  CString schemas  = MakeDriverString(szSchemaName, cbSchemaName);
  CString tables   = MakeDriverString(szTableName,  cbTableName);
  catalogs.MakeUpper();
  schemas. MakeUpper();
  tables.  MakeUpper();

  query = GetMetaQueryTablePrivileges(m_attributes.m_metaDataID,server,catalogs,schemas,tables);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN 
ODBCStatement::SQLColumnPrivileges(SQLCHAR*    szCatalogName
                                  ,SQLSMALLINT cbCatalogName
                                  ,SQLCHAR*    szSchemaName
                                  ,SQLSMALLINT cbSchemaName
                                  ,SQLCHAR*    szTableName
                                  ,SQLSMALLINT cbTableName
                                  ,SQLCHAR*    szColumnName
                                  ,SQLSMALLINT cbColumnName)
{
  CString query;
  CString server   = m_connection->GetServer();
  server.  MakeUpper();

  CString catalogs = MakeDriverString(szCatalogName,cbCatalogName);
  CString schemas  = MakeDriverString(szSchemaName, cbSchemaName);
  CString tables   = MakeDriverString(szTableName,  cbTableName);
  CString columns  = MakeDriverString(szColumnName, cbColumnName);
  catalogs.MakeUpper();
  schemas. MakeUpper();
  tables.  MakeUpper();
  columns. MakeUpper();

  query = GetMetaQueryColumnPrivileges(m_attributes.m_metaDataID,server,catalogs,schemas,tables,columns);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SQLProcedures(SQLCHAR*    szCatalogName
                            ,SQLSMALLINT cbCatalogName
                            ,SQLCHAR*    szSchemaName
                            ,SQLSMALLINT cbSchemaName
                            ,SQLCHAR*    szProcName
                            ,SQLSMALLINT cbProcName)
{
  CString query;
  CString server   = m_connection->GetServer();
  server.  MakeUpper();

  CString catalogs = MakeDriverString(szCatalogName,cbCatalogName);
  CString schemas  = MakeDriverString(szSchemaName, cbSchemaName);
  CString procs    = MakeDriverString(szProcName,   cbProcName);
  catalogs.MakeUpper();
  schemas. MakeUpper();
  procs.   MakeUpper();

  query = GetMetaQueryProcedures(m_attributes.m_metaDataID,server,catalogs,schemas,procs);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SQLProcedureColumns(SQLCHAR*    szCatalogName
                                  ,SQLSMALLINT cbCatalogName
                                  ,SQLCHAR*    szSchemaName
                                  ,SQLSMALLINT cbSchemaName
                                  ,SQLCHAR*    szProcName
                                  ,SQLSMALLINT cbProcName
                                  ,SQLCHAR*    szColumnName
                                  ,SQLSMALLINT cbColumnName)
{
  CString query;
  CString server   = m_connection->GetServer();
  server.  MakeUpper();

  CString catalogs   = MakeDriverString(szCatalogName,cbCatalogName);
  CString schemas    = MakeDriverString(szSchemaName, cbSchemaName);
  CString procedures = MakeDriverString(szProcName,   cbProcName);
  CString columns    = MakeDriverString(szColumnName, cbColumnName);
  catalogs.  MakeUpper();
  schemas.   MakeUpper();
  procedures.MakeUpper();
  columns.   MakeUpper();

  query = GetMetaQueryProcColumns(m_attributes.m_metaDataID,server,catalogs,schemas,procedures,columns);
  SQLRETURN result = SQLPrepare(query);
  if(result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
  {
    return SQLExecute();
  }
  return SQL_ERROR;
}

SQLRETURN
ODBCStatement::SQLSetCursorName(SQLCHAR* p_cursorName)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  if(m_attributes.m_statementType)
  {
    // Cursor is already in prepared or executed state
    m_ociError.AddError("24000");
    return SQL_ERROR;
  }
  if(m_connection->UniqueCursor(p_cursorName) == SQL_FALSE)
  {
    // Duplicate cursor name
    m_ociError.AddError("3C000");
    return SQL_ERROR;
  }
  m_attributes.m_cursorName = p_cursorName;
  return SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLGetCursorName(SQLCHAR*     CursorName
                               ,SQLSMALLINT  BufferLength
                               ,SQLSMALLINT* NameLength)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  int len = 0;
  CopyODBCStringToApp(CursorName,(char*)m_attributes.m_cursorName.GetString(),BufferLength,&len,&m_ociError);
  *NameLength = (SQLSMALLINT)len;
  return SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLDescribeParam(SQLUSMALLINT ipar
                               ,SQLSMALLINT* pfSqlType
                               ,SQLULEN*     pcbParamDef
                               ,SQLSMALLINT* pibScale
                               ,SQLSMALLINT* pfNullable)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  if(ipar < 1 || ipar > m_IPD->GetRecCount())
  {
    // Invalid descriptor index
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  OracleItems* record = m_IPD->GetItem(ipar);
  if(pfSqlType)
  {
    *pfSqlType = (SQLSMALLINT)m_IPD->ConvertInternalToODBCType(record->m_valueType);
  }
  if(pcbParamDef)
  {
    *pcbParamDef = (SQLUINTEGER) record->m_desc_length;
  }
  if(pibScale)
  {
    *pibScale = (SQLSMALLINT) record->m_desc_scale;
  }
  if(pfNullable)
  {
    *pfNullable = (SQLSMALLINT) record->m_desc_nullable;
  }
  return SQL_SUCCESS;
}

SQLRETURN
ODBCStatement::SQLCancel()
{
  // In ODBC 3.x Canceling has no other effect
  // In older versions Canceling is equivalent to freeing the statement.
  // But for now, we just record the canceled state for SQLFetch, SQLExtendedFetch and SQLScrollFetch
  m_cancel    = true;
  m_fetchType = FETCHTYPE_NOTSET;

  // canceling works by explicitly fetching zero (0) records
  sword ociRes = OCIStmtFetch(m_hStmt,m_ociError.GetOwnerErrorObject(),0,OCI_FETCH_NEXT,OCI_DEFAULT);
  if(ociRes != OCI_SUCCESS)	
  {
    WRITELOG("SQLFetch no more data");
    return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
  }
  m_noData = true;
  WRITELOG("SQLCancel: Statement canceled");
  return SQL_SUCCESS;
}

SQLRETURN 
ODBCStatement::SQLSetScrollOptions(SQLUSMALLINT /*fConcurrency */
                                  ,SQLLEN       /*crowKeyset   */
                                  ,SQLUSMALLINT /*crowRowset   */)
{
  return SQL_ERROR;
}

SQLRETURN 
ODBCStatement::SQLParamData(SQLPOINTER* Value)
{
  // Extra level to pop Guard before calling SQLExecute
  {
    STILL_EXECUTING;
    Guard guard(m_lock_mutex);
    m_ociError.Clear();

    // Must be called after a SQLExecute returns SQL_NEED_DATA
    if(m_cancel)
    {
      // Statement canceled
      m_ociError.AddError("HY008");
      return SQL_ERROR;
    }
    if(!m_inNeedForData)
    {
      // No column flagged in SQLExecute. So nothing to do.
      // Function sequence error
      m_ociError.AddError("HY010");
      return SQL_ERROR;
    }
    OracleItems* record = m_IPD->GetItem((short)m_needDataColumn);
    ODBCItems* lpColumn = m_APD->GetItem((short)m_needDataColumn);
    if(record == NULL || lpColumn == NULL)
    {
      // Invalid descriptor index
      m_ociError.AddError("07009");
      return SQL_ERROR;
    }
    // Check if record is complete
    if(record->m_pendingLength > 0)
    {
      // Get's you the SQL_DESC_DATA_PTR as bound in SQLBindParameter
      // ODBC Standard is unclear why you should do this and what you should do with it.
      // But the pointer from SQLBindParameter is meant to be the column number
      // So we pass that back here, so the application will know which column comes next.
      *Value = lpColumn->m_desc_data_ptr;
      return SQL_NEED_DATA;
    }
  }
  // COMPLETE, retry at next column
  return SQLExecute();
}

SQLRETURN 
ODBCStatement::SQLPutData(SQLPOINTER Data
                         ,SQLLEN     StrLen_or_Ind)
{
  STILL_EXECUTING;
  Guard guard(m_lock_mutex);
  m_ociError.Clear();

  // Must be called after a SQLExecute returns SQL_NEED_DATA
  if(m_cancel)
  {
    // Statement canceled
    m_ociError.AddError("HY008");
    return SQL_ERROR;
  }
  if(!m_inNeedForData)
  {
    // No column flagged in SQLExecute. So nothing to do.
    // Function sequence error
    m_ociError.AddError("HY010");
    return SQL_ERROR;
  }
  OracleItems* record = m_IPD->GetItem((short)m_needDataColumn);
  ODBCItems* lpColumn = m_APD->GetItem((short)m_needDataColumn);
  if(record == NULL || lpColumn == NULL)
  {
    // Invalid descriptor index
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  OCIEnv* environment = GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv();
  switch(record->m_valueType)
  {
    case STRING_TYPE:   return CopyODBCStringToRDBMS(environment
                                                    ,lpColumn->m_desc_length   // Original bind length
                                                    ,(char*)Data
                                                    ,StrLen_or_Ind
                                                    ,NULL                       // No result len. We know that now
                                                    ,&m_ociError
                                                    ,record
                                                    ,GetMaxLength()
                                                    ,true);
    case BLOB_TYPE:     // Fall through
    case CLOB_TYPE:     return CopyODBCLOBToRDBMS (GetDBConnection()->GetOCIConnection()
                                                  ,record->Value.lob_val
                                                  ,Data
                                                  ,StrLen_or_Ind
                                                  ,NULL             // No result len. We know that now
                                                  ,m_ociError
                                                  ,record
                                                  ,GetMaxLength()
                                                  ,true);
    case RAW_TYPE:      // Fall through
    case LONGRAW_TYPE:  return CopyODBCRAWToRDBMS (environment
                                                  ,record->Value.raw_val
                                                  ,record->m_desc_length
                                                  ,Data
                                                  ,StrLen_or_Ind
                                                  ,NULL             // No ResultLen. We know that already
                                                  ,&m_ociError
                                                  ,record
                                                  ,GetMaxLength()
                                                  ,true);
                        break;
 // case LONG_TYPE:     // LONG is not implemented. THe only streaming interface for this datatype
                        // is by using the OCI-Piecewise interface. 

    default:            // Restricted / Incorrect data type error
                        m_ociError.AddError("07006");
                        return SQL_ERROR;
  }
}

