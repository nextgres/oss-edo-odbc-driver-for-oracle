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
// Contents:  Interface for the Internal Descriptor
//            Internal Row  Descriptor and
//            Internal Parameter Descriptor
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//

#pragma once
#include "Descriptor.h"
#include "ODBCTypes.h"

// relocation table between Oracle Type Code from Param Descriptor to OraTypesEnumeration
typedef struct tagOraTypesDescriptor
{
  sb2 OraType;
  sb2 BindType;
  sb2 PiecewiseType;
  int MaxLength;
  int BindLength;
}
OraTypesDescriptor;

extern OraTypesDescriptor OraTypesEnumeration_to_OraTypes[];

// struct for relocation table between Oracle Type Code from Param Descriptor to OraTypesEnumeration
typedef struct tagRelocTableOfOraTypeCodeToOraTypes
{
  sb2 OraType;
  OraTypesEnumeration TypeValue;
}
RelocTableOfOraTypeCodeToOraTypes;

// struct for relocation table between OraTypesEnumeration to ODBC type code
typedef struct tagODBCtoOracle
{
  SQLSMALLINT         ODBC_Type;
  OraTypesEnumeration TypeValue;
}
RelocTableOfOraTypesToODBC;

extern RelocTableOfOraTypeCodeToOraTypes OraTypes_to_OraTypesEnumeration[];
extern RelocTableOfOraTypesToODBC        OraTypesEnumeration_to_ODBC[];
extern RelocTableOfOraTypesToODBC        ODBC_to_OraTypesEnumeration[];
extern char*                             ODBC_Prefixes[];
extern char*                             ODBC_Suffixes[];

class ODBC_IRD;
class ODBC_ARD;

// Class for representing ORACLE OCI Value holders
class OracleItems 
{
public:
  SQLRETURN Init();
  SQLRETURN Done(ODBCStatement* p_statement);
  SQLRETURN CopyFrom(OracleItems* p_source);

  // Unpack Column information from OCI library
  SQLRETURN ReadColumnInfo(ODBC_IRD* Owner,OCIParam* hColDesc,ODBCErrorHolder& ociError,int p_maxLength);

  //  Change type of holder
  //  Free all resources (if was created) & and create with type that == val
  SQLRETURN AllocType(OraTypesEnumeration val,int len,ODBCStatement *p_statement);
  // Open LOB locator
  SQLRETURN LobOpen (OCISvcCtx* serviceContext,ODBCErrorHolder& ociError,ub1 p_mode);
  // Close LOB locator
  SQLRETURN LobClose(OCISvcCtx* serviceContext,ODBCErrorHolder& ociError);
  // Return type value for bind or define		
  sb2   GetBindType();
  // Return type value for piecewise bind or define
  sb2   GetPiecewiseType();
  // Return the maximum length for the datatype
  int   GetMaxTypeLength();
  // Return ptr to value for bind or define		
  dvoid *MapToPtrValue();	
  // Set new descriptor name
  void  SetDescName(const char* p_name,int p_size);
  void  SetDescTypeName(const char* p_name,int p_size);
  // For GetData
  void  ResetGetData();     // Call after each fetch
  bool  AllDataGotten();    // This column is ready with GetData (SQL_NO_DATA)
  int   GetPendingLength(); // Get the pending data length  for the next GetData
  bool  IsLOBvalue();       // Is it a CLOB/BLOB?
  bool  IsBinaryValue();    // Is it a binary?

  OraTypesEnumeration m_valueType;  // Type of Value union. Internal Oracle type
  // All possible value type
  union
  {
    void*           UnknownPtr;   // Unknown type ptr
    OCIDate         date_val;     // ORACLE DATE type
    OCINumber       number_val;   // ORACLE NUMBER type
    OCIString*      string_val;   // ORACLE STRING type
    OCIDateTime*    datetime_val; // ORACLE DATETIME type
    OCIInterval*    interval_val; // ORACLE INTERVAL type
    OCIRowid*       rowid_val;    // ORACLE ROWID type
    double          double_val;   // ORACLE FLOAT type
    int             int_val;      // ORACLE INTEGER type
    OCIClobLocator* lob_val;      // ORACLE B/C LOB type
    OCIRaw*         raw_val;      // ORACLE RAW/LONGRAW type
    unsigned char*  long_val;     // ORACLE LONG type
  } 
  Value;  
  sb2 m_indicator;      // Length / Null indicator / AtExec for this value
  sb1 m_lobOpen;        // LOBLocator open for streaming mode
  ub2 m_returnedSize;   // Returned size from fetch
  union
  {
    OCIDefine* lpDefine;  // For select return column info
    OCIBind*   lpBind;    // For binding operation info
  } 
  OCIObject; // OCI Bind Define object
  int   m_TypeLen;                // Len value for LEN ORACLE Types (Detect changes in length)
  sb2   m_desc_precision;         // SQL_DESC_PRECISION         Precision of this value
  sb1   m_desc_scale;             // SQL_DESC_SACLE             Scale of value
  int   m_desc_length;            // SQL_DESC_LENGTH            (BindSize) size what will be used for define or bind
  bool  m_desc_fixed_prec_scale;  // SQL_DESC_FIXED_PREC_SCALE  Explicitly defined precision and scale
  bool  m_desc_nullable;          // SQL_DESC_NULLABLE          Whether the column is nullable
  char* m_desc_name;              // SQL_DESC_NAME              Displayname of the column
  char* m_desc_type_name;         // SQL_DESC_TYPE_NAME         Type of this column by Oracle
  int   m_desc_octet_length;      // SQL_DESC_OCTET_LENGTH      Length of unicode string

  // For SQLGetData with SQL_ATTR_MAX_LENGTH
  //ub1*  m_pendingGetData;
  int       m_pendingLength;
  DWORD_PTR m_pendingOffset;
  // Speed optimalisation
  int   m_toODBCType;
  ConversionToFunction* m_toFunction;
};

inline sb2 
OracleItems::GetBindType()
{
  return OraTypesEnumeration_to_OraTypes[m_valueType].BindType;
}


// Get the pending data length  for the next GetData
inline int   
OracleItems::GetPendingLength() 
{
  return m_pendingLength;
}

inline sb2
OracleItems::GetPiecewiseType()
{
  return OraTypesEnumeration_to_OraTypes[m_valueType].PiecewiseType;
}

inline int
OracleItems::GetMaxTypeLength()
{
  return OraTypesEnumeration_to_OraTypes[m_valueType].MaxLength;
}

//////////////////////////////////////////////////////////////////////////
//
// ODBC_IRD
//
// Class for SQL_HANDLE_DESC type (SQLHDESC) IRD IPD
// Describe call for ODBC functions with SQLHDESC IRD IPD input descriptor
//
//////////////////////////////////////////////////////////////////////////

class ODBC_IRD:public ODBCDescriptor
{
  friend class OracleItems;
public:
  // Constructor
  ODBC_IRD(ODBCConnection* p_connection,ODBCStatement* p_statement,bool p_row = true);	
  virtual ~ODBC_IRD();                      // DTOR
  SQLRETURN FreeObjectHandler();            // Free object handler
  void DoneObject();                        // Pre destruction
  SQLRETURN CopyFrom(ODBC_IRD* p_source);   // Copy from another IRD
  //Set count of items (PARAMS OR COLUMNS) in descriptor
  // 1) If Count>RecArraySize then VarArray will be reallocated
  // 2) if Count<RecCount VarArray will stell the same size (for buffering)
  // 3) If RESET_IRDARD_DESCRIPTOR it will reset the array
  // param Count new count of items 
  SQLRETURN SetItemsCount(int Count);
  //Convert oracle type to OraTypesEnumeration	
  OraTypesEnumeration ConvertOracleTypeToInternal(int Type);
  //Convert OraTypesEnumeration to ODBC SQL Type
  SQLSMALLINT ConvertInternalToODBCType(OraTypesEnumeration Type);
  //Define all columns in OCI library
  SQLRETURN DefineAllColumns();
  //Bind all Params in OCI library
  SQLRETURN BindAllParams(ODBC_ARD* p_APD);
  // We have processed one row
  void ProcessRow();
  // Get the number of rows processed
  int  GetRowNumber();
  // Find bind handle for piece operation
  short FindBindHandle(OCIBind* p_bind);  
  //Get internal holder for column value
  OracleItems *GetItem(int Item)
  {
    if(Item < 0 || Item >= m_desc_count)
    {
      return NULL;
    }
    return &VarArray[Item];
  }
  //Get internal holder for column value if item not present realloc array
  OracleItems *AllocItem(short Item);
  //ODBC FUNCTIONS
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
                           ,SQLLEN*      StringLength
                           ,SQLLEN*      Indicator);
private:
  // Reset the VarArray
  void DoneVarArray();

  OracleItems* VarArray;                  // Array of ORACLE bind or define variables
  SQLUINTEGER  m_rows_processed;          // Internal counter of rows processed
  SQLUINTEGER* m_desc_rows_processed_ptr; // Were we put the rows counter
};

inline int
ODBC_IRD::GetRowNumber()
{
  return m_rows_processed;
}

