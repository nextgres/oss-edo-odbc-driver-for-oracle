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
// Contents:  Interface of the ErrorHolder
//            Holds a set of errors for an ODBCBaseObject
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include <oci.h>

// Count of internal Error records in ODBCErrorHolder::InternalRecord
#define ODBCErrorRecCounts 5

class ODBCBaseObject;

//f Class for representing ODBC errors records
class ODBCErrorHolder
{
public:	
  ODBCErrorHolder(OCIEnv *hEnv);
  ODBCErrorHolder();
  ~ODBCErrorHolder();
  void Done();
  void Clear();

  int         DiagRecords();
  int         GetErrorRecord();
  int         AddError(const char *SQLState,int InternalErrorCode = 0,const char *ErrorText = "");
  OCIError*   GetOwnerErrorObject();
  int         GetErrorFromOCIHandle();
  void        SetDSNName(const char *Name);
  const char* GetDSNName();
  SQLRETURN   SQLGetDiagRec(SQLSMALLINT  RecNumber
                           ,SQLCHAR*     Sqlstate
                           ,SQLINTEGER*  NativeError
                           ,SQLCHAR*     MessageText
                           ,SQLSMALLINT  BufferLength
                           ,SQLSMALLINT* TextLength);
  SQLRETURN SQLGetDiagField(SQLSMALLINT   HandleType
                           ,ODBCBaseObject* obj
                           ,SQLSMALLINT   RecNumber
                           ,SQLSMALLINT   DiagIdentifier
                           ,SQLPOINTER    DiagInfo
                           ,SQLSMALLINT   BufferLength
                           ,SQLSMALLINT*  StringLength);
protected:
  OCIError* m_hError;           // OCI functions Error Holders
  struct    m_errorRecord
  {
    char    SQLState[6];        // Standard ODBC SQLSTATE field
    int     InternalErrorCode;  // Internal driver error code
    CString ErrorTextString;    // Text description of code
  } 
  m_InternalRecord[ODBCErrorRecCounts]; // internal error record

  CString m_DSNName;                    // DSN name if present
  int     m_CountOfInternalRecords;     // Count of internal error records is filed
  int     m_CountOfRecordsReported;     // Reported by SQLError
};

inline int
ODBCErrorHolder::DiagRecords()
{
  return m_CountOfInternalRecords;
}

inline OCIError* 
ODBCErrorHolder::GetOwnerErrorObject()
{
  return m_hError;
}

inline void 
ODBCErrorHolder::SetDSNName(const char *Name)
{
  m_DSNName=Name;
}

inline const char* 
ODBCErrorHolder::GetDSNName()
{
  return m_DSNName.GetString();
}
