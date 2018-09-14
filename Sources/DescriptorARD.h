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
// Contents:  Interface for the Application Descriptors
//            Application Row Descriptor
//            Application Parameter Descriptor
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//

#pragma once
#include "Descriptor.h"

// Class for representing ODBC Value holders
class ODBCItems 
{
public:
  // Constructor. Fills all values with default value
  SQLRETURN Init(ODBCStatement* p_statement);
  // Destructor.  Free all resources & Fill all values with default value
  SQLRETURN Done(ODBCStatement* p_statement);	
  // Copy from another ODBCItemsType
  SQLRETURN CopyFrom(ODBCItems* p_source);

  SQLSMALLINT m_desc_type;            // SQL_DESC_TYPE            m_valueType         Type of Value	(internal OraTypesEnumeration)
  SQLPOINTER  m_desc_data_ptr;        // SQL_DESC_DATA_PTR        m_value             ptr to ODBC Value                    
  SQLINTEGER* m_desc_indicator_ptr;   // SQL_DESC_INDICATOR_PTR   m_indicator         Null or Len indicator for this value 
  SQLINTEGER  m_desc_length;          // SQL_DESC_LENGTH          m_valueLen          Len of Value memory (BufferLength)   
  SQLSMALLINT m_desc_parameter_type;  // SQL_DESC_PARAMETER_TYPE  m_inputOutPutType   Input/Output indicator (For param)   
  SQLSMALLINT m_desc_param_type;      // SQL_DESC_TYPE of IPD!!   m_ODBCSQL_Type      ODBC SQL Type
  SQLUINTEGER m_desc_precision;       // SQL_DESC_PRECISION       m_columnSize        Column size
  SQLSMALLINT m_desc_scale;           // SQL_DESC_SCALE           m_decimalDigits     Decimal digits
  CString     m_desc_name;            // SQL_DESC_NAME  
};

//////////////////////////////////////////////////////////////////////////
//
// ODBC_ARD
//
//////////////////////////////////////////////////////////////////////////


// Class for SQL_HANDLE_DESC type (SQLHDESC) ARD APD
// Describe call for ODBC functions with SQLHDESC ARD APD input descriptor
class ODBC_ARD:public ODBCDescriptor
{
  friend class ODBCItems;
public:
  // Constructor		
  ODBC_ARD(ODBCConnection* p_connection
          ,ODBCStatement*  p_statement
          ,SQLSMALLINT     p_alloc_type = SQL_DESC_ALLOC_AUTO
          ,bool            p_row = true);	
  // Destructor
  virtual ~ODBC_ARD();
  SQLRETURN FreeObjectHandler();          // Free object handler
  void      DoneObject();                 // Pre destruction
  SQLRETURN CopyFrom(ODBC_ARD* p_source); // Copy from another ARD
  // Set the statement pointer after assigning a new appliction allocated (AllocHandel) ARD.
  void      SetStatement(ODBCStatement* p_statement);
  // Set count of items (PARAMS OR COLUMNS) in descriptor
  // 1) If Count>RecArraySize then VarArray will be reallocated
  // 2) if Count<RecCount VarArray will set the same size (for buffering)
  // 3) if Count = RESET_IRDARD_DESCRIPTOR it resets the array
  SQLRETURN SetItemsCount(int Count);
  // Array size of ROW-WISE / COLUMN-WISE binding
  SQLUINTEGER GetArraySize();
  //Get internal holder for column value
  ODBCItems*  GetItem(short Item);
  //Get internal holder for column value if item not present realloc array
  ODBCItems *AllocItem(short Item);
  //Convert ODBC SQL Type to OraTypesEnumeration
  OraTypesEnumeration ConvertODBCTypeToInternal(SQLSMALLINT Type);

  // ODBC FUNCTIONS
  SQLRETURN SQLGetDescField(SQLSMALLINT  RecNumber
                           ,SQLSMALLINT  FieldIdentifier
                           ,SQLPOINTER   Value
                           ,SQLINTEGER   BufferLength
                           ,SQLINTEGER*  StringLength);
  SQLRETURN SQLSetDescField(SQLSMALLINT  RecNumber
                           ,SQLSMALLINT  FieldIdentifier
                           ,SQLPOINTER   Value
                           ,SQLINTEGER   BufferLength);
  SQLRETURN SQLGetDescRec  (SQLSMALLINT  RecNumber
                           ,SQLCHAR*     Name
                           ,SQLSMALLINT  BufferLength
                           ,SQLSMALLINT* StringLength
                           ,SQLSMALLINT* Type
                           ,SQLSMALLINT* SubType
                           ,SQLLEN*      Length
                           ,SQLSMALLINT* Precision
                           ,SQLSMALLINT* Scale
                           ,SQLSMALLINT* Nullable);
  SQLRETURN SQLSetDescRec  (SQLSMALLINT  RecNumber
                           ,SQLSMALLINT  Type
                           ,SQLSMALLINT  SubType
                           ,SQLINTEGER   Length
                           ,SQLSMALLINT  Precision
                           ,SQLSMALLINT  Scale
                           ,SQLPOINTER   Data
                           ,SQLINTEGER*  StringLength
                           ,SQLINTEGER*  Indicator);

protected:
  ODBCItems* VarArray;    // All data
  SQLUINTEGER m_desc_array_size;      // Number of rows in the ARD
  SQLINTEGER* m_desc_bind_offset_ptr; // Ptr to the bind offset
  SQLUINTEGER m_desc_bind_type;       // Way of binding in ExtendedFetch
};


inline ODBCItems* 
ODBC_ARD::GetItem(short Item)
{ 
  if(Item<0 || Item>=m_desc_count)
  {
    return NULL;
  }
  return &VarArray[Item];
}

inline SQLUINTEGER
ODBC_ARD::GetArraySize()
{
  return m_desc_array_size;
}