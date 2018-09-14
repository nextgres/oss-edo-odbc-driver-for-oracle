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
// Contents:  Implementation of Application Descriptor
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "Descriptor.h"
#include "DescriptorARD.h"
#include "DescriptorIRD.h"
#include "ODBCTypes.h"
#include "Utility.h"

SQLRETURN 
ODBCItems::Init(ODBCStatement* /*p_statement*/)
{
  m_desc_type           = 0;	
  m_desc_data_ptr       = NULL;
  m_desc_indicator_ptr  = NULL;
  m_desc_length         = 0;
  m_desc_parameter_type = 0;
  m_desc_param_type     = 0;
  m_desc_precision      = 0;
  m_desc_scale          = 0;
  return SQL_SUCCESS;
}

SQLRETURN 
ODBCItems::Done(ODBCStatement* /*p_statement*/)
{
  m_desc_type           = 0;	
  m_desc_data_ptr       = NULL;
  m_desc_indicator_ptr  = NULL;
  m_desc_length         = 0;
  m_desc_parameter_type = 0;
  m_desc_param_type     = 0;
  m_desc_precision      = 0;
  m_desc_scale          = 0;
  return SQL_SUCCESS;
}

SQLRETURN
ODBCItems::CopyFrom(ODBCItems* p_source)
{
  m_desc_type           = p_source->m_desc_type;
  m_desc_data_ptr       = p_source->m_desc_data_ptr;
  m_desc_indicator_ptr  = p_source->m_desc_indicator_ptr;
  m_desc_length         = p_source->m_desc_length;
  m_desc_parameter_type = p_source->m_desc_parameter_type;
  m_desc_param_type     = p_source->m_desc_param_type;
  m_desc_precision      = p_source->m_desc_precision;
  m_desc_scale          = p_source->m_desc_scale;
  m_desc_name           = p_source->m_desc_name;
  return SQL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
// 
// APPLICATION ROW DESCRIPTOR (ARD / APD)
//
//////////////////////////////////////////////////////////////////////////

#pragma warning (disable: 4311)

ODBC_ARD::ODBC_ARD(ODBCConnection* p_connection
                  ,ODBCStatement*  p_statement
                  ,SQLSMALLINT     p_alloc_type
                  ,bool            p_row)
         :ODBCDescriptor(p_connection,p_statement,p_alloc_type,p_row)
         ,m_desc_bind_type(SQL_BIND_BY_COLUMN)
         ,m_desc_bind_offset_ptr(NULL)
         ,m_desc_array_size(1)
{
  VarArray = (ODBCItems*)malloc(sizeof(ODBCItems) * m_desc_count_alloc);
  for(int i=0;i<m_desc_count_alloc;i++)
  {
    VarArray[i].Init(m_statement);
  }
}

#pragma warning (default: 4311)

ODBC_ARD::~ODBC_ARD()
{
  if(VarArray)
  {
    free(VarArray);
    VarArray = NULL;
  }
}

// Set the statement pointer after assigning a new application allocated (AllocHandel) ARD.
void 
ODBC_ARD::SetStatement(ODBCStatement* p_statement)
{
  m_statement = p_statement;
}

SQLRETURN 
ODBC_ARD::SetItemsCount(int Count)
{
  if(Count == 0)
  {
    return SQL_ERROR;
  }
  if(Count == m_desc_count)
  {
    return SQL_SUCCESS;
  }
  if(Count == RESET_IRDARD_DESCRIPTOR)
  {
    if(VarArray)
    {
      free(VarArray);
      VarArray = NULL;
    }
    m_desc_count   = 0;
    m_desc_count_alloc = 0;
    return SQL_SUCCESS;
  }
  int i;
  if(Count > m_desc_count_alloc)
  {
    int oldMax = m_desc_count_alloc;
    m_desc_count_alloc = (Count / DESCRIPTOR_BLOCK_SIZE + 1) * DESCRIPTOR_BLOCK_SIZE;
    VarArray = (ODBCItems*)realloc(VarArray,sizeof(ODBCItems) * m_desc_count_alloc);		
    if(VarArray == NULL)
    {
      return SQL_ERROR;
    }
    for(i = oldMax; i<m_desc_count_alloc; ++i)
    {
      VarArray[i].Init(m_statement);
    }
  }
  else
  {
    if(Count<m_desc_count)
    {
      for(i=Count;i<m_desc_count;i++)
      {
        VarArray[i].Done(m_statement);
      }
    }
  }
  m_desc_count = (SQLSMALLINT)Count;
  return SQL_SUCCESS;
}

void 
ODBC_ARD::DoneObject()
{
  for(int i=0;i<m_desc_count_alloc;i++)
  {
    VarArray[i].Done(m_statement);	
  }
  m_desc_count           = 0;
  m_desc_count_alloc     = 0;
  m_desc_array_size      = 1;
  m_desc_bind_offset_ptr = NULL;
  m_desc_bind_type       = SQL_BIND_BY_COLUMN;
}

SQLRETURN 
ODBC_ARD::FreeObjectHandler()
{
  delete this;
  return SQL_SUCCESS;
}

// Used to determine the SQL type for BindParameter (m_desc_param_type -> OraItems.m_valueType)
// For incoming parameters of the binds in the SQL Statements ('?')
tagODBCtoOracle ODBC_to_OraTypesEnumeration[]=
{
  0,                            UNKNOWN_TYPE,
  SQL_CHAR,                     STRING_TYPE,
  SQL_VARCHAR,                  STRING_TYPE,
  SQL_LONGVARCHAR,              CLOB_TYPE,
  SQL_WCHAR,                    STRING_TYPE, // NCHAR
  SQL_WVARCHAR,                 STRING_TYPE, // NVARCHAR
  SQL_WLONGVARCHAR,             STRING_TYPE, // NCLOB
  SQL_DECIMAL,                  NUMBER_TYPE,
  SQL_NUMERIC,                  NUMBER_TYPE,
  SQL_SMALLINT,                 INTEGER_TYPE,
  SQL_INTEGER,                  INTEGER_TYPE,
  SQL_REAL,                     DOUBLE_TYPE,
  SQL_FLOAT,                    DOUBLE_TYPE,
  SQL_DOUBLE,                   DOUBLE_TYPE,
  SQL_BIT,                      INTEGER_TYPE,
  SQL_TINYINT,                  INTEGER_TYPE,
  SQL_BIGINT,                   NUMBER_TYPE,
  SQL_BINARY,                   RAW_TYPE,
  SQL_VARBINARY,                LONGRAW_TYPE,
  SQL_LONGVARBINARY,            BLOB_TYPE,
  SQL_TYPE_DATE,                DATE_TYPE,
  SQL_TYPE_TIME,                DATE_TYPE,
  SQL_TYPE_TIMESTAMP,           DATETIME_TYPE,
  SQL_INTERVAL_MONTH,           INTERVALYM_TYPE,
  SQL_INTERVAL_YEAR,            INTERVALYM_TYPE,
  SQL_INTERVAL_YEAR_TO_MONTH,   INTERVALYM_TYPE,
  SQL_INTERVAL_DAY,             INTERVALDS_TYPE,
  SQL_INTERVAL_HOUR,            INTERVALDS_TYPE,
  SQL_INTERVAL_MINUTE,          INTERVALDS_TYPE,
  SQL_INTERVAL_SECOND,          INTERVALDS_TYPE,
  SQL_INTERVAL_DAY_TO_HOUR,     INTERVALDS_TYPE,
  SQL_INTERVAL_DAY_TO_MINUTE,   INTERVALDS_TYPE,
  SQL_INTERVAL_DAY_TO_SECOND,   INTERVALDS_TYPE,
  SQL_INTERVAL_HOUR_TO_MINUTE,  INTERVALDS_TYPE,
  SQL_INTERVAL_HOUR_TO_SECOND,  INTERVALDS_TYPE,
  SQL_INTERVAL_MINUTE_TO_SECOND,INTERVALDS_TYPE,
  SQL_GUID,                     STRING_TYPE       // No counterpart in Oracle!
};

OraTypesEnumeration 
ODBC_ARD::ConvertODBCTypeToInternal(SQLSMALLINT Type)
{
  for(int i=0,len=sizeof(ODBC_to_OraTypesEnumeration)/sizeof(ODBC_to_OraTypesEnumeration[0]);i<len;i++)
  {
    if(ODBC_to_OraTypesEnumeration[i].ODBC_Type==Type)
    {
      return ODBC_to_OraTypesEnumeration[i].TypeValue;
    }
  }
  return UNKNOWN_TYPE;
}

ODBCItems*
ODBC_ARD::AllocItem(short Item)
{
  if(Item<0)
  {
    return NULL;
  }
  if(Item>=m_desc_count)
  {
    if(Item>=m_desc_count_alloc)
    {
      m_desc_count_alloc = ((Item + 1) / DESCRIPTOR_BLOCK_SIZE + 1) * DESCRIPTOR_BLOCK_SIZE;
      VarArray = (ODBCItems*)realloc(VarArray,sizeof(ODBCItems)*m_desc_count_alloc);		
      for(int i=m_desc_count;i<m_desc_count_alloc;i++)
      {
        VarArray[i].Init(m_statement);
      }
    }
    m_desc_count=Item+1;
  }
  return &VarArray[Item];
}

SQLRETURN
ODBC_ARD::SQLGetDescField(SQLSMALLINT RecNumber
                         ,SQLSMALLINT FieldIdentifier
                         ,SQLPOINTER  Value
                         ,SQLINTEGER  /*BufferLength*/
                         ,SQLINTEGER* StringLength)
{
  SQLSMALLINT tiny   = 0xFF;
  SQLUINTEGER number = 0xFFFF;
  // Header fields
  switch(FieldIdentifier)
  {
    case SQL_DESC_ALLOC_TYPE:         tiny = m_desc_alloc_type ? SQL_DESC_ALLOC_AUTO : SQL_DESC_ALLOC_USER;
                                      break;
    case SQL_DESC_ARRAY_SIZE:         number = m_desc_array_size;
                                      break;
    case SQL_DESC_ARRAY_STATUS_PTR:   number = (SQLUINTEGER)m_desc_array_status_ptr;
                                      break;
    case SQL_DESC_BIND_OFFSET_PTR:    number = (SQLUINTEGER)m_desc_bind_offset_ptr;
                                      break;
    case SQL_DESC_BIND_TYPE:          number = m_desc_bind_type;
                                      break;
    case SQL_DESC_COUNT:              tiny = m_desc_count;
                                      break;
    case SQL_DESC_ROWS_PROCESSED_PTR: // Does not exist in the ARD
                                      m_ociError.AddError("HY091");
                                      return SQL_ERROR;
  }
  if(RecNumber < 0 || RecNumber > m_desc_count)
  {
    // Invalid descriptor index
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  if(RecNumber > 0)
  {
    // RECORD FIELDS
    switch(FieldIdentifier)
    {
      case SQL_DESC_AUTO_UNIQUE_VALUE:  // Not in the ARD
      default:                          m_ociError.AddError("HY091");
                                        return SQL_ERROR;
    }
  }
  if(tiny != 0xFF)
  {
    *((SQLSMALLINT*)Value) = tiny;
    if(StringLength)
    {
      *StringLength = SQL_IS_SMALLINT;
    }
    return SQL_SUCCESS;
  }
  if(number != 0xFF)
  {
    *((SQLUINTEGER*)Value) = number;
    if(StringLength)
    {
      *StringLength = SQL_IS_UINTEGER;
    }
    return SQL_SUCCESS;
  }
  return SQL_ERROR;
}

SQLRETURN 
ODBC_ARD::SQLSetDescField(SQLSMALLINT RecNumber
                         ,SQLSMALLINT FieldIdentifier
                         ,SQLPOINTER  Value
                         ,SQLINTEGER  /*BufferLength*/)
{
  // HEADER FIELDS
  switch(FieldIdentifier)
  {
    case SQL_DESC_ARRAY_STATUS_PTR:   m_desc_array_status_ptr = (SQLUSMALLINT*)Value;
                                      return SQL_SUCCESS;
    case SQL_DESC_COUNT:              m_desc_count = (SQLSMALLINT)Value;
                                      return SetItemsCount(m_desc_count);
    case SQL_DESC_ARRAY_SIZE:         m_desc_array_size = (SQLUINTEGER)Value;
                                      return SQL_SUCCESS;
    case SQL_DESC_BIND_OFFSET_PTR:    m_desc_bind_offset_ptr = (SQLINTEGER*)Value;
                                      return SQL_SUCCESS;
    case SQL_DESC_BIND_TYPE:          if(m_desc_count)
                                      {
                                        // Function sequence error. (already bound)
                                        m_ociError.AddError("HY010");
                                        return SQL_ERROR;
                                      }
                                      m_desc_bind_type = (SQLUINTEGER)Value;
                                      return SQL_SUCCESS;
    case SQL_DESC_ALLOC_TYPE:         // Read only. Cannot be set     
                                      m_ociError.AddError("HY016");
                                      return SQL_ERROR;
    case SQL_DESC_ROWS_PROCESSED_PTR: // Does not exist in ARD
                                      m_ociError.AddError("HY091");
                                      return SQL_ERROR;
  }
  if(RecNumber < 0 || RecNumber > m_desc_count)
  {
    // Field record number out of range
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  // RECORD FIELDS
  ODBCItems* record = GetItem(RecNumber-1);
  switch(FieldIdentifier)
  {
    case SQL_DESC_CONCISE_TYPE:               record->m_desc_type = (SQLSMALLINT)Value;
                                              break;
    case SQL_DESC_DATA_PTR:                   record->m_desc_data_ptr = Value;
                                              break;
    case SQL_DESC_DATETIME_INTERVAL_CODE:     switch((SQLINTEGER)Value)
                                              {
                                                  case SQL_INTERVAL_DAY_TO_SECOND:
                                                  case SQL_INTERVAL_YEAR_TO_MONTH: record->m_desc_type = (SQLSMALLINT)Value;
                                                                                   break;
                                              }
                                              break;
    case SQL_DESC_DATETIME_INTERVAL_PRECISION:break;
    case SQL_DESC_INDICATOR_PTR:              record->m_desc_indicator_ptr = (SQLINTEGER*)Value;
                                              break;
    case SQL_DESC_LENGTH:                     record->m_desc_length = (SQLINTEGER)Value;
                                              break;
    case SQL_DESC_NUM_PREC_RADIX:             break;
    case SQL_DESC_OCTET_LENGTH:               // ? IRD?
                                              break;
    case SQL_DESC_OCTET_LENGTH_PTR:           break;
    case SQL_DESC_PRECISION:                  record->m_desc_precision = (SQLUINTEGER)Value;
                                              break;
    case SQL_DESC_SCALE:                      record->m_desc_scale = (SQLSMALLINT)Value;
                                              break;
    case SQL_DESC_TYPE:                       record->m_desc_type = (SQLSMALLINT)Value;
                                              break;

    case SQL_DESC_AUTO_UNIQUE_VALUE:          // Does not exist in ARD
    case SQL_DESC_BASE_COLUMN_NAME:           // Does not exist in ARD
    case SQL_DESC_BASE_TABLE_NAME:            // Does not exist in ARD
    case SQL_DESC_CASE_SENSITIVE:             // Does not exist in ARD
    case SQL_DESC_CATALOG_NAME:               // Does not exist in ARD
    case SQL_DESC_DISPLAY_SIZE:               // Does not exist in ARD
    case SQL_DESC_FIXED_PREC_SCALE:           // Does not exist in ARD
    case SQL_DESC_LABEL:                      // Does not exist in ARD
    case SQL_DESC_LITERAL_PREFIX:             // Does not exist in ARD
    case SQL_DESC_LITERAL_SUFFIX:             // Does not exist in ARD
    case SQL_DESC_LOCAL_TYPE_NAME:            // Does not exist in ARD
    case SQL_DESC_NAME:                       // Does not exist in ARD
    case SQL_DESC_PARAMETER_TYPE:             // Does not exist in ARD
    case SQL_DESC_ROWVER:                     // Does not exist in ARD
    case SQL_DESC_SCHEMA_NAME:                // Does not exist in ARD
    case SQL_DESC_SEARCHABLE:                 // Does not exist in ARD
    case SQL_DESC_TABLE_NAME:                 // Does not exist in ARD
    case SQL_DESC_TYPE_NAME:                  // Does not exist in ARD
    case SQL_DESC_UNNAMED:                    // Does not exist in ARD
    case SQL_DESC_UNSIGNED:                   // Does not exist in ARD
    case SQL_DESC_UPDATABLE:                  // Does not exist in ARD
    default:                                  m_ociError.AddError("HY091");
                                              return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

// Set descriptor multiple internal fields by one call
SQLRETURN 
ODBC_ARD::SQLSetDescRec(SQLSMALLINT /*RecNumber    */
                       ,SQLSMALLINT /*Type         */
                       ,SQLSMALLINT /*SubType      */
                       ,SQLINTEGER  /*Length       */
                       ,SQLSMALLINT /*Precision    */
                       ,SQLSMALLINT /*Scale        */
                       ,SQLPOINTER  /*Data         */
                       ,SQLINTEGER* /*StringLength */
                       ,SQLINTEGER* /*Indicator    */)
{
  return SQL_ERROR;
}

SQLRETURN 
ODBC_ARD::SQLGetDescRec(SQLSMALLINT  RecNumber
                       ,SQLCHAR*     Name
                       ,SQLSMALLINT  BufferLength
                       ,SQLSMALLINT* StringLength
                       ,SQLSMALLINT* Type
                       ,SQLSMALLINT* SubType
                       ,SQLLEN*      Length
                       ,SQLSMALLINT* Precision
                       ,SQLSMALLINT* Scale
                       ,SQLSMALLINT* Nullable)
{
  if(RecNumber < 0 || RecNumber > m_desc_count)
  {
    // Field record number out of range
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  ODBCItems* record = GetItem(RecNumber-1);
  if(Type)
  {
    *Type = record->m_desc_type;
  }
  if(SubType)
  {
    //
  }
  if(Length)
  {
    *Length = record->m_desc_length;
  }
  if(Precision)
  {
    *Precision = (SQLSMALLINT)record->m_desc_precision;
  }
  if(Scale)
  {
    *Scale = record->m_desc_scale;
  }
  if(Nullable)
  {
    // *Nullable = record->m_desc_nullable;
  }
  // Name
  int len = 0;
  SQLRETURN res = CopyODBCStringToApp(Name,(char*)record->m_desc_name.GetString(),BufferLength,&len,&m_ociError);
  if(StringLength)
  {
    *StringLength = (SQLSMALLINT)len;
  }
  return res;
}

SQLRETURN
ODBC_ARD::CopyFrom(ODBC_ARD* p_source)
{
  // Descriptor
  m_desc_array_status_ptr = p_source->m_desc_array_status_ptr;
  // ARD
  m_desc_array_size       = p_source->m_desc_array_size;
  m_desc_bind_type        = p_source->m_desc_bind_type;
  m_desc_bind_offset_ptr  = p_source->m_desc_bind_offset_ptr;
  // SpecialODBCItems array
  int total = p_source->m_desc_count;
  if(total == 0)
  {
    // Already ready
    return SQL_SUCCESS;
  }
  SetItemsCount(RESET_IRDARD_DESCRIPTOR);
  SQLRETURN res = SetItemsCount(total);
  if(res != SQL_SUCCESS)
  {
    // Possibly memory error
    return res;
  }
  for(short i=0; i<total; ++i)
  {
    ODBCItems* target = GetItem(i);
    ODBCItems* source = p_source->GetItem(i);
    target->CopyFrom(source);
  }
  return SQL_SUCCESS;
}
