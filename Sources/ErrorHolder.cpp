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
// Contents:  Implementation of the ErrorHolder
//            Holds a set of errors for an ODBCBaseObject
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "ErrorHolder.h"
#include "MainObject.h"
#include "ErrorInfo.h"
#include "BaseObject.h"
#include "Statement.h"
#include "ODBCTypes.h"
#include "Utility.h"

ODBCErrorHolder::ODBCErrorHolder(OCIEnv *hEnv)
{
  m_hError                  = NULL;
  m_CountOfInternalRecords  = 0;
  m_CountOfRecordsReported  = 0;
  m_DSNName                 = "";
 // Now allocate an OCIError object
  OCIHandleAlloc(hEnv,(dvoid**)&m_hError,OCI_HTYPE_ERROR,0,(dvoid**)0);
}

ODBCErrorHolder::ODBCErrorHolder()
{
  m_hError                 = NULL;
  m_CountOfInternalRecords = 0;
  m_CountOfRecordsReported = 0;
  m_DSNName                = "";
  // OCI Handle not yet called!!
}

ODBCErrorHolder::~ODBCErrorHolder()
{	
  Done();
}

void 
ODBCErrorHolder::Done()
{
  if(m_hError)
  {
    try
    {
      OCIHandleFree((dvoid*)m_hError,OCI_HTYPE_ERROR); //Free OCIError object
    }
    catch(...) {}
    m_hError = NULL;
  }
  Clear();
}

void ODBCErrorHolder::Clear()
{
  for(int i = 0; i < m_CountOfInternalRecords; ++i)
  {
    m_InternalRecord[i].SQLState[0]       = 0;
    m_InternalRecord[i].InternalErrorCode = 0;
    m_InternalRecord[i].ErrorTextString   = "";
  }
  m_CountOfInternalRecords = 0;
  m_CountOfRecordsReported = 0;
}

SQLRETURN 
ODBCErrorHolder::SQLGetDiagRec(SQLSMALLINT  RecNumber
                              ,SQLCHAR*     Sqlstate
                              ,SQLINTEGER*  NativeError
                              ,SQLCHAR*     MessageText
                              ,SQLSMALLINT  BufferLength
                              ,SQLSMALLINT* TextLength)
{
  --RecNumber;
  if(RecNumber < 0)
  {
    WRITELOG("SQLGetDiagRec record number below 1");
    return SQL_ERROR;
  }
  if((BufferLength < 0) && (BufferLength != SQL_NTS))
  {
    WRITELOG("SQLGetDiagRec no room in buffer");
    return SQL_ERROR;
  }
  // Reset error info 
  if(Sqlstate)
  {
    memset(Sqlstate,0,5);
  }
  if(MessageText)
  {
    MessageText[0] = 0;
  }
  if(NativeError)
  {
    *NativeError=0;
  }
  if(TextLength)
  {
    *TextLength=0;
  }
  if(RecNumber >= m_CountOfInternalRecords)
  {
    WRITELOG("SQLGetDiagRec record number to big (%d)",RecNumber + 1);
    return SQL_NO_DATA;
  }
  if(Sqlstate)
  {
    memcpy(Sqlstate,m_InternalRecord[RecNumber].SQLState,5);
    Sqlstate[5]=0;
  }
  if(NativeError)
  {
    *NativeError = m_InternalRecord[RecNumber].InternalErrorCode;
  }
  int len = m_InternalRecord[RecNumber].ErrorTextString.GetLength();
  SQLRETURN result = SQL_SUCCESS;
  if(MessageText)
  {
    // result can be set to SQL_SUCCESS or SQL_SUCCESS_WITH_INFO
    // SQL_ERROR is not possible, we checked MessageText
    result = CopyODBCStringToApp(MessageText,m_InternalRecord[RecNumber].ErrorTextString.GetBuffer(),BufferLength,&len,NULL);
  }
  if(TextLength)
  {
    // Record real copied length
    *TextLength = (SQLSMALLINT)len;
  }
  return result;
}

int 
ODBCErrorHolder::GetErrorFromOCIHandle()
{
  if(m_CountOfInternalRecords < ODBCErrorRecCounts) //is buffer full
  {
    char ErrorText[1024];
    sb4  errorcode = 0;
    ErrorText[0] = 0;
    sword last_res = OCIErrorGet(m_hError,1,NULL
                                ,&errorcode
                                ,(OraText*)ErrorText
                                ,sizeof(ErrorText)
                                ,OCI_HTYPE_ERROR);
    if(last_res!=OCI_SUCCESS)
    {
      return -1;
    }
    memcpy(m_InternalRecord[m_CountOfInternalRecords].SQLState,"HY000",5);
    m_InternalRecord[m_CountOfInternalRecords].SQLState[5]       = 0;
    m_InternalRecord[m_CountOfInternalRecords].InternalErrorCode = errorcode;
    m_InternalRecord[m_CountOfInternalRecords].ErrorTextString   = "[EDO Open Oracle ODBC]";
    if(m_DSNName.GetLength() > 0)//if DSN name of connection is present
    {
      m_InternalRecord[m_CountOfInternalRecords].ErrorTextString += "[";
      m_InternalRecord[m_CountOfInternalRecords].ErrorTextString += m_DSNName;
      m_InternalRecord[m_CountOfInternalRecords].ErrorTextString += "]";
    }
    m_InternalRecord[m_CountOfInternalRecords].ErrorTextString += ErrorText;
    if(ODBCDriver.LogSystemOn())
    {
      ODBCDriver.PrintLog("OCI Error record added \"%s\":%d:\"%s\"",m_InternalRecord[m_CountOfInternalRecords].SQLState,m_InternalRecord[m_CountOfInternalRecords].InternalErrorCode,m_InternalRecord[m_CountOfInternalRecords].ErrorTextString.GetBuffer());
    }
    ++m_CountOfInternalRecords;           //inc count of records
    return m_CountOfInternalRecords - 1;  //return number of record
  }
  else 
  {
    WRITELOG("ODBCErrorHolder NO SPACE TO ADD OCI ERROR RECORD",m_CountOfInternalRecords);
  }
  //record not inserted
  return -1;
}

int 
ODBCErrorHolder::AddError(const char *SQLState,int InternalErrorCode,const char *ErrorText)
{
  //is buffer full
  if(m_CountOfInternalRecords < ODBCErrorRecCounts) 
  {
    memcpy(m_InternalRecord[m_CountOfInternalRecords].SQLState,SQLState,5);
    m_InternalRecord[m_CountOfInternalRecords].SQLState[5]       = 0;
    m_InternalRecord[m_CountOfInternalRecords].InternalErrorCode = InternalErrorCode;
    m_InternalRecord[m_CountOfInternalRecords].ErrorTextString   = "[EDO][Open Oracle ODBC]";
    if(m_DSNName.GetLength()>0)//if DSN name of connection is present
    {
      m_InternalRecord[m_CountOfInternalRecords].ErrorTextString += "[";
      m_InternalRecord[m_CountOfInternalRecords].ErrorTextString += m_DSNName;
      m_InternalRecord[m_CountOfInternalRecords].ErrorTextString += "]";
    }
    int IncomeStrLen = (int)strlen(ErrorText);//Len of ErrorText
    if(IncomeStrLen==0)
    {
      ErrorText = FindODBCErrorText(SQLState,InternalErrorCode);
      if(ErrorText[0]==0)
      {
        ErrorText = "Unknown error";
      }
      IncomeStrLen = (int)strlen(ErrorText);
    }
    m_InternalRecord[m_CountOfInternalRecords].ErrorTextString += ErrorText;
    if(ODBCDriver.LogSystemOn())
    {
      ODBCDriver.PrintLog("Error record added \"%s\"%d\"%s\"",SQLState,InternalErrorCode,m_InternalRecord[m_CountOfInternalRecords].ErrorTextString.GetBuffer());
    }
    ++m_CountOfInternalRecords;
    return m_CountOfInternalRecords - 1;
  }
  else 
  {
    WRITELOG("ODBCErrorHolder NO SPACE TO ADD ERROR RECORD %d",m_CountOfInternalRecords);
  }
  //record not inserted
  return -1;
}

int
ODBCErrorHolder::GetErrorRecord()
{
  if(m_CountOfRecordsReported < m_CountOfInternalRecords)
  {
    int num = m_CountOfRecordsReported;
    ++m_CountOfRecordsReported;
    return num;
  }
  return -1;
}

SQLRETURN 
ODBCErrorHolder::SQLGetDiagField(SQLSMALLINT   HandleType
                                ,ODBCBaseObject* obj
                                ,SQLSMALLINT   RecNumber
                                ,SQLSMALLINT   DiagIdentifier
                                ,SQLPOINTER    DiagInfo
                                ,SQLSMALLINT   BufferLength
                                ,SQLSMALLINT*  StringLength)
{
  int len = 0;
  SQLINTEGER* integer = (SQLINTEGER*)DiagInfo;
  ODBCStatement* statement = NULL;
  if(HandleType == SQL_HANDLE_STMT && obj->ValidObject() == SQL_HANDLE_STMT)
  {
    statement = (ODBCStatement*)obj;
  }
  switch(DiagIdentifier)
  {
    case SQL_DIAG_ROW_COUNT:            // Fall through
    case SQL_DIAG_CURSOR_ROW_COUNT:     if(statement)
                                        {
                                          SQLLEN count;
                                          statement->SQLRowCount(&count);
                                          return (SQLRETURN)count;
                                        }
                                        return SQL_INVALID_HANDLE;
    case SQL_DIAG_DYNAMIC_FUNCTION:     if(statement)
                                        {
                                          SQLRETURN ret = CopyODBCStringToApp((SQLCHAR*)DiagInfo,(char*)statement->GetCachedSQL().GetString(),BufferLength,&len,NULL);
                                          if(StringLength)
                                          {
                                            *StringLength = (SQLSMALLINT)len;
                                          }
                                          return ret;
                                        }
                                        return SQL_INVALID_HANDLE;
    case SQL_DIAG_DYNAMIC_FUNCTION_CODE:if(statement)
                                        {
                                          int type = statement->GetCachedSQLType();
                                          switch(type)
                                          {
                                            case OCI_STMT_SELECT: *integer = SQL_DIAG_SELECT_CURSOR;  break;
                                            case OCI_STMT_UPDATE: *integer = SQL_DIAG_UPDATE_WHERE;   break;
                                            case OCI_STMT_DELETE: *integer = SQL_DIAG_DELETE_WHERE;   break;
                                            case OCI_STMT_INSERT: *integer = SQL_DIAG_INSERT;         break;
                                            case OCI_STMT_DROP:   *integer = SQL_DIAG_DROP_TABLE;     break;
                                            case OCI_STMT_ALTER:  *integer = SQL_DIAG_ALTER_TABLE;    break;
                                            // CREATE/DROP/ALTER are reported on the table
                                            // even though it can be an index/constraint/view/collation/schema etc.
                                            // OCI cannot distinguish between them
                                            case OCI_STMT_CREATE: *integer = SQL_DIAG_CREATE_TABLE;   break;
                                            case OCI_STMT_BEGIN:  // Fall through
                                            case OCI_STMT_DECLARE:// Fall through
                                            case 0:               // Fall through
                                            default:              *integer = SQL_DIAG_UNKNOWN_STATEMENT;
                                                                  break;
                                          }
                                          return SQL_SUCCESS;
                                        }
                                        return SQL_INVALID_HANDLE;
    case SQL_DIAG_NUMBER:               return (SQLRETURN) m_CountOfInternalRecords;
    case SQL_DIAG_CONNECTION_NAME:      // Fall through
    case SQL_DIAG_SERVER_NAME:          CopyODBCStringToApp((SQLCHAR*)DiagInfo,(char*)m_DSNName.GetString(),BufferLength,&len,NULL);
                                        if(StringLength)
                                        {
                                          *StringLength = (SQLSMALLINT)len;
                                        }
                                        return SQL_SUCCESS;
  }
  if(RecNumber)
  {
    --RecNumber;
    char* odbc_class = NULL;
    SQLRETURN result = SQL_ERROR;
    if(RecNumber < m_CountOfInternalRecords)
    {
      switch(DiagIdentifier)
      {
        case SQL_DIAG_CLASS_ORIGIN:         // Fall through
        case SQL_DIAG_SUBCLASS_ORIGIN:      if(strncmp(m_InternalRecord[RecNumber].SQLState,"IM",2) == 0)
                                            {
                                              odbc_class = "ODBC 3.0";
                                            }
                                            else
                                            {
                                              odbc_class = "ISO 9075";
                                            }
                                            result = CopyODBCStringToApp((SQLCHAR*)DiagInfo,odbc_class,BufferLength,&len,NULL);
                                            if(StringLength)
                                            {
                                              *StringLength = (SQLSMALLINT) len;
                                            }
                                            return result;
        case SQL_DIAG_COLUMN_NUMBER:        *integer = SQL_COLUMN_NUMBER_UNKNOWN;
                                            return SQL_SUCCESS;
        case SQL_DIAG_ROW_NUMBER:           *integer = SQL_ROW_NUMBER_UNKNOWN;
                                            return SQL_SUCCESS;
        case SQL_DIAG_MESSAGE_TEXT:         result = CopyODBCStringToApp((SQLCHAR*)DiagInfo,(char*)m_InternalRecord[RecNumber].ErrorTextString.GetString(),BufferLength,&len,NULL);
                                            if(StringLength)
                                            {
                                              *StringLength = (SQLSMALLINT)len;
                                            }
                                            return result;
        case SQL_DIAG_NATIVE:               *integer = m_InternalRecord[RecNumber].InternalErrorCode;
                                            return SQL_SUCCESS;
        case SQL_DIAG_SQLSTATE:             result = CopyODBCStringToApp((SQLCHAR*)DiagInfo,m_InternalRecord[RecNumber].SQLState,BufferLength,&len,NULL);
                                            if(StringLength)
                                            {
                                              *StringLength = (SQLSMALLINT) len;
                                            }
                                            return result;
      }
    }
  }
  return SQL_ERROR;
}
