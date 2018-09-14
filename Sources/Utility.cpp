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
// Contents:  Utility conversion routines
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "Utility.h"
#include "ErrorInfo.h"
#include "MainObject.h"
#include "Connection.h"
#include "Environment.h"
#include <math.h>

// For static OCINumberToText and OCITextToNumber conversions
const char* NLS_NUMERIC_CHARACTERS      = "NLS_NUMERIC_CHARACTERS='.,'";
const int   NLS_LENGTH                  = 27;

#ifdef EDO_LOGGING

SQLValues SQL_C_TypesInString[]=
{
  DEF_INT_AND_TEXT_VAL(SQL_C_CHAR),
  DEF_INT_AND_TEXT_VAL(SQL_C_SSHORT),
  DEF_INT_AND_TEXT_VAL(SQL_C_SHORT),
  DEF_INT_AND_TEXT_VAL(SQL_C_USHORT),
  DEF_INT_AND_TEXT_VAL(SQL_C_SLONG),
  DEF_INT_AND_TEXT_VAL(SQL_C_LONG),
  DEF_INT_AND_TEXT_VAL(SQL_C_ULONG),
  DEF_INT_AND_TEXT_VAL(SQL_C_FLOAT),
  DEF_INT_AND_TEXT_VAL(SQL_C_DOUBLE),
  DEF_INT_AND_TEXT_VAL(SQL_C_BIT),
  DEF_INT_AND_TEXT_VAL(SQL_C_STINYINT),
  DEF_INT_AND_TEXT_VAL(SQL_C_TINYINT),
  DEF_INT_AND_TEXT_VAL(SQL_C_UTINYINT),
  DEF_INT_AND_TEXT_VAL(SQL_C_SBIGINT),
  DEF_INT_AND_TEXT_VAL(SQL_C_UBIGINT),
  DEF_INT_AND_TEXT_VAL(SQL_C_NUMERIC),
  DEF_INT_AND_TEXT_VAL(SQL_C_TYPE_DATE),
  DEF_INT_AND_TEXT_VAL(SQL_C_TYPE_TIME),
  DEF_INT_AND_TEXT_VAL(SQL_C_TYPE_TIMESTAMP),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_DAY),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_DAY_TO_HOUR),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_DAY_TO_MINUTE),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_DAY_TO_SECOND),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_HOUR),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_HOUR_TO_MINUTE),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_HOUR_TO_SECOND),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_MINUTE),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_MINUTE_TO_SECOND),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_SECOND),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_YEAR),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_MONTH),
  DEF_INT_AND_TEXT_VAL(SQL_C_INTERVAL_YEAR_TO_MONTH),
  DEF_INT_AND_TEXT_VAL(SQL_C_BINARY),
  DEF_INT_AND_TEXT_VAL(SQL_C_GUID)
};

SQLValues SQL_TypesInString[]=
{
  DEF_INT_AND_TEXT_VAL(SQL_CHAR),
  DEF_INT_AND_TEXT_VAL(SQL_VARCHAR),
  DEF_INT_AND_TEXT_VAL(SQL_LONGVARCHAR),
  DEF_INT_AND_TEXT_VAL(SQL_DECIMAL),
  DEF_INT_AND_TEXT_VAL(SQL_NUMERIC),
  DEF_INT_AND_TEXT_VAL(SQL_SMALLINT),
  DEF_INT_AND_TEXT_VAL(SQL_INTEGER),
  DEF_INT_AND_TEXT_VAL(SQL_REAL),
  DEF_INT_AND_TEXT_VAL(SQL_FLOAT),
  DEF_INT_AND_TEXT_VAL(SQL_DOUBLE),
  DEF_INT_AND_TEXT_VAL(SQL_BIT),
  DEF_INT_AND_TEXT_VAL(SQL_TINYINT),
  DEF_INT_AND_TEXT_VAL(SQL_BIGINT),
  DEF_INT_AND_TEXT_VAL(SQL_BINARY),
  DEF_INT_AND_TEXT_VAL(SQL_VARBINARY),
  DEF_INT_AND_TEXT_VAL(SQL_LONGVARBINARY),
  DEF_INT_AND_TEXT_VAL(SQL_TYPE_DATE),
  DEF_INT_AND_TEXT_VAL(SQL_TYPE_TIME),
  DEF_INT_AND_TEXT_VAL(SQL_TYPE_TIMESTAMP),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_MONTH),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_YEAR),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_YEAR_TO_MONTH),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_DAY),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_HOUR),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_MINUTE),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_SECOND),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_DAY_TO_HOUR),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_DAY_TO_MINUTE),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_DAY_TO_SECOND),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_HOUR_TO_MINUTE),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_HOUR_TO_SECOND),
  DEF_INT_AND_TEXT_VAL(SQL_INTERVAL_MINUTE_TO_SECOND),
  DEF_INT_AND_TEXT_VAL(SQL_GUID)
};

const char*
ConvertSQL_TypeToString(SQLSMALLINT ODBC_Type)
{
  int Len = sizeof(SQL_TypesInString)/sizeof(SQL_TypesInString[0]);
  for(int i=0; i<Len; ++i)
  {
    if(SQL_TypesInString[i].ODBC_Type == ODBC_Type)
    {
      return SQL_TypesInString[i].Str;
    }
  }
  return "SQL_UNKNOWN";
}

const char*
ConvertSQL_C_TypeToString(SQLSMALLINT ODBC_Type)
{
  int Len = sizeof(SQL_C_TypesInString)/sizeof(SQL_C_TypesInString[0]);
  for(int i=0; i<Len; ++i)
  {
    if(SQL_C_TypesInString[i].ODBC_Type == ODBC_Type)
    {
      return SQL_C_TypesInString[i].Str;
    }
  }
  return "SQL_C_UNKNOWN";
}

#endif // EDO_LOGGING

//////////////////////////////////////////////////////////////////////////
//
// Helper functions
//
//////////////////////////////////////////////////////////////////////////
//
// Data flows TO the ODBC driver
CString
MakeDriverString(SQLCHAR* p_string,SQLINTEGER p_length)
{
  CString string;
  if(p_length == 0)
  {
    // It's very easy to return an empty string
    return string;
  }
  else if(p_length == SQL_NTS)
  {
    string = (char*)p_string;
  }
  else if(p_length > 0)
  {
    p_length;
    // Possibly a Delphi character array.
    // Don't count on a terminating 0 after the string
    char* buffer = string.GetBufferSetLength(p_length + 1);
    strncpy_s(buffer,p_length + 1,(char*)p_string,p_length);
    // Zero terminate it
    buffer[p_length] = 0;
    // Release the buffer, setting the new string length
    // Otherwise CString will call for length calculation
    // which is costly in a driver
    string.ReleaseBuffer(p_length);
  }
  else // p_length < 0 && p_length != SQL_NTS
  {
    // Illegal string. Should not happen. DM must check this!!
    // AddError("22026")
  }
  return string;
}

// ODBC;DSN=test;UID=user;PWD=verysecret;
// Find with p_var = "UID="
int
GetVariableFromConnectionString(CString& p_string,char* p_var,char* p_buffer,int p_buflen)
{
  int len = 0;
  const char* buf = p_string.GetString();
  char* find = strstr((char*)buf,p_var);

  // Make buffer empty
  *p_buffer = 0;

  if(find)
  {
    find += (int)strlen(p_var);
    while(*find && *find != ';' && p_buflen--)
    {
      ++len;
      *p_buffer++ = *find++;
    }
    *p_buffer = 0;
  }
  return len;
}

//////////////////////////////////////////////////////////////////////////
//
// DriverToApplication: copy a string with all special cases
// Buffer:      Application buffer accepting a NTS string
// Source:      Driver or datasource source of the string
// Length:      Application can accept this much data
// RealLength:  Really copied string data
// ociError:    Register our errors here.
// oraItem:     IRD storage of the field of this row
// maxLength:   Maximum data of SQL_ATTR_MAX_LENGTH of the statement
//              After this much we **Must** stop and flag SQL_NO_TOTAL
// getData:     Doing or resuming GetData streaming mode
//
SQLRETURN
CopyODBCStringToApp(SQLCHAR*          Buffer
                   ,char*             Source
                   ,int               Length
                   ,int*              RealLength
                   ,ODBCErrorHolder*  ociError
                   ,OracleItems*      p_oraItem   /*=NULL*/
                   ,int               p_maxLength /*=0*/
                   ,bool              p_getData   /*=false*/)
{
  int length = (int)Length;
  int copied = 0;

  if(Length == SQL_NTS)
  {
    // Application flags this as an unbound buffer
    length = 0x7ffffff;
  }
  if(length == 0)
  {
    // Older applications call for the real length
    // Should not happen in ODBC 3.52 sources, but there are 
    // far too many applications around that do this sort of things!!
    *RealLength = (int)strlen((char*)Source);
    return SQL_SUCCESS;
  }
  if(length < 0)
  {
    if(ociError)
    {
      // Invalid string or buffer length
      ociError->AddError("HY090");
    }
    return SQL_ERROR;
  }
  // Initially empty the result
  *Buffer = 0;
  // Calculate max length for GetData parts 
  if(p_getData)
  {
    if(p_maxLength)
    {
      if(length > p_maxLength)
      {
        // Truncate at SQL_ATTR_MAX_LENGTH and flag for application
        // We have no way of knowing how much more there is, because
        // network traffic has been truncated
        length = p_maxLength;
        if(RealLength)
        {
          *RealLength = p_maxLength;
        }
      }
    }
  }
  // Here comes the copy operation
  while(--length && *Source)
  {
    ++copied;
    *Buffer++ = *Source++;
  }
  // Closing zero
  *Buffer = 0;
  // Test truncation in copy buffer and still data at *Source available
  if((length == 0) && *Source)
  {
    if(p_getData && p_oraItem)
    {
      int actualLength = (int)strlen(Source);
      if(RealLength && (!p_maxLength || (actualLength < p_maxLength)))
      {
        *RealLength = actualLength;
      }
      // Next GetData must start at this source pointer
      // and has this much work left to do
      p_oraItem->m_pendingOffset = (int)Source;
      p_oraItem->m_pendingLength  = actualLength;
      return SQL_SUCCESS_WITH_INFO;
    }
    if(ociError)
    {
      // String data, right truncated
      ociError->AddError("01004");
    }
    return SQL_SUCCESS_WITH_INFO;
  }
  // Real copied data length if requested
  if(RealLength)
  {
    *RealLength = copied;
  }
  // Nothing more to be gotten
  if(p_getData && p_oraItem)
  {
    p_oraItem->m_pendingLength = SQL_NULL_DATA;
    p_oraItem->m_pendingOffset = 0;
  }
  return SQL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// DriverToApplication: copy a LOB with all special cases
// Buffer:      Application buffer accepting a memory buffer
// Locator:     OCI Handle to a [C/B]LOB object
// Length:      Application can accept this much data
// RealLength:  Really copied string data
// ociError:    Register our errors here.
// oraItem:     IRD storage of the field of this row
// maxLength:   Maximum data of SQL_ATTR_MAX_LENGTH of the statement
//              After this much we **Must** stop and flag SQL_NO_TOTAL
// getData:     Doing or resuming GetData streaming mode
//
SQLRETURN
CopyODBCLOBToApp(OCISvcCtx*        ServiceContext
                ,SQLPOINTER        Buffer
                ,OCILobLocator*    Locator
                ,int               Length
                ,int*              RealLength
                ,ODBCErrorHolder&  ociError
                ,OracleItems*      p_oraItem
                ,int               p_maxLength
                ,bool              p_getData)
{
  int length = (int)Length;
  int extra  = 0;
  ub4 amount = 0;

  if(length < 0)
  {
    // Data length mismatch
    ociError.AddError("22026");
    return SQL_ERROR;
  }
  // Calculate max length for GetData parts 
  if(p_getData && p_maxLength && (length > p_maxLength))
  {
    // Truncate at SQL_ATTR_MAX_LENGTH and flag for application
    // We have no way of knowing how much more there is, because
    // network traffic has been truncated
    length = p_maxLength + 1;
  }
  if(!p_oraItem->IsBinaryValue())
  {
    extra = 1;
    --length;
  }
  amount = length;
  if(length <= 0)
  {
    // GetData trick to read the length of a column
    // Can be VERY expensive to discover the length in this way
    ub4 LobLength = 0;
    if(OCILobGetLength(ServiceContext,ociError.GetOwnerErrorObject(),Locator,&LobLength) == OCI_SUCCESS)
    {
      // This is the real length
      if(RealLength)
      {
        *RealLength = LobLength;
      }
      return SQL_SUCCESS;
    }
    ociError.GetErrorFromOCIHandle();
    return SQL_ERROR;
  }
  // Open the LOB?
  if(p_oraItem->LobOpen(ServiceContext,ociError,OCI_LOB_READONLY) == SQL_SUCCESS)
  {
    ub4 offset = 1;
    if(p_getData)
    {
      offset += (ub4)(p_oraItem->m_pendingLength);
    }
    sword res = OCILobRead(ServiceContext
                          ,ociError.GetOwnerErrorObject()
                          ,Locator
                          ,(ub4*)&amount
                          ,offset
                          ,(dvoid*)Buffer
                          ,(ub4)length
                          ,NULL       // Context of callback function
                          ,NULL       // Callback function
                          ,0          // Charset ID
                          ,SQLCS_IMPLICIT); // == OCI_SUCCESS)
    if(res == OCI_SUCCESS || res == OCI_SUCCESS_WITH_INFO || res == OCI_NEED_DATA)
    {
      ((SQLCHAR*)Buffer)[amount] = 0;
      if(RealLength)
      {
        *RealLength = amount;
      }
    }
    else
    {
      ociError.GetErrorFromOCIHandle();
    }
    if(amount < (ub4)(Length-1))
    {
      // Read amount < requested. This is the end of the LOB
      // We've got everything
      return p_oraItem->LobClose(ServiceContext,ociError);
    }
    // Still more data pending on this LOB
    p_oraItem->m_pendingLength += amount;
    return SQL_SUCCESS_WITH_INFO;
  }
  // Lob not open
  return SQL_ERROR;
}

// Helper function for "ConvertionalFunction 1 & 2" files
SQLRETURN
TypeConversionError(ODBCErrorHolder& ociError,OracleItems& Holder)
{
  ociError.Clear();
  ociError.AddError("HY000",0,"Type conversion error");
  Holder.m_pendingLength = SQL_NULL_DATA;
  return SQL_ERROR;
}

//////////////////////////////////////////////////////////////////////////
//
// DriverToApplication: copy a BINARY with all special cases
// BinaryBuffer: Pointer to buffer with binary data
// BinaryLength: Indicator of data length in buffer
// Buffer:       Application buffer accepting a memory buffer
// Length:       Application can accept this much data
// RealLength:   Really copied string data
// ociError:     Register our errors here.
// oraItem:      IRD storage of the field of this row
// maxLength:    Maximum data of SQL_ATTR_MAX_LENGTH of the statement
//               After this much we **Must** stop and flag SQL_NO_TOTAL
// getData:      Doing or resuming GetData streaming mode
//
SQLRETURN
CopyODBCRAWToApp(OCIEnv*           Environment
                ,OCIRaw*           raw
                ,SQLPOINTER        Buffer
                ,int               Length
                ,int*              RealLength
                ,ODBCErrorHolder*  ociError
                ,OracleItems*      p_oraItem   /*=NULL*/
                ,int               p_maxLength /*=0*/
                ,bool              p_getData   /*=false*/)
{
  int  length = (int)Length;
  ub4  BinaryLength = OCIRawSize(Environment,raw);
  ub1* BinaryBuffer = OCIRawPtr(Environment,raw);
  ub4  amount = 0;

  if(length <= 0)
  {
    if(ociError)
    {
      // Data length mismatch
      ociError->AddError("22026");
    }
    return SQL_ERROR;
  }
  // Calculate max length for GetData parts 
  if(p_getData)
  {
    if(p_maxLength)
    {
      if(length > p_maxLength)
      {
        // Truncate at SQL_ATTR_MAX_LENGTH and flag for application
        // We have no way of knowing how much more there is, because
        // network traffic has been truncated
        length = p_maxLength;
        if(RealLength)
        {
          *RealLength = p_maxLength;
        }
      }
    }
  }
  amount = (length - 1);
  if(amount <= 0 && p_getData)
  {
    // ODBC Trick to learn the length of the data available
    // This is the real length
    if(RealLength)
    {
      *RealLength = BinaryLength;
    }
    // Binary data truncated
    ociError->AddError("01005");  
    return SQL_SUCCESS_WITH_INFO;
  }
  ub4 copyLength = BinaryLength;
  if(BinaryLength > amount)
  {
    copyLength = length;
  }
  // THIS IS THE MEMCOPY
  memcpy(Buffer,BinaryBuffer,copyLength);

  if(RealLength)
  {
    *RealLength = copyLength;
  }
  if(copyLength == BinaryLength)
  {
    if(p_oraItem)
    {
      p_oraItem->m_pendingLength = SQL_NULL_DATA;
      p_oraItem->m_pendingOffset = 0;
    }
    return SQL_SUCCESS;
  }
  // Still more data pending on this binary 
  p_oraItem->m_pendingOffset = (int)((char*)BinaryBuffer + copyLength);
  p_oraItem->m_pendingLength  = BinaryLength - copyLength;
  // Binary data truncated
  ociError->AddError("01005");  
  return SQL_SUCCESS_WITH_INFO;
}

// Make pattern for OCI Number conversion
// Following Oracle conventions
// -123.456E+22 -> S999.999EEEE
// 12345.67890  -> 99999.99999
// +234.23      -> S999.99
void
MakeNumericPattern(char* buffer,char* pattern)
{
  char* pnt = buffer;
  if(*pnt == '-' || *pnt == '+')
  {
    ++pnt;
    *pattern++ = 'S';
  }
  while(*pnt)
  {
    if(toupper(*pnt) == 'E')
    {
      strcat_s(pattern,5,"EEEE");
      pattern += 4;
      break;
    }
    *pattern = (*pnt == '.') ? 'D' : '9';
    ++pattern;
    ++pnt;
  }
  *pattern = 0;
}

void
DoubleToString(char* buffer,double value)
{
  // Test for ISO:9075 border values = 1.0E-32 / 1.0E+33
  // INTEL x86 double is smaller. So we use that. Sigh
  if(value < 1.0E-16 || value >= 1.0E+17)
  {
    sprintf_s(buffer,40,"%.16lG",value);
  }
  else
  {
    sprintf_s(buffer,40,"%.16lf",value);
    // Strip trailing "0000" or ".0000"
    int len = (int) strlen(buffer) - 1;
    char* pnt = &buffer[len];
    while(*pnt=='0' && len-- >= 0)
    {
      *pnt-- = 0;
    }
    if(*pnt == '.')
    {
      *pnt = 0;
    }
  }
}

void
FloatToString(char* buffer,float value)
{
  // Test for ISO:9075 border values = 1.0E-32 / 1.0E+33
  // INTEL x86 float is smaller. So we use that. Sigh
  if(value < 1.0E-8 || value >= 1.0E+9)
  {
    sprintf_s(buffer,40,"%.8G",value);
  }
  else
  {
    sprintf_s(buffer,40,"%.8f",value);
    // Strip trailing "0000" or ".0000"
    int len = (int) strlen(buffer) - 1;
    char* pnt = &buffer[len];
    while(*pnt=='0' && len-- >= 0)
    {
      *pnt-- = 0;
    }
    if(*pnt == '.')
    {
      *pnt = 0;
    }
  }
}

#pragma warning (disable: 4996)

bool
StringToDouble(char* p_string,double* value)
{
  if(strtok(p_string,"eE"))
  {
    if(_sscanf_s_l(p_string,"%lG",ODBC_Locale,value) == 1)
    {
      return true;
    }
  }
  else
  {
    if(_sscanf_s_l(p_string,"%lf",ODBC_Locale,value) == 1)
    {
      return true;
    }
  }
  return false;
}

bool
StringToFloat(char* p_string,float* value)
{
  if(strtok(p_string,"eE"))
  {
    if(_sscanf_s_l(p_string,"%G",ODBC_Locale,value) == 1)
    {
      return true;
    }
  }
  else
  {
    if(_sscanf_s_l(p_string,"%f",ODBC_Locale,value) == 1)
    {
      return true;
    }
  }
  return false;
}

// "DATE" or "DATE TIME" string to OCIDate
SQLRETURN
StringToDate(ODBCConnection*  p_connection
            ,char*            p_string
            ,OCIDate*         p_date
            ,ODBCErrorHolder* ociError)
{
  // Standard = "NLS_DATE_FORMAT" string
  CString format = p_connection->GetNLSDateFormat();
  bool two = (strchr(p_string,' ') != NULL);
  if(two)
  {
    // Use "NLS_DATE_FORMAT NLS_TIME_FORMAT" string
    format += " " + p_connection->GetNLSTimeFormat();
  }
  if(OCIDateFromText(ociError->GetOwnerErrorObject()
                    ,(const oratext*)p_string
                    ,(ub4)strlen(p_string)
                    ,(const oratext*)format.GetString()
                    ,(ub1)format.GetLength()
                    ,NULL
                    ,0
                    ,p_date) != OCI_SUCCESS)
  {
    ociError->GetErrorFromOCIHandle();
    return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

// Convert OCIDate to string with NLS_ formatting
SQLRETURN
DateToString(ODBCConnection*  p_connection
            ,OCIDate*         p_date
            ,char*            p_string
            ,int              p_bufsize
            ,ODBCErrorHolder* p_ociError)
{
  CString format;
  ub1 hour,min,sec;
  OCIDateGetTime(p_date,&hour,&min,&sec);
  if(hour == 0 && min == 0 && sec == 0)
  {
    format = p_connection->GetNLSDateFormat();
  }
  else
  {
    format = p_connection->GetNLSTimestampFormat();
  }
  if(OCIDateToText(p_ociError->GetOwnerErrorObject()
                  ,p_date
                  ,(const oratext*)format.GetString()
                  ,(ub1)format.GetLength()
                  ,NULL,0
                  ,(ub4*)&p_bufsize
                  ,(oratext*)p_string) != OCI_SUCCESS)
  {
    p_ociError->GetErrorFromOCIHandle();
    return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

// "Timestamp" or "Timestamp TZ" to OCIDateTime
SQLRETURN 
StringToTimestamp(ODBCConnection*  p_connection
                 ,char*            p_string
                 ,bool             p_timezone
                 ,OCIDateTime*     p_datetime
                 ,ODBCErrorHolder* ociError)
{
  CString format;
  if(p_timezone)
  {
    format = p_connection->GetNLSTimestampFormatTZ();
  }
  else
  {
    format = p_connection->GetNLSTimestampFormat();
  }
  OCIEnv* environment = p_connection->GetOwnerODBCEnv()->GetOwnerEnv();
  if(OCIDateTimeFromText(environment
                        ,ociError->GetOwnerErrorObject()
                        ,(const oratext*)p_string
                        ,strlen(p_string)
                        ,(const oratext*)format.GetString()
                        ,(ub1)format.GetLength()
                        ,NULL
                        ,0
                        ,p_datetime) != OCI_SUCCESS)
  {
    ociError->GetErrorFromOCIHandle();
    return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

// Timestamp conversion back to string
SQLRETURN
TimestampToString(ODBCConnection*  p_connection
                 ,OCIDateTime*     p_datetime
                 ,char*            p_string
                 ,int              p_bufsize
                 ,bool             p_timezone
                 ,ODBCErrorHolder* p_ociError)
{
  CString format;
  if(p_timezone)
  {
    format = p_connection->GetNLSTimestampFormatTZ();
  }
  else
  {
    format = p_connection->GetNLSTimestampFormat();
  }
  if(OCIDateTimeToText(p_connection->GetOwnerODBCEnv()->GetOwnerEnv()
                      ,p_ociError->GetOwnerErrorObject()
                      ,p_datetime
                      ,(const oratext*) format.GetString()
                      ,(ub1)format.GetLength()
                      ,6
                      ,NULL,0
                      ,(ub4*)&p_bufsize
                      ,(oratext*)p_string) == OCI_SUCCESS)
  {
    p_ociError->GetErrorFromOCIHandle();
    return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
//  SQL_NUMERIC_STRUCT and conversions
//
//  Code was taken from the MySQL ODBC driver version 5.1.4r1107
//  See utility.h and utility.cpp at www.mysql.org
//
//////////////////////////////////////////////////////////////////////////

// Retrieve a SQL_NUMERIC_STRUCT from a string. The requested scale
// and precision are first read from sqlnum, and then updated values
// are written back at the end.
// 
// @param[in] numstr       String representation of number to convert
// @param[in] sqlnum       Destination struct
// @param[in] overflow_ptr Whether or not whole-number overflow occurred.
//                         This indicates failure, and the result of sqlnum
//                         is undefined.
void 
StringToNumeric(const char*         numstr
               ,SQL_NUMERIC_STRUCT* sqlnum
               ,int*                overflow_ptr)
{
  /*
  We use 16 bits of each integer to convert the
  current segment of the number leaving extra bits
  to multiply/carry
  */
  int build_up[8], tmp_prec_calc[8];
  /* current segment as integer */
  unsigned int curnum;
  /* current segment digits copied for strtoul() */
  char curdigs[5];
  /* number of digits in current segment */
  int usedig;
  int i;
  int len;
  char *decpt = (char*)strchr(numstr, '.');
  int overflow= 0;
  SQLSCHAR reqscale = sqlnum->scale;
  SQLCHAR  reqprec  = sqlnum->precision;

  memset(&sqlnum->val, 0, sizeof(sqlnum->val));
  memset(build_up, 0, sizeof(build_up));

  /* handle sign */
  if (0 == (sqlnum->sign = !(*numstr == '-')))
  {
    ++numstr;
  }
  len= (int) strlen(numstr);
  sqlnum->precision = (sb1)len;
  sqlnum->scale= 0;

  /* process digits in groups of <=4 */
  for (i= 0; i < len; i += usedig)
  {
    if (i + 4 < len)
    {
      usedig = 4;
    }
    else
    {
      usedig = len - i;
    }
    // if we have the decimal point, ignore it by setting it to the
    // last char (will be ignored by strtoul)
    if (decpt && decpt >= numstr + i && decpt < numstr + i + usedig)
    {
      usedig = (int) (decpt - (numstr + i) + 1);
      sqlnum->scale = (sb1) (len - (i + usedig));
      sqlnum->precision--;
      decpt= NULL;
    }
    /* terminate prematurely if we can't do anything else */
    /*if (overflow && !decpt)
    break;
    else */if (overflow)
      /*continue;*/goto end;
    /* grab just this piece, and convert to int */
    memcpy(curdigs, numstr + i, usedig);
    curdigs[usedig]= 0;
    curnum= strtoul(curdigs, NULL, 10);
    if (curdigs[usedig - 1] == '.')
    {
      sqlnum_scale(build_up, usedig - 1);
    }
    else
    {
      sqlnum_scale(build_up, usedig);
    }
    /* add the current number */
    build_up[0] += curnum;
    sqlnum_carry(build_up);
    if (build_up[7] & ~0xffff)
    {
      overflow = 1;
    }
  }
  /* scale up to SQL_DESC_SCALE */
  if (reqscale > 0 && reqscale > sqlnum->scale)
  {
    while (reqscale > sqlnum->scale)
    {
      sqlnum_scale(build_up, 1);
      sqlnum_carry(build_up);
      sqlnum->scale++;
    }
  }
  /* scale back, truncating decimals */
  else if (reqscale < sqlnum->scale)
  {
    while (reqscale < sqlnum->scale && sqlnum->scale > 0)
    {
      sqlnum_unscale_le(build_up);
      build_up[0] /= 10;
      sqlnum->precision--;
      sqlnum->scale--;
    }
  }
  /* scale back whole numbers while there's no significant digits */
  if (reqscale < 0)
  {
    memcpy(tmp_prec_calc, build_up, sizeof(build_up));
    while (reqscale < sqlnum->scale)
    {
      sqlnum_unscale_le(tmp_prec_calc);
      if (tmp_prec_calc[0] % 10)
      {
        overflow= 1;
        goto end;
      }
      sqlnum_unscale_le(build_up);
      tmp_prec_calc[0] /= 10;
      build_up[0] /= 10;
      sqlnum->precision--;
      sqlnum->scale--;
    }
  }
  /* calculate minimum precision */
  memcpy(tmp_prec_calc, build_up, sizeof(build_up));
  do
  {
    sqlnum_unscale_le(tmp_prec_calc);
    i= tmp_prec_calc[0] % 10;
    tmp_prec_calc[0] /= 10;
    if (i == 0)
    {
      sqlnum->precision--;
    }
  } 
  while (i == 0 && sqlnum->precision > 0);

  /* detect precision overflow */
  if (sqlnum->precision > reqprec)
  {
    overflow= 1;
  }
  else
  {
    sqlnum->precision = reqprec;
  }
  /* compress results into SQL_NUMERIC_STRUCT.val */
  for (i= 0; i < 8; ++i)
  {
    int elem= 2 * i;
    sqlnum->val[elem]   =  build_up[i] & 0xff;
    sqlnum->val[elem+1] = (build_up[i] >> 8) & 0xff;
  }
end:
  if (overflow_ptr)
  {
    *overflow_ptr = overflow;
  }
}

// Scale an int[] representing SQL_C_NUMERIC
// 
// @param[in] ary   Array in little endian form
// @param[in] s     Scale
static void 
sqlnum_scale(int *ary, int s)
{
  /* multiply out all pieces */
  while (s--)
  {
    ary[0] *= 10;
    ary[1] *= 10;
    ary[2] *= 10;
    ary[3] *= 10;
    ary[4] *= 10;
    ary[5] *= 10;
    ary[6] *= 10;
    ary[7] *= 10;
  }
}

// Unscale an int[] representing SQL_C_NUMERIC. This
// leaves the last element (0) with the value of the
// last digit.
// 
// @param[in] ary   Array in little endian form
static void 
sqlnum_unscale_le(int *ary)
{
  int i;
  for (i= 7; i > 0; --i)
  {
    ary[i - 1] += (ary[i] % 10) << 16;
    ary[i] /= 10;
  }
}

// Unscale an int[] representing SQL_C_NUMERIC. This
// leaves the last element (7) with the value of the
// last digit.
// 
// @param[in] ary   Array in big endian form
static void 
sqlnum_unscale_be(int *ary, int start)
{
  int i;
  for (i= start; i < 7; ++i)
  {
    ary[i + 1] += (ary[i] % 10) << 16;
    ary[i] /= 10;
  }
}

// Perform the carry to get all elements below 2^16.
// Should be called right after sqlnum_scale().
// 
// @param[in] ary   Array in little endian form
static void 
sqlnum_carry(int *ary)
{
  int i;
  /* carry over rest of structure */
  for (i= 0; i < 7; ++i)
  {
    ary[i+1] += ary[i] >> 16;
    ary[i] &= 0xffff;
  }
}

// Convert a SQL_NUMERIC_STRUCT to a string. Only val and sign are
// read from the struct. precision and scale will be updated on the
// struct with the final values used in the conversion.
// 
// @param[in] sqlnum       Source struct
// @param[in] numstr       Buffer to convert into string. Note that you
//                         MUST use numbegin to read the result string.
//                         This should point to the LAST byte available.
//                         (We fill in digits backwards.)
// @param[in] numbegin     String pointer that will be set to the start of
//                         the result string.
// @param[in] reqprec      Requested precision
// @param[in] reqscale     Requested scale
// @param[in] truncptr     Pointer to set the truncation type encountered.
//                         If SQLNUM_TRUNC_WHOLE, this indicates a failure
//                         and the contents of numstr are undefined and
//                         numbegin will not be written to.
void 
NumericToString(SQL_NUMERIC_STRUCT* sqlnum
               ,SQLCHAR*            numstr
               ,SQLCHAR**           numbegin
               ,SQLCHAR             reqprec
               ,SQLSCHAR            reqscale
               ,int*                truncptr)
{
  int expanded[8];
  int i, j;
  int max_space= 0;
  int calcprec= 0;
  int trunc= 0; /* truncation indicator */

  *numstr--= 0;

  /*
  it's expected to have enough space
  (~at least min(39, max(prec, scale+2)) + 3)
  */

  /*
  expand the packed sqlnum->val so we have space to divide through
  expansion happens into an array in big-endian form
  */
  for (i= 0; i < 8; ++i)
  {
    expanded[7 - i] = (sqlnum->val[(2 * i) + 1] << 8) | sqlnum->val[2 * i];
  }
  /* max digits = 39 = log_10(2^128)+1 */
  for (j= 0; j < 39; ++j)
  {
    /* skip empty prefix */
    while (!expanded[max_space])
    {
      max_space++;
    }
    /* if only the last piece has a value, it's the end */
    if (max_space >= 7)
    {
      i= 7;
      if (!expanded[7])
      {
        /* special case for zero, we'll end immediately */
        if (!*(numstr + 1))
        {
          *numstr--= '0';
          calcprec= 1;
        }
        break;
      }
    }
    else
    {
      /* extract the next digit */
      sqlnum_unscale_be(expanded, max_space);
    }
    *numstr--= '0' + (expanded[7] % 10);
    expanded[7] /= 10;
    calcprec++;
    if (j == reqscale - 1)
    {
      *numstr--= '.';
    }
  }
  sqlnum->scale= reqscale;
  /* add <- dec pt */
  if (calcprec < reqscale)
  {
    while (calcprec < reqscale)
    {
      *numstr--= '0';
      reqscale--;
    }
    *numstr--= '.';
    *numstr--= '0';
  }
  /* handle fractional truncation */
  if (calcprec > reqprec && reqscale > 0)
  {
    SQLCHAR *end= numstr + strlen((char *)numstr) - 1;
    while (calcprec > reqprec && reqscale)
    {
      *end--= 0;
      calcprec--;
      reqscale--;
    }
    if (calcprec > reqprec && reqscale == 0)
    {
      trunc = SQLNUM_TRUNC_WHOLE;
      goto end;
    }
    if (*end == '.')
    {
      *end--= 0;
    }
    else
    {
      /* move the dec pt-- ??? */
      /*
      char c2, c= numstr[calcprec - reqscale];
      numstr[calcprec - reqscale]= '.';
      while (reqscale)
      {
      c2= numstr[calcprec + 1 - reqscale];
      numstr[calcprec + 1 - reqscale]= c;
      c= c2;
      reqscale--;
      }
      */
    }
    trunc = SQLNUM_TRUNC_FRAC;
  }
  /* add zeros for negative scale */
  if (reqscale < 0)
  {
    int i;
    reqscale *= -1;
    for (i= 1; i <= calcprec; ++i)
    {
      *(numstr + i - reqscale) = *(numstr + i);
    }
    numstr -= reqscale;
    memset(numstr + calcprec + 1, '0', reqscale);
  }
  sqlnum->precision = (sb1)calcprec;

  /* finish up, handle auxiliary fix-ups */
  if (!sqlnum->sign)
  {
    *numstr--= '-';
  }
  numstr++;
  *numbegin= numstr;

end:
  if (truncptr)
  {
    *truncptr= trunc;
  }
}


//////////////////////////////////////////////////////////////////////////
//
// DRIVER TO RDBMS
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// DriverToRDBMS: copy a string with all special cases
// 
// Environment  Needed for database locale
// Source:      Driver or datasource source of the string
// Length:      Application can accept this much data
// RealLength:  Really copied string data
// ociError:    Register our errors here.
// oraItem:     IRD storage of the field of this row
// maxLength:   Maximum data of SQL_ATTR_MAX_LENGTH of the statement
//              After this much we **Must** stop and flag SQL_NO_TOTAL
//
SQLRETURN
CopyODBCStringToRDBMS(OCIEnv*           p_environment
                     ,char*             Source
                     ,int               Length
                     ,int*              RealLength
                     ,ODBCErrorHolder*  ociError
                     ,OracleItems&      p_oraItem
                     ,int               p_maxLength)
{
  int length = (int)Length;

  if(Length == SQL_NTS)
  {
    // Application flags this as an unbound buffer
    length = (int)strlen(Source);
  }
  if(length <= 0)
  {
    if(ociError)
    {
      // String data length mismatch
      ociError->AddError("22026");
    }
    return SQL_ERROR;
  }
  // Calculate max length for PutData parts 
  if(length > p_maxLength)
  {
    length = p_maxLength;
  }
  if(OCIStringAssignText(p_environment
                        ,ociError->GetOwnerErrorObject()
                        ,(const oratext*)Source
                        ,length
                        ,&(p_oraItem.Value.string_val)) != OCI_SUCCESS)
  {
    ociError->GetErrorFromOCIHandle();
    return SQL_ERROR;
  }
  // Real copied data length if requested
  if(RealLength)
  {
    *RealLength = length;
  }
  // Test truncation in copy buffer and still data at *Source available
  if(p_maxLength && (Length > p_maxLength))
  {
    // String data, right truncated
    ociError->AddError("01004");
    return SQL_SUCCESS_WITH_INFO;
  }
  // Nothing more to be gotten
  return SQL_SUCCESS;
}


SQLRETURN
CopyODBCRAWToRDBMS(OCIEnv*           Environment
                  ,OCIRaw*           raw
                  ,SQLPOINTER        Buffer
                  ,int               Length
                  ,int*              RealLength
                  ,ODBCErrorHolder*  ociError
                  ,int               p_maxLength)
{
  int  length = Length;
  ub4  BinaryLength = OCIRawSize(Environment,raw);

  if(length <= 0)
  {
    if(ociError)
    {
      // Data length mismatch
      ociError->AddError("22026");
    }
    return SQL_ERROR;
  }
  if(BinaryLength != (ub4)length)
  {
    sword ociRes = OCIRawResize(Environment,ociError->GetOwnerErrorObject(),length,&raw);
    if(ociRes != OCI_SUCCESS)
    {
      ociError->GetErrorFromOCIHandle();
      return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
    }
  }
  // Calculate max length for GetData parts 
  if(p_maxLength && (length > p_maxLength))
  {
    length = p_maxLength;
  }
  // Here comes the real copy
  sword ociRes;
  ociRes = OCIRawAssignBytes(Environment
                            ,ociError->GetOwnerErrorObject()
                            ,(ub1*)Buffer
                            ,length
                            ,&raw);
  if(ociRes != OCI_SUCCESS)
  {
    ociError->GetErrorFromOCIHandle();
    return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
  }
  if(RealLength)
  {
    *RealLength = length;
  }
  if(p_maxLength && (Length > p_maxLength))
  {
    // Binary truncation
    ociError->AddError("01005");
    return SQL_SUCCESS_WITH_INFO;
  }
  return SQL_SUCCESS;
}

// Insert a CLOB in one piece
SQLRETURN
CopyODBCLOBToRDBMS(OCISvcCtx*        ServiceContext
                  ,OCILobLocator*    Locator
                  ,SQLPOINTER        Buffer
                  ,int               Length
                  ,int*              RealLength
                  ,ODBCErrorHolder&  ociError
                  ,OracleItems*      p_oraItem)
{
  if(Length <= 0)
  {
    // Data length mismatch
    ociError.AddError("22026");
    return SQL_ERROR;
  }
  if(p_oraItem->LobOpen(ServiceContext,ociError,(ub1)OCI_LOB_READWRITE) != SQL_SUCCESS)
  {
    // LOB not open
    return SQL_ERROR;
  }
  OCIError* error = ociError.GetOwnerErrorObject();
  sword res = OCILobWrite(ServiceContext
                         ,error
                         ,Locator
                         ,(ub4*) &Length
                         ,(ub4)   1
                         ,(dvoid*)Buffer
                         ,(ub4)   Length
                         ,(ub1)   OCI_ONE_PIECE
                         ,(dvoid*)0               // Context of callback function
                         ,NULL                    // Call back
                         ,(ub2) 0                 // CSID
                         ,(ub1) SQLCS_IMPLICIT);  // == OCI_SUCCESS)
  if(res == OCI_SUCCESS || res == OCI_SUCCESS_WITH_INFO || res == OCI_NEED_DATA)
  {
    if(RealLength)
    {
      // Amount is correct last time called
      *RealLength = Length;
    }
  }
  else
  {
    ociError.GetErrorFromOCIHandle();
  }
  // One piece. Close lob again
  return p_oraItem->LobClose(ServiceContext,ociError);
}

SQLRETURN
CopyODBCBufferToRDBMS(char*             p_buffer
                     ,int               p_bufLength
                     ,char*             p_source
                     ,int               p_length
                     ,int*              p_realLength
                     ,ODBCErrorHolder&  p_ociError
                     ,OracleItems&      p_oraItem)
{
  int length = (int)p_length;

  if(p_length == SQL_NTS)
  {
    // Application flags this as an unbound buffer
    length = (int) strlen(p_source) + 1;
  }
  if(length <= 0)
  {
    // String data length mismatch
    p_ociError.AddError("22026");
    return SQL_ERROR;
  }
  if(p_length > p_bufLength)
  {
    length = p_bufLength;
  }
  // The real copy!
  memcpy(p_buffer,p_source,length);
  // Real copied data length if requested
  if(p_realLength)
  {
    *p_realLength = length;
  }
  p_oraItem.m_pendingLength  = SQL_NULL_DATA;
  p_oraItem.m_pendingOffset  = 0;
  // Test truncation in copy buffer 
  if(length < p_length)
  {
    // String data, right truncated
    p_ociError.AddError("01004");
    return SQL_SUCCESS_WITH_INFO;
  }
  return SQL_SUCCESS;
}
