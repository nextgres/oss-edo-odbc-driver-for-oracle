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
// Contents:  prototypes and definitions for the utility conversion routines
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "ErrorHolder.h"
#include "DescriptorIRD.h"
#include <oci.h>
#include <sqlext.h>

// Truncation types in SQL_NUMERIC_STRUCT conversions
#define SQLNUM_TRUNC_FRAC  1
#define SQLNUM_TRUNC_WHOLE 2
// Max length of a NUMERIC
#define SQLNUM_MAXLEN      65

#define DEF_INT_AND_TEXT_VAL(x) x,#x

// For static OCINumberToText and OCITextToNumber conversions
extern const char* NLS_NUMERIC_CHARACTERS;
extern const int   NLS_LENGTH;

#ifdef EDO_LOGGING

typedef struct tagSQLValues
{
  SQLSMALLINT ODBC_Type;
  const char *Str;
}
SQLValues;

const char *ConvertSQL_C_TypeToString(SQLSMALLINT ODBC_Type);
const char *ConvertSQL_TypeToString  (SQLSMALLINT ODBC_Type);

#endif // EDO_LOGGING

// Get a variable from a connection string in SQLDriverConnect / SQLBrowseConnect
int       GetVariableFromConnectionString(CString& p_string,char* p_var,char* p_buffer,int p_buflen);
// Type conversion not found (SQL_<TYPE> / OCI TYPE) combination
SQLRETURN TypeConversionError(ODBCErrorHolder& ociError,OracleItems& Holder);

// Make a driver CString out of a pointer and an optional length
CString MakeDriverString(SQLCHAR* p_string,SQLINTEGER p_length);
// Make pattern for OCI Number conversion
void MakeNumericPattern(char* buffer,char* pattern);
// Convert native DOUBLE to string
void DoubleToString(char* buffer,double value);
// Convert native FLOAT to string
void FloatToString(char* buffer,float value);
// Convert string to native DOUBLE
bool StringToDouble(char* p_string,double* value);
// Convert string to native FLOAT
bool StringToFloat(char* p_string,float* value);

// Convert String to OCIDate with NLS_DATE_FORMAT'ing
SQLRETURN StringToDate(ODBCConnection*  p_connection
                      ,char*            p_string
                      ,OCIDate*         p_date
                      ,ODBCErrorHolder* ociError);
// Convert OCIDate to string with NLS_ formatting
SQLRETURN DateToString(ODBCConnection*  p_connection
                      ,OCIDate*         p_date
                      ,char*            p_string
                      ,int              p_bufsize
                      ,ODBCErrorHolder* p_ociError);
// Convert String to OCIDateTime with NLS formatting
SQLRETURN StringToTimestamp(ODBCConnection*  p_connection
                           ,char*            p_string
                           ,bool             p_timezone
                           ,OCIDateTime*     p_datetime
                           ,ODBCErrorHolder* ociError);
// Timestamp conversion back to string
SQLRETURN TimestampToString(ODBCConnection*  p_connection
                           ,OCIDateTime*     p_datetime
                           ,char*            p_string
                           ,int              p_bufsize
                           ,bool             p_timezone
                           ,ODBCErrorHolder* p_ociError);

// Data flows FROM the ODBC driver to the application in string form
SQLRETURN CopyODBCStringToApp (SQLCHAR*         Buffer
                              ,char*            Source
                              ,int              Length
                              ,int*             RealLength
                              ,ODBCErrorHolder* ociError
                              ,OracleItems*     p_oraItem   = NULL
                              ,int              p_maxLength = 0
                              ,bool             p_getData   = false);
// Data flows FROM the ODBC driver to the application in binary form
SQLRETURN CopyODBCLOBToApp   (OCISvcCtx*        ServiceContext
                             ,SQLPOINTER        Buffer
                             ,OCILobLocator*    Locator
                             ,int               Length
                             ,int*              RealLength
                             ,ODBCErrorHolder&  ociError
                             ,OracleItems*      p_oraItem
                             ,int               p_maxLength
                             ,bool              p_getData);
SQLRETURN CopyODBCRAWToApp   (OCIEnv*           Environment
                             ,OCIRaw*           raw
                             ,SQLPOINTER        Buffer
                             ,int               Length
                             ,int*              RealLength
                             ,ODBCErrorHolder*  ociError
                             ,OracleItems*      p_oraItem   = NULL
                             ,int               p_maxLength = 0
                             ,bool              p_getData   = false);

// Copy from the driver to the RDBMS

SQLRETURN CopyODBCStringToRDBMS(OCIEnv*           p_environment
                               ,char*             Source
                               ,int               Length
                               ,int*              RealLength
                               ,ODBCErrorHolder*  ociError
                               ,OracleItems&      p_oraItem
                               ,int               p_maxLength = 0);
SQLRETURN CopyODBCRAWToRDBMS   (OCIEnv*           Environment
                               ,OCIRaw*           raw
                               ,SQLPOINTER        Buffer
                               ,int               Length
                               ,int*              RealLength
                               ,ODBCErrorHolder*  ociError
                               ,int               p_maxLength);
SQLRETURN CopyODBCLOBToRDBMS   (OCISvcCtx*        ServiceContext
                               ,OCILobLocator*    Locator
                               ,SQLPOINTER        Buffer
                               ,int               Length
                               ,int*              RealLength
                               ,ODBCErrorHolder&  ociError
                               ,OracleItems*      p_oraItem);
SQLRETURN CopyODBCBufferToRDBMS(char*             p_buffer
                               ,int               p_bufLength
                               ,char*             p_source
                               ,int               p_length
                               ,int*              p_realLength
                               ,ODBCErrorHolder&  p_ociError
                               ,OracleItems&      p_oraItem);

// NUMERIC INTERFACE

static void sqlnum_scale(int *ary, int s);
static void sqlnum_unscale_le(int *ary);
static void sqlnum_unscale_be(int *ary, int start);
static void sqlnum_carry(int *ary);
void 
StringToNumeric(const char*         numstr
               ,SQL_NUMERIC_STRUCT* sqlnum
               ,int*                overflow_ptr);
void 
NumericToString(SQL_NUMERIC_STRUCT* sqlnum
               ,SQLCHAR*            numstr
               ,SQLCHAR**           numbegin
               ,SQLCHAR             reqprec
               ,SQLSCHAR            reqscale
               ,int*                truncptr);
