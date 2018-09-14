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
// Contents:  Interface for the Internal Descriptor
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "BaseObject.h"
#include <oci.h>

// For resetting the internal arrays
#define RESET_IRDARD_DESCRIPTOR -2
// Blocks of allocated fields
#define DESCRIPTOR_BLOCK_SIZE   15

//ORACLE Types enumerations
enum OraTypesEnumeration
{
  UNKNOWN_TYPE      =  0,  // Unknown type
  DATE_TYPE         =  1,  // ORACLE DATE type
  NUMBER_TYPE       =  2,  // ORACLE NUMBER (INT,SMALLINT,FLOAT ...) type
  STRING_TYPE       =  3,  // ORACLE STRING (CHAR,VARCHAR2 ...) type
  DATETIME_TYPE     =  4,  // ORACLE TIMESTAMP type
  INTERVALYM_TYPE   =  5,  // ORACLE INTERVAL YEAR TO MONTH type
  ROWID_TYPE        =  6,  // ORACLE ROWID type
  ANSI_DATE_TYPE    =  7,  // ORACLE ANSI DATE type
  DATETIMEWTZ_TYPE  =  8,  // ORACLE DATETIME with TIME ZONE type
  DATETIMEWLTZ_TYPE =  9,  // ORACLE DATETIME with LOCAL TIME ZONE type
  INTERVALDS_TYPE   = 10,  // ORACLE INTERVAL DAY TO SECOND type
  INTEGER_TYPE      = 11,  // ORACLE INTEGER type
  DOUBLE_TYPE       = 12,  // ORACLE FLOAT type
  CLOB_TYPE         = 13,  // ORACLE CLOB type
  BLOB_TYPE         = 14,  // ORACLE BLOB type
  LONG_TYPE         = 15,  // ORACLE LONG type
  RAW_TYPE          = 16,  // ORACLE RAW type
  LONGRAW_TYPE      = 17,  // ORACLE LONG RAW type
};

class ODBCStatement;
class ODBCConnection;

// Base interface for SQL_HANDLE_DESC type (SQLHDESC)
// Describe all base call for ODBC functions with SQLHDESC input descriptor

class ODBCDescriptor:public ODBCBaseObject
{
public:
  // Constructor		
  ODBCDescriptor(ODBCConnection* p_connection
                ,ODBCStatement*  p_statement
                ,SQLSMALLINT     p_alloc_type
                ,bool            p_row);	
  // Destructor
  virtual ~ODBCDescriptor();
  // Validation of Description object, -1 if not valid object or SQL_HANDLE_DESC
  int ValidHandle();
  // Get count of items in descriptor
  int GetRecCount();
  // Is allocated by the application
  bool IsInternal();
  // Is a row or parameter descriptor
  bool IsRowDescriptor();
  // Get the status array pointer
  SQLUSMALLINT* GetArrayStatusPointer();


  // ODBC CALLS
  virtual SQLRETURN SQLGetDescField(SQLSMALLINT  RecNumber
                                   ,SQLSMALLINT  FieldIdentifier
                                   ,SQLPOINTER   Value
                                   ,SQLINTEGER   BufferLength
                                   ,SQLINTEGER*  StringLength) = 0;
  virtual SQLRETURN SQLSetDescField(SQLSMALLINT  RecNumber
                                   ,SQLSMALLINT  FieldIdentifier
                                   ,SQLPOINTER   Value
                                   ,SQLINTEGER   BufferLength) = 0;
  virtual SQLRETURN SQLGetDescRec  (SQLSMALLINT  RecNumber
                                   ,SQLCHAR*     Name
                                   ,SQLSMALLINT  BufferLength
                                   ,SQLSMALLINT* StringLength
                                   ,SQLSMALLINT* Type
                                   ,SQLSMALLINT* SubType
                                   ,SQLLEN*      Length
                                   ,SQLSMALLINT* Precision
                                   ,SQLSMALLINT* Scale
                                   ,SQLSMALLINT* Nullable) = 0;
  virtual SQLRETURN SQLSetDescRec  (SQLSMALLINT  RecNumber
                                   ,SQLSMALLINT  Type
                                   ,SQLSMALLINT  SubType
                                   ,SQLINTEGER   Length
                                   ,SQLSMALLINT  Precision
                                   ,SQLSMALLINT  Scale
                                   ,SQLPOINTER   Data
                                   ,SQLLEN*      StringLength
                                   ,SQLLEN*      Indicator) = 0;

  // Get statement for witch this descriptor was auto created
  // Return statement object for witch this object was implicitly allocated
  // return Statement object to witch connected, if descriptor was allocated by an application return NULL
  ODBCStatement* GetOwnerStmt();
  // Get DB connection for witch this descriptor was allocated
  ODBCConnection* GetOwnerDBConnection();
protected:
  ODBCStatement*  m_statement;                // if implicitly created for a statement
  ODBCConnection* m_connection;               // Connection owner of this object
  bool            m_desc_row_type;            // Row or parameter descriptor
  SQLSMALLINT     m_desc_alloc_type;          // Internal allocated or by application by AllocHandle? SQL_DESC_ALLOC_TYPE
  SQLSMALLINT     m_desc_count;               // Count of records in Descriptor (Params or Columns)
  int             m_desc_count_alloc;         // Size (count of elements) of records array, in Descriptor (Params or Columns)
  SQLUSMALLINT*   m_desc_array_status_ptr;    // status of a fetch (size = m_desc_array_size)
};

inline ODBCStatement* 
ODBCDescriptor::GetOwnerStmt()
{
  return m_statement;
}

inline ODBCConnection* 
ODBCDescriptor::GetOwnerDBConnection()
{
  return m_connection;
}

inline int 
ODBCDescriptor::GetRecCount()
{
  return m_desc_count;
}

inline bool 
ODBCDescriptor::IsRowDescriptor()
{
  return m_desc_row_type;
}

inline bool 
ODBCDescriptor::IsInternal()
{
  return (m_desc_alloc_type == SQL_DESC_ALLOC_AUTO);
}

inline SQLUSMALLINT*
ODBCDescriptor::GetArrayStatusPointer()
{
  return m_desc_array_status_ptr;
}