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
// Contents:  Implementation of the Internal Descriptor
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "Descriptor.h"
#include "DescriptorIRD.h"
#include "DescriptorARD.h"
#include "Statement.h"
#include "Environment.h"
#include "ODBCTypes.h"
#include "ErrorInfo.h"
#include "MainObject.h"
#include "Utility.h"

// relocation table between OraTypesEnumeration Oracle Type Code
OraTypesDescriptor OraTypesEnumeration_to_OraTypes[]=
{
//ORATYPE             BINDTYPE            PIECEWISE  MAXLENGTH   Bind Length
  0,                  0,                  0,         0,          0,                      // UNKNOWN_TYPE
  SQLT_ODT,           SQLT_ODT,           0,         0,          sizeof(OCIDate),        // DATE_TYPE          -> map to OCIDate     (date_val)
  SQLT_VNU,           SQLT_VNU,           0,         0,          sizeof(OCINumber),      // NUMBER_TYPE        -> map to OCINumber   (number_val)
  SQLT_VST,           SQLT_VST,           SQLT_CHR,  4000,       -1,                     // STRING_TYPE        -> map to OCIString   (string_val)
  SQLT_TIMESTAMP,     SQLT_TIMESTAMP,     0,         0,          0,                      // DATETIME_TYPE      -> map to OCIDateTime (datetime_val)
  SQLT_INTERVAL_YM,   SQLT_INTERVAL_YM,   0,         0,          0,                      // INTERVALYM_TYPE    -> map to OCIInterval (interval_val)
  SQLT_RDD,           SQLT_RDD,           0,         0,          0,                      // ROWID_TYPE         -> map to OCIRowid    (rowid_val)
  SQLT_DATE,          SQLT_DATE,          0,         0,          0,                      // ANSI_DATE_TYPE     -> map to OCIDateTime (datetime_val)
  SQLT_TIMESTAMP_TZ,  SQLT_TIMESTAMP_TZ,  0,         0,          0,                      // DATETIMEWTZ_TYPE   -> map to OCIDateTime (datetime_val)
  SQLT_TIMESTAMP_LTZ, SQLT_TIMESTAMP_LTZ, 0,         0,          0,                      // DATETIMEWLTZ_TYPE  -> map to OCIDateTime (datetime_val)
  SQLT_INTERVAL_DS,   SQLT_INTERVAL_DS,   0,         0,          0,                      // INTERVALDS_TYPE    -> map to OCIInterval (interval_val)
  SQLT_INT,           SQLT_INT,           0,         0,          sizeof(int),            // INTEGER_TYPE       -> map to int         (int_val)
  SQLT_FLT,           SQLT_FLT,           0,         0,          sizeof(double),         // DOUBLE_TYPE        -> map to double      (double_val)
  SQLT_CLOB,          SQLT_CLOB,          SQLT_CHR,  0x7FFFFFFF, sizeof(OCILobLocator*), // CLOB_TYPE          -> map to OCILobLocator
  SQLT_BLOB,          SQLT_BLOB,          SQLT_LBI,  0x7FFFFFFF, sizeof(OCILobLocator*), // BLOB_TYPE          -> map to OCILobLocator
  SQLT_LNG,           SQLT_LVC,           SQLT_CHR,  0x7FFFFFFF, NULL,                   // LONG_TYPE          -> map to char* buffer
  SQLT_BIN,           SQLT_LVB,           SQLT_BIN,  2000,       sizeof(OCIRaw*),        // RAW_TYPE           -> map to OCIRaw
  SQLT_LBI,           SQLT_LVB,           SQLT_LBI,  0x7FFFFFFF, sizeof(OCIRaw*),        // LONGRAW_TYPE       -> map to OCIRaw
};

// definition relocation table between Oracle Type Code from Param Descriptor to OraTypesEnumeration
tagRelocTableOfOraTypeCodeToOraTypes OraTypes_to_OraTypesEnumeration[]=
{
  0,      UNKNOWN_TYPE,
  1,      STRING_TYPE,      // VARCHAR2
  2,      NUMBER_TYPE,      // NUMBER
  3,      INTEGER_TYPE,     // SIGNED INTEGER
  4,      DOUBLE_TYPE,      // FLOAT
  5,      STRING_TYPE,      // SZ STRING (VARCHAR2+1)
  6,      NUMBER_TYPE,      // VARNUM
  8,      LONG_TYPE,        // LONG
  9,      STRING_TYPE,      // VARCHAR 
  12,     DATE_TYPE,        // DATE
  68,     NUMBER_TYPE,      // UNSIGNED INTEGER
  94,     STRING_TYPE,      // LONG
  96,     STRING_TYPE,      // CHAR
  97,     STRING_TYPE,      // CHARZ (n+1)
  104,    ROWID_TYPE,	      // ROWID DESCRIPTOR
  112,    CLOB_TYPE,        // CHAR LOB DESCRIPTOR
  113,    BLOB_TYPE,        // BINARY LOB
  155,    STRING_TYPE,      // OCI_STRING
  156,    DATE_TYPE,        // OCI_DATE
  184,    ANSI_DATE_TYPE,   // ANSI DATE
  187,    DATETIME_TYPE,    // TIMESTAMP
  188,    DATETIMEWTZ_TYPE, // TIMESTAMP WITH TIME ZONE
  189,    INTERVALYM_TYPE,  // INTERVAL YEAR TO MONTH
  190,    INTERVALDS_TYPE,  // INTERVAL DAY TO SECOND
  232,    DATETIMEWLTZ_TYPE,// TIMESTAMP WITH LOCAL TIME ZONE
  23,     RAW_TYPE,         // RAW
  24,     LONGRAW_TYPE,     // LONG RAW
  // Not mapped now types. Older Oracle versions.
  // Types only exists as internal types for connections to older databases
  15,     UNKNOWN_TYPE,     // VARRAW      No longer supported in Oracle 9 and onwards
  95,     UNKNOWN_TYPE,     // LONG VARRAW No longer supported in Oracle 9 and onwards
  // Types for which no ODBC counterparts exists.
  108,    UNKNOWN_TYPE,     // NAMED DATA TYPE
  110,    UNKNOWN_TYPE,     // REF to a COMPOUND DATA TYPE
  114,    UNKNOWN_TYPE,     // BINARY FILE DESCRIPTOR
  115,    UNKNOWN_TYPE      // CHARACTER FILE DESCRIPTOR
};

// Relocation table between OraTypesEnumeration and ODBC SQL types
tagODBCtoOracle OraTypesEnumeration_to_ODBC[]=
{
  0,                            UNKNOWN_TYPE,
  SQL_TYPE_TIMESTAMP,           DATE_TYPE,
  SQL_NUMERIC,                  NUMBER_TYPE,
  SQL_VARCHAR,                  STRING_TYPE,
  SQL_TYPE_TIMESTAMP,           DATETIME_TYPE,
  SQL_INTERVAL_YEAR_TO_MONTH,   INTERVALYM_TYPE,
  SQL_VARCHAR,                  ROWID_TYPE,
  SQL_TYPE_TIMESTAMP,           ANSI_DATE_TYPE,
  SQL_TYPE_TIMESTAMP,           DATETIMEWTZ_TYPE,
  SQL_TYPE_TIMESTAMP,           DATETIMEWLTZ_TYPE,
  SQL_INTERVAL_DAY_TO_SECOND,   INTERVALDS_TYPE,
  SQL_INTEGER,                  INTEGER_TYPE,
  SQL_DOUBLE,                   DOUBLE_TYPE,
  SQL_LONGVARCHAR,              CLOB_TYPE,
  SQL_LONGVARBINARY,            BLOB_TYPE,
  SQL_LONGVARCHAR,              LONG_TYPE,
  SQL_BINARY,                   RAW_TYPE,
  SQL_VARBINARY,                LONGRAW_TYPE,
};

char* ODBC_Prefixes[] =
{
   ""                       // Prefix of UNKNOWN_TYPE
  ,"'"                      // Prefix of DATE_TYPE
  ,""                       // Prefix of NUMBER_TYPE
  ,"'"                      // Prefix of STRING_TYPE
  ,"'"                      // Prefix of DATETIME_TYPE
  ,"'"                      // Prefix of INTERVALYM_TYPE
  ,"'"                      // Prefix of ROWID_TYPE
  ,"'"                      // Prefix of ANSI_DATE_TYPE
  ,"'"                      // Prefix of DATETIMEWTZ_TYPE
  ,"'"                      // Prefix of DATETIMEWLTZ_TYPE 
  ,","                      // Prefix of INTERVALDS_TYPE
  ,""                       // Prefix of INTEGER_TYPE
  ,""                       // Prefix of DOUBLE_TYPE
  ,"'"                      // Prefix of Clob_type
  ,"'"                      // Prefix of Blob_type
  ,"'"                      // Prefix of LONG_TYPE
  ,"'"                      // Prefix of RAW_TYPE
  ,"'"                      // Prefix of LONGRAW_TYPE
};

char* ODBC_Suffixes[] =
{
   ""                       // Suffix of UNKNOWN_TYPE
  ,"'"                      // Suffix of DATE_TYPE
  ,""                       // Suffix of NUMBER_TYPE
  ,"'"                      // Suffix of STRING_TYPE
  ,"'"                      // Suffix of DATETIME_TYPE
  ,"'"                      // Suffix of INTERVALYM_TYPE
  ,"'"                      // Suffix of ROWID_TYPE
  ,"'"                      // Suffix of ANSI_DATE_TYPE
  ,"'"                      // Suffix of DATETIMEWTZ_TYPE
  ,"'"                      // Suffix of DATETIMEWLTZ_TYPE 
  ,","                      // Suffix of INTERVALDS_TYPE
  ,""                       // Suffix of INTEGER_TYPE
  ,""                       // Suffix of DOUBLE_TYPE
  ,"'"                      // Suffix of Clob_type
  ,"'"                      // Suffix of Blob_type
  ,"'"                      // Suffix of LONG_TYPE
  ,"'"                      // Suffix of RAW_TYPE
  ,"'"                      // Suffix of LONGRAW_TYPE
};

SQLRETURN 
OracleItems::Init()
{
  Value.UnknownPtr        = NULL;
  m_valueType             = UNKNOWN_TYPE;	
  m_indicator             = SQL_NULL_DATA;
  m_lobOpen               = 0;
  OCIObject.lpDefine      = NULL;
  m_TypeLen               = 0;
  m_desc_length           = 0;
  m_desc_precision        = 0;
  m_desc_scale            = 0;
  m_desc_name             = NULL;
  m_desc_type_name        = NULL;
  m_desc_fixed_prec_scale = false;
  m_desc_nullable         = true;
  m_desc_octet_length     = 0;
  m_toFunction            = NULL;
  m_toODBCType            = 0;
  m_pendingLength         = SQL_NULL_DATA;
  m_pendingOffset         = 0;
  m_returnedSize          = 0;
  return SQL_SUCCESS;
}

SQLRETURN 
OracleItems::ReadColumnInfo(ODBC_IRD*        Owner
                           ,OCIParam*        hColDesc
                           ,ODBCErrorHolder& ociError
                           ,int              p_maxLength)
{
  ub2 ociDataType  = 0;
  ub2 ociDataLen   = 0;
  sb2 ociPrecision = 0;
  sb1 ociScale     = 0;
  ub1 ociNullable  = 0;
  ub2 ociOctetLen  = 0; // In UNICODE half the data_size!

  OraTypesEnumeration NeedType = UNKNOWN_TYPE;
  OCIAttrGet(hColDesc,OCI_DTYPE_PARAM,&ociDataType, NULL,OCI_ATTR_DATA_TYPE,ociError.GetOwnerErrorObject());
  OCIAttrGet(hColDesc,OCI_DTYPE_PARAM,&ociDataLen,  NULL,OCI_ATTR_DATA_SIZE,ociError.GetOwnerErrorObject());
  OCIAttrGet(hColDesc,OCI_DTYPE_PARAM,&ociPrecision,NULL,OCI_ATTR_PRECISION,ociError.GetOwnerErrorObject());
  OCIAttrGet(hColDesc,OCI_DTYPE_PARAM,&ociScale,    NULL,OCI_ATTR_SCALE,    ociError.GetOwnerErrorObject());
  OCIAttrGet(hColDesc,OCI_DTYPE_PARAM,&ociOctetLen, NULL,OCI_ATTR_CHAR_SIZE,ociError.GetOwnerErrorObject());
  OCIAttrGet(hColDesc,OCI_DTYPE_PARAM,&ociNullable, NULL,OCI_ATTR_IS_NULL,  ociError.GetOwnerErrorObject());

  // Get the correct data type
  NeedType = Owner->ConvertOracleTypeToInternal(ociDataType);
  if(NeedType == NUMBER_TYPE)
  {
    if(ociPrecision != 0 && ((ociPrecision  < 16  && ociScale == -127)||
                             (ociPrecision == 126 && ociScale == -127)))
    {
      // IEEE Type can have a precision of at most 16 digits
      NeedType = DOUBLE_TYPE;
    }
    if(ociPrecision > 0 && ociScale == 0 && ociPrecision < 11)
    {
      NeedType = INTEGER_TYPE;
    }
    if(ociPrecision == 0 && ociScale == 0 && Owner->GetOwnerStmt()->GetIntegerSpeedup())
    {
      NeedType = INTEGER_TYPE;
    }
  }
  if(NeedType == LONG_TYPE || NeedType == LONGRAW_TYPE)
  {
    ociDataLen = (ub2)((p_maxLength > 4000) ? p_maxLength : 4000);
  }
  // Allocate a new empty type entry
  SQLRETURN resCode = AllocType(NeedType,ociDataLen,Owner->m_statement);
  if(resCode != SQL_SUCCESS)
  {
    return resCode;
  }
  // Record everything
  m_desc_fixed_prec_scale = (ociPrecision == 0 && ociScale == 0) ? false : true;
  m_desc_nullable         = (ociNullable == 0) ? SQL_NO_NULLS : SQL_NULLABLE;
  m_desc_octet_length     = ociOctetLen;
  switch(m_valueType)
  {
    case INTEGER_TYPE:          m_desc_precision = ociPrecision;
                                m_desc_scale     = 0;
                                m_desc_length    = OraTypesEnumeration_to_OraTypes[m_valueType].BindLength;
                                break;
    case DOUBLE_TYPE:           m_desc_precision = 25;
                                m_desc_scale     = 0;
                                m_desc_length    = OraTypesEnumeration_to_OraTypes[m_valueType].BindLength;
                                break;
    case NUMBER_TYPE:           m_desc_precision = ociPrecision;
                                m_desc_scale     = ociScale;
                                m_desc_length    = OraTypesEnumeration_to_OraTypes[m_valueType].BindLength;
                                break;
    case DATE_TYPE:
    case DATETIME_TYPE:
    case DATETIMEWTZ_TYPE:
    case DATETIMEWLTZ_TYPE:
    case ANSI_DATE_TYPE:
    case INTERVALYM_TYPE:
    case INTERVALDS_TYPE:
    case ROWID_TYPE:            m_desc_precision = 0;
                                m_desc_scale     = 0;
                                m_desc_length    = OraTypesEnumeration_to_OraTypes[m_valueType].BindLength;
                                break;	
    case LONG_TYPE:             m_desc_length    = ociDataLen + sizeof(int);
                                break;
    case RAW_TYPE:              
    case LONGRAW_TYPE:          m_desc_length    = ociDataLen;
                                break;
    case CLOB_TYPE:             // Fall through
    case BLOB_TYPE:             m_TypeLen        = 
                                m_desc_length    = 0x7FFFFFFF;
                                break;
    case STRING_TYPE:           m_desc_precision = (sb2) ociDataLen;
                                m_desc_length    = (int) ociDataLen;
                                break;		
  }
  // Get the column name
  ub4   ociIdentSize = 0;
  text* ociAttributeName;
  OCIAttrGet(hColDesc,OCI_DTYPE_PARAM,&ociAttributeName,&ociIdentSize,OCI_ATTR_NAME,ociError.GetOwnerErrorObject());
  SetDescName((const char*)ociAttributeName,ociIdentSize);
  // Get the type name
  OCIAttrGet(hColDesc,OCI_DTYPE_PARAM,&ociAttributeName,&ociIdentSize,OCI_ATTR_TYPE_NAME,ociError.GetOwnerErrorObject());
  SetDescTypeName((const char*)ociAttributeName,ociIdentSize);
  if(ociIdentSize == 0)
  {
    switch(m_valueType)
    {
      case CLOB_TYPE:    SetDescTypeName("CLOB",4);     break;
      case LONG_TYPE:    SetDescTypeName("LONG",4);     break;
      case RAW_TYPE:     SetDescTypeName("RAW" ,3);     break;
      case LONGRAW_TYPE: SetDescTypeName("LONG RAW",8); break;
    }
  }
  return SQL_SUCCESS;
}

void
OracleItems::SetDescName(const char* p_name,int p_size)
{
  if(m_desc_name)
  {
    //WRITELOG("Free desc name: %s",m_desc_name);
    free(m_desc_name);
    m_desc_name = NULL;
  }
  if(p_size > 0)
  {
    //WRITELOG("Alloc desc name: %s",p_name);
    m_desc_name = (char*) malloc(p_size + 1);
    strncpy_s(m_desc_name,p_size + 1,p_name,p_size);
    m_desc_name[p_size] = 0;
  }
}

void
OracleItems::SetDescTypeName(const char* p_name,int p_size)
{
  if(m_desc_type_name)
  {
    free(m_desc_type_name);
    m_desc_type_name = NULL;
  }
  if(p_size > 0)
  {
    m_desc_type_name = (char*) malloc(p_size + 1);
    strncpy_s(m_desc_type_name,p_size + 1,p_name,p_size);
    m_desc_type_name[p_size] = 0;
  }
}

SQLRETURN 
OracleItems::Done(ODBCStatement* p_statement)
{
  if(m_valueType!=UNKNOWN_TYPE)
  {
    switch(m_valueType)
    {
      case DATE_TYPE:			    break;
      case NUMBER_TYPE:		    break;
      case INTEGER_TYPE:	    break;
      case DOUBLE_TYPE:		    break;
      case STRING_TYPE:       if(Value.string_val!=NULL)
                              {
                                sword ociRes;
                                ociRes = OCIStringResize(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,p_statement->GetOCIErrorObject()->GetOwnerErrorObject()
                                                        ,0
                                                        ,&Value.string_val);
                                if(ociRes!=OCI_SUCCESS)
                                {
                                  p_statement->GetOCIErrorObject()->GetErrorFromOCIHandle();
                                  return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
                                }
                              }
                              break;
      case RAW_TYPE:          // Fall through
      case LONGRAW_TYPE:      if(Value.raw_val != NULL)
                              {
                                sword ociRes;
                                ociRes = OCIRawResize(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                     ,p_statement->GetOCIErrorObject()->GetOwnerErrorObject()
                                                     ,0
                                                     ,&Value.raw_val);
    
                                if(ociRes!=OCI_SUCCESS)
                                {
                                  p_statement->GetOCIErrorObject()->GetErrorFromOCIHandle();
                                  return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
                                }
                              }
                              break;
      case LONG_TYPE:         if(Value.long_val != NULL)
                              {
                                free(Value.long_val);
                                Value.long_val = NULL;
                              }
                              break;
      case BLOB_TYPE:         // Fall through
      case CLOB_TYPE:         if(Value.lob_val)
                              {
                                ODBCErrorHolder* ociError = p_statement->GetOCIErrorObject();
                                OCIError* error = ociError->GetOwnerErrorObject();
                                OCISvcCtx* serviceContext = p_statement->GetDBConnection()->GetOCIConnection();
                                OCIEnv* environment = NULL;
                                ub4 size = sizeof(OCIEnv*);

                                if(m_lobOpen)
                                {
                                  LobClose(serviceContext,*ociError);
                                }
                                if(OCIAttrGet(serviceContext,OCI_HTYPE_SVCCTX,&environment,&size,OCI_ATTR_ENV,error) != OCI_SUCCESS)
                                {
                                  ociError->GetErrorFromOCIHandle();
                                  return SQL_ERROR;
                                }
                                // Free up a temporary write-to BLOB/CLOB
                                boolean is_temporary = false;
                                OCILobIsTemporary(environment,error,Value.lob_val,&is_temporary);
                                if(is_temporary)
                                {
                                  // Free up in the TEMP space
                                  if(OCILobFreeTemporary(serviceContext,error,Value.lob_val) != OCI_SUCCESS)
                                  {
                                    ociError->GetErrorFromOCIHandle();
                                    return SQL_ERROR;
                                  }
                                }
                                OCIDescriptorFree(Value.lob_val,OCI_DTYPE_LOCATOR);
                                Value.lob_val = NULL;
                              }
                              break;
      case DATETIME_TYPE:     if(Value.datetime_val)
                              {
                                OCIDescriptorFree(Value.datetime_val,OCI_DTYPE_TIMESTAMP);
                              }
                              break;
      case DATETIMEWTZ_TYPE:  if(Value.datetime_val)
                              {
                                OCIDescriptorFree(Value.datetime_val,OCI_DTYPE_TIMESTAMP_TZ);
                              }
                              break;			
      case DATETIMEWLTZ_TYPE: if(Value.datetime_val)
                              {
                                OCIDescriptorFree(Value.datetime_val,OCI_DTYPE_TIMESTAMP_LTZ);
                              }
                              break;
      case ANSI_DATE_TYPE:    if(Value.datetime_val)
                              {
                                OCIDescriptorFree(Value.datetime_val,OCI_DTYPE_DATE);
                              }
                              break;
      case INTERVALYM_TYPE:   if(Value.interval_val)
                              {
                                OCIDescriptorFree(Value.interval_val,OCI_DTYPE_INTERVAL_YM);
                              }
                              break;
      case INTERVALDS_TYPE:   if(Value.interval_val)
                              {
                                OCIDescriptorFree(Value.interval_val,OCI_DTYPE_INTERVAL_DS);
                              }
                              break;
      case ROWID_TYPE:        if(Value.rowid_val)
                              {
                                OCIDescriptorFree(Value.rowid_val,OCI_DTYPE_ROWID);
                              }
                              break;
    }
    Value.UnknownPtr        = NULL;
    m_valueType             = UNKNOWN_TYPE;
    m_indicator             = SQL_NULL_DATA; 
    m_lobOpen               = 0;
    OCIObject.lpDefine      = NULL;
    m_desc_fixed_prec_scale = false;
    m_desc_octet_length     = 0;
    m_desc_nullable         = true;
    m_toFunction            = NULL;
    m_toODBCType            = 0;
    m_pendingLength         = SQL_NULL_DATA;
    m_pendingOffset         = 0;
    m_returnedSize          = 0;
  }	
  return SQL_SUCCESS;
}

SQLRETURN 
OracleItems::AllocType(OraTypesEnumeration val,int len,ODBCStatement* p_statement)
{
  if(m_valueType != val || m_TypeLen != len)
  {
    sword ociRes = OCI_ERROR;

    Done(p_statement);
    m_valueType   = val;
    m_TypeLen     = len;
    m_desc_length = OraTypesEnumeration_to_OraTypes[m_valueType].BindLength;

    switch(m_valueType)
    {
      case DATE_TYPE: 			  // Fall through
      case NUMBER_TYPE: 			// Fall through
      case INTEGER_TYPE:			// Fall through
      case DOUBLE_TYPE:  			ociRes = OCI_SUCCESS;
                              break;
      case STRING_TYPE:       m_desc_length = len;
                              ociRes = OCIStringResize(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                      ,p_statement->GetOCIErrorObject()->GetOwnerErrorObject()
                                                      ,(ub4)len
                                                      ,&Value.string_val);
                              break;
      case RAW_TYPE:          // fall through
      case LONGRAW_TYPE:      m_desc_length = len;
                              ociRes = OCIRawResize(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                   ,p_statement->GetOCIErrorObject()->GetOwnerErrorObject()
                                                   ,(ub4)len
                                                   ,&Value.raw_val);
                              break;
      case LONG_TYPE:         Value.long_val = (unsigned char*) calloc(1,len + 1 + sizeof(int));
                              if(Value.long_val == NULL)
                              {
                                // Memory error;
                                p_statement->GetOCIErrorObject()->AddError("HY001");
                                return SQL_ERROR;
                              }
                              WRITELOG("LONG buffer [%p] size [%d + %d]",Value.long_val,len,(1 + sizeof(int)));
                              ociRes = SQL_SUCCESS;
                              break;
      case BLOB_TYPE:         // Fall through
      case CLOB_TYPE:         ociRes = OCIDescriptorAlloc(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                         ,(dvoid**)&Value.lob_val
                                                         ,OCI_DTYPE_LOB
                                                         ,0
                                                         ,NULL);
                              m_lobOpen = 0;
                              WRITELOG("IRD => Alloc LOB: %p",Value.lob_val);
                              break;
      case DATETIME_TYPE:			ociRes = OCIDescriptorAlloc(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                         ,(dvoid**)&Value.datetime_val
                                                         ,OCI_DTYPE_TIMESTAMP
                                                         ,0
                                                         ,NULL);
                              break;
      case DATETIMEWTZ_TYPE:  ociRes = OCIDescriptorAlloc(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                         ,(dvoid**)&Value.datetime_val
                                                         ,OCI_DTYPE_TIMESTAMP_TZ
                                                         ,0
                                                         ,NULL);
                              break;			
      case DATETIMEWLTZ_TYPE:	ociRes = OCIDescriptorAlloc(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                         ,(dvoid**)&Value.datetime_val
                                                         ,OCI_DTYPE_TIMESTAMP_LTZ
                                                         ,0
                                                         ,NULL);
                              break;
      case ANSI_DATE_TYPE:		ociRes = OCIDescriptorAlloc(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                         ,(dvoid**)&Value.datetime_val
                                                         ,OCI_DTYPE_DATE
                                                         ,0
                                                         ,NULL);
                              break;
      case INTERVALYM_TYPE:		ociRes = OCIDescriptorAlloc(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                         ,(dvoid**)&Value.interval_val
                                                         ,OCI_DTYPE_INTERVAL_YM
                                                         ,0
                                                         ,NULL);
                              break;
      case INTERVALDS_TYPE:  	ociRes = OCIDescriptorAlloc(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                         ,(dvoid**)&Value.interval_val
                                                         ,OCI_DTYPE_INTERVAL_DS
                                                         ,0
                                                         ,NULL);
                              break;
      case ROWID_TYPE: 			  ociRes = OCIDescriptorAlloc(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                         ,(dvoid**)&Value.rowid_val
                                                         ,OCI_DTYPE_ROWID
                                                         ,0
                                                         ,NULL);
                              break;
    }
    if(ociRes != OCI_SUCCESS)
    {
      p_statement->GetOCIErrorObject()->GetErrorFromOCIHandle();
      return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
    }
  }
  return SQL_SUCCESS;
}

dvoid*
OracleItems::MapToPtrValue()
{
  switch(m_valueType)
  {
    case INTEGER_TYPE:  			return &Value.int_val;
    case DOUBLE_TYPE:   			return &Value.double_val;
    case DATE_TYPE:     			return &Value.date_val;
    case NUMBER_TYPE:   			return &Value.number_val;
    case STRING_TYPE:   			return &Value.string_val;
    case DATETIME_TYPE:       // Fall through
    case DATETIMEWTZ_TYPE:    // Fall through
    case DATETIMEWLTZ_TYPE:   // Fall through
    case ANSI_DATE_TYPE:			return &Value.datetime_val;			
    case INTERVALYM_TYPE:     // Fall through
    case INTERVALDS_TYPE:			return &Value.interval_val;
    case ROWID_TYPE:      		return &Value.rowid_val;
    case RAW_TYPE:            // Fall through
    case LONGRAW_TYPE:        return  Value.raw_val;
    case LONG_TYPE:           return  Value.long_val;
    case BLOB_TYPE:           // Fall through
    case CLOB_TYPE:           return &(Value.lob_val);
  }
  return NULL;
}

SQLRETURN
OracleItems::CopyFrom(OracleItems* p_source)
{
  switch(p_source->m_valueType)
  {
    case UNKNOWN_TYPE:      Value.UnknownPtr   = p_source->Value.UnknownPtr;
                            break;
    case DATE_TYPE:         Value.date_val     = p_source->Value.date_val;
                            break;
    case STRING_TYPE:       Value.string_val   = p_source->Value.string_val;
                            break;
    case NUMBER_TYPE:       Value.number_val   = p_source->Value.number_val;
                            break;
    case DATETIME_TYPE:     // Fall through     
    case ANSI_DATE_TYPE:    // Fall through     
    case DATETIMEWTZ_TYPE:  // Fall through     
    case DATETIMEWLTZ_TYPE: Value.datetime_val = p_source->Value.datetime_val;
                            break;
    case INTERVALYM_TYPE: 
    case INTERVALDS_TYPE:   Value.interval_val = p_source->Value.interval_val;
                            break;
    case ROWID_TYPE:        Value.rowid_val    = p_source->Value.rowid_val;
                            break;
    case DOUBLE_TYPE:       Value.double_val   = p_source->Value.double_val;
                            break;
    case INTEGER_TYPE:      Value.int_val      = p_source->Value.int_val;
                            break;
    case RAW_TYPE:          // Fall through     
    case LONGRAW_TYPE:      Value.raw_val      = p_source->Value.raw_val;
                            break;
    case LONG_TYPE:         Value.long_val     = p_source->Value.long_val;
                            break;
    case BLOB_TYPE:         // Fall through     
    case CLOB_TYPE:         Value.lob_val      = p_source->Value.lob_val;
  }
  m_valueType             = p_source->m_valueType;
  m_indicator             = p_source->m_indicator;
  m_lobOpen               = p_source->m_lobOpen;
  OCIObject.lpBind        = p_source->OCIObject.lpBind;
  m_TypeLen               = p_source->m_TypeLen;
  m_desc_precision        = p_source->m_desc_precision;
  m_desc_scale            = p_source->m_desc_scale;
  m_desc_length           = p_source->m_desc_length;
  m_desc_fixed_prec_scale = p_source->m_desc_fixed_prec_scale;
  m_desc_nullable         = p_source->m_desc_nullable;
  m_desc_octet_length     = p_source->m_desc_octet_length;
  // Names
  SetDescName    (p_source->m_desc_name     ,strlen(p_source->m_desc_name));
  SetDescTypeName(p_source->m_desc_type_name,strlen(p_source->m_desc_type_name));
  return SQL_SUCCESS;
}

void 
OracleItems::ResetGetData()
{
  m_pendingOffset = 0;
  m_pendingLength = 0;
}

bool
OracleItems::AllDataGotten()
{
  return (m_pendingLength == SQL_NULL_DATA);
}

bool
OracleItems::IsLOBvalue()
{
  switch(m_valueType)
  {
    case CLOB_TYPE:
    case BLOB_TYPE: return true;
  }
  return false;
}

bool
OracleItems::IsBinaryValue()
{
  switch(m_valueType)
  {
    case RAW_TYPE:
    case BLOB_TYPE:
    case LONGRAW_TYPE: return true;
  }
  return false;
}

SQLRETURN
OracleItems::LobOpen(OCISvcCtx* serviceContext
                    ,ODBCErrorHolder& ociError
                    ,ub1 p_mode)
{
  if(m_lobOpen == false)
  {
    OCIError* error = ociError.GetOwnerErrorObject();
    if(p_mode == OCI_LOB_READWRITE)
    {
      ub1 lobType = OCI_TEMP_CLOB;
      switch(m_valueType)
      {
        case BLOB_TYPE: lobType = OCI_TEMP_BLOB;  break;
        case CLOB_TYPE: lobType = OCI_TEMP_CLOB;  break;
      }
      if(OCILobCreateTemporary(serviceContext
                              ,error
                              ,Value.lob_val
                              ,OCI_DEFAULT     // Character set ID
                              ,OCI_DEFAULT     // Character set Form
                              ,lobType
                              ,FALSE
                              ,OCI_DURATION_SESSION) != OCI_SUCCESS)
      {
        ociError.GetErrorFromOCIHandle();
        return SQL_ERROR;
      }
    }
    // Try to open LOB
    if(OCILobOpen(serviceContext,error,Value.lob_val,p_mode) != OCI_SUCCESS)
    {
      ociError.GetErrorFromOCIHandle();
      return SQL_ERROR;
    }
    m_lobOpen = true;
  }
  return SQL_SUCCESS;
}

SQLRETURN
OracleItems::LobClose(OCISvcCtx* serviceContext
                     ,ODBCErrorHolder& ociError)
{
  if(m_lobOpen)
  {
    OCIError* error = ociError.GetOwnerErrorObject();

    if(OCILobClose(serviceContext,error,Value.lob_val) != OCI_SUCCESS)
    {
      ociError.GetErrorFromOCIHandle();
      return SQL_ERROR;
    }
    m_lobOpen       = false;
    m_pendingLength = SQL_NULL_DATA;
    m_pendingOffset = 0;
  }
  return SQL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// ODBC_IRD
//
//////////////////////////////////////////////////////////////////////////

#pragma warning (disable: 4311)

ODBC_IRD::ODBC_IRD(ODBCConnection* p_connection,ODBCStatement* p_statement,bool p_row /*=true*/)
         :ODBCDescriptor(p_connection,p_statement,SQL_DESC_ALLOC_AUTO,p_row)
         ,m_desc_rows_processed_ptr(NULL)
         ,m_rows_processed(0)
{
  VarArray=(OracleItems*)malloc(sizeof(OracleItems) * m_desc_count_alloc);
  for(int i=0;i<m_desc_count_alloc;i++)
  {
    VarArray[i].Init();
  }
}

#pragma warning (default: 4311)

ODBC_IRD::~ODBC_IRD()
{
  DoneVarArray();
}

void
ODBC_IRD::DoneVarArray()
{
  if(VarArray)
  {
    for(int i=0;i<m_desc_count;++i)
    {
      if(VarArray[i].m_desc_name)
      {
        //WRITELOG("Free desc name: %s",VarArray[i].m_desc_name);
        free(VarArray[i].m_desc_name);
      }
      if(VarArray[i].m_desc_type_name)
      {
        free(VarArray[i].m_desc_type_name);
      }
    }
    free(VarArray);
    VarArray=NULL;
  }
}

SQLRETURN 
ODBC_IRD::SetItemsCount(int Count)
{
  if(Count==0)
  {
    return SQL_ERROR;
  }
  if(Count==m_desc_count)
  {
    return SQL_SUCCESS;
  }
  if(Count == RESET_IRDARD_DESCRIPTOR)
  {
    DoneVarArray();
    m_desc_count       = 0;
    m_desc_count_alloc = 0;
    m_rows_processed;
    return SQL_SUCCESS;
  }
  if(Count > m_desc_count_alloc)
  {
    int oldMax = m_desc_count_alloc;
    m_desc_count_alloc = (Count / DESCRIPTOR_BLOCK_SIZE + 1) * DESCRIPTOR_BLOCK_SIZE;
    VarArray = (OracleItems*)realloc(VarArray,sizeof(OracleItems) * m_desc_count_alloc);		
    for(int i=oldMax; i<m_desc_count_alloc; i++)
    {
      VarArray[i].Init();
    }
  }
  else
  {
    if(Count<m_desc_count)
    {
      for(int i=Count;i<m_desc_count;i++)
      {
        VarArray[i].Done(m_statement);
      }
    }
  }
  m_desc_count = (short)Count;
  return SQL_SUCCESS;
}

void 
ODBC_IRD::DoneObject()
{
  DoneVarArray();
  m_desc_count       = 0;
  m_desc_count_alloc = 0;
  m_desc_rows_processed_ptr = NULL;
}

SQLRETURN
ODBC_IRD::FreeObjectHandler()
{
  delete this;
  return SQL_SUCCESS;
}

SQLRETURN 
ODBC_IRD::BindAllParams(ODBC_ARD* p_ARD)
{
  m_statement->GetOCIErrorObject()->Clear();
  sword ociRes;
  int maxLength = m_statement->GetMaxLength();
  int bindSize  = 0;

  OCIStmt*  ociStatement = m_statement->GetOCIStatement();
  OCIError* ociError     = m_statement->GetOCIErrorObject()->GetOwnerErrorObject();

  for(short i=0; i<m_desc_count; ++i)
  {
    OracleItems* record = GetItem(i);
    ODBCItems*   item   = p_ARD->GetItem(i);

    if(record->IsBinaryValue())
    {
      // Need round trip to the server to determine the bindsize 
      // Nowhere to be found in Oracle documentation (OCI manual) but 
      // otherwise it won't work!!!!!!
      if(record->m_valueType == RAW_TYPE)
      {
        // Use always the maximum value for RAW. 
        // Otherwise OCI will generate errors
        record->m_desc_length = 2000;
      }
      else if(record->m_valueType == LONGRAW_TYPE)
      {
        // Use driver/statement ATTR_MAX_LENGTH
        // If not set, use arbitrary length of 100K Bytes
        record->m_desc_length = (maxLength > 0) ? maxLength : 100*1024;
      }
    }
    bindSize = record->m_desc_length;
    if(maxLength && (bindSize > maxLength))
    {
      bindSize = maxLength;
    }
    dvoid* valuep = record->MapToPtrValue();
    dvoid* indp   = &(record->m_indicator);
    sb2    type   = record->GetBindType();
    ub4    mode   = OCI_DEFAULT;

    // PIECEWISE OPERATION?
    if(*item->m_desc_indicator_ptr == SQL_DATA_AT_EXEC ||
       *item->m_desc_indicator_ptr <= SQL_LEN_DATA_AT_EXEC_OFFSET)
    {
      // Different bind type for SQLParamData/SQLPutData cycle
      mode   = OCI_DATA_AT_EXEC;
      valuep = NULL;
      indp   = NULL;
      type   = record->GetPiecewiseType();

      if(type == 0)
      {
        // Invalid parameter type
        m_ociError.AddError("HY005");
        return SQL_ERROR;
      }
      if(*item->m_desc_indicator_ptr <= SQL_LEN_DATA_AT_EXEC_OFFSET)
      {
        // Recalculate the total pending length from the indicator of the APD
        bindSize = 
        record->m_pendingLength = -((*item->m_desc_indicator_ptr) - SQL_LEN_DATA_AT_EXEC_OFFSET);
      }
      else
      {
        // Without knowing the length beforehand, we cannot do anything.
        // Besides the SQL_NEED_LONG_DATA_LEN attribute of the connections requests so!!
        // Length mismatch error
        m_ociError.AddError("22026");
        return SQL_ERROR;
      }
      if(bindSize > record->GetMaxTypeLength())
      {
        // Piecewise operation cannot extend beyond maximum limit
        m_ociError.AddError("HY090");
        return SQL_ERROR;
      }
    }

    //     WRITELOG("OCI statement handle: %p",ociStatement);
    //     WRITELOG("OCI Bind      handle: %p",&(record->OCIObject.lpBind));
    //     WRITELOG("OCI Error     handle: %p",ociError);
    //     WRITELOG("Column number       : %d",i+1);
    //     WRITELOG("Bind buffer         : %p",record->MapToPtrValue());
    //     WRITELOG("Bind size           : %d",bindSize);
    //     WRITELOG("ORACLE SQLT_ type   : %d",record->GetBindType());
    //     WRITELOG("NULL indicator hold : %p",&(record->m_indicator));

    ociRes = OCIBindByPos(ociStatement
                         ,&(record->OCIObject.lpBind)
                         ,ociError
                         ,i+1
                         ,valuep
                         ,bindSize
                         ,type
                         ,indp
                         ,0
                         ,0
                         ,0
                         ,0
                         ,mode);
    if(ociRes!=OCI_SUCCESS && ociRes!=OCI_SUCCESS_WITH_INFO)
    {
      WRITELOG("BindAllParams(%p) error Param Number=%d",m_statement,i+1);
      m_statement->GetOCIErrorObject()->GetErrorFromOCIHandle();
      return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
    }
  }
  return SQL_SUCCESS;
}

SQLRETURN 
ODBC_IRD::DefineAllColumns()
{
  m_statement->GetOCIErrorObject()->Clear();
  sword ociRes;
  int maxLength = m_statement->GetMaxLength();
  int bindSize  = 0;

  for(int i=0;i<m_desc_count;i++)
  {
    OracleItems* record = GetItem(i);

    bindSize = record->m_desc_length;
    if(maxLength && (bindSize > maxLength))
    {
      bindSize = maxLength;
    }
    //     WRITELOG("OCI statement handle: %p",m_statement->GetOCIStatement());
    //     WRITELOG("OCI Define    handle: %p",&record->OCIObject.lpDefine);
    //     WRITELOG("OCI Error     handle: %p",m_statement->GetOCIErrorObject()->GetOwnerErrorObject());
    //     WRITELOG("Column number       : %d",i+1);
    //     WRITELOG("Bind buffer         : %p",record->MapToPtrValue());
    //     WRITELOG("Bind size           : %d",bindSize);
    //     WRITELOG("ORACLE SQLT_ type   : %d",record->GetBindType());
    //     WRITELOG("NULL indicator hold : %p",&(record->m_NULLIndicator));
    //     WRITELOG("Returned size       : %p",&(record->m_returnedSize));

    ociRes = OCIDefineByPos(m_statement->GetOCIStatement()
                           ,&record->OCIObject.lpDefine
                           ,m_statement->GetOCIErrorObject()->GetOwnerErrorObject()
                           ,i+1                        // Column number
                           ,record->MapToPtrValue()    // &Value.<whatever>
                           ,bindSize                   // Max bind size
                           ,record->GetBindType()      // SQLT_* type
                           ,&(record->m_indicator)     // Address of the NULL indicator
                           ,&(record->m_returnedSize)
                           ,0
                           ,(ub4)OCI_DEFAULT);
    if(ociRes != OCI_SUCCESS && ociRes != OCI_SUCCESS_WITH_INFO)
    {
      WRITELOG("DefineAllColumns(%p) error Column Number=%d",m_statement,i+1);
      m_statement->GetOCIErrorObject()->GetErrorFromOCIHandle();
      return TranslateOCIReturnCodeToSQLReturnCode(ociRes);
    }
  }
  return SQL_SUCCESS;
}

SQLSMALLINT 
ODBC_IRD::ConvertInternalToODBCType(OraTypesEnumeration Type)
{
  return OraTypesEnumeration_to_ODBC[Type].ODBC_Type;
}

OracleItems*
ODBC_IRD::AllocItem(short Item)
{
  if(Item < 0)
  {
    return NULL;
  }
  if(Item >= m_desc_count)
  {
    if(Item >= m_desc_count_alloc)
    {
      m_desc_count_alloc = ((Item + 1) / DESCRIPTOR_BLOCK_SIZE + 1) * DESCRIPTOR_BLOCK_SIZE;
      VarArray=(OracleItems*)realloc(VarArray,sizeof(OracleItems)*m_desc_count_alloc);		
      for(int i=m_desc_count;i<m_desc_count_alloc;++i)
      {
        VarArray[i].Init();
      }
    }
    for(int i=m_desc_count;i<Item;++i)
    {
      VarArray[i].Init();
    }
    m_desc_count=Item+1;
  }
  return &VarArray[Item];
}

OraTypesEnumeration 
ODBC_IRD::ConvertOracleTypeToInternal(int Type)
{
  for(int i=0,len=sizeof(OraTypes_to_OraTypesEnumeration)/sizeof(OraTypes_to_OraTypesEnumeration[0]);i<len;i++)
  {
    if(OraTypes_to_OraTypesEnumeration[i].OraType==Type)
    {
      return OraTypes_to_OraTypesEnumeration[i].TypeValue;
    }
  }
  return UNKNOWN_TYPE;
}

// After each fetch, we increment the rows-processed 
void
ODBC_IRD::ProcessRow()
{
  ++m_rows_processed;
  if(m_desc_rows_processed_ptr)
  {
    *m_desc_rows_processed_ptr = m_rows_processed;
  }
}

// Find  a column by way of the bind handle
short
ODBC_IRD::FindBindHandle(OCIBind* p_bind)
{
  for(short i = 0; i <= m_desc_count; ++i)
  {
    OracleItems* item = GetItem(i);
    if(item->OCIObject.lpBind == p_bind)
    {
      return i;
    }
  }
  return -1;
}

SQLRETURN
ODBC_IRD::SQLGetDescField(SQLSMALLINT RecNumber
                         ,SQLSMALLINT FieldIdentifier
                         ,SQLPOINTER  Value
                         ,SQLINTEGER  BufferLength
                         ,SQLINTEGER* StringLength)
{
  SQLSMALLINT tiny   = 0xFF;
  SQLUINTEGER number = 0xFFFF;
  char* string = NULL;
  // HEADER FIELDS
  switch(FieldIdentifier)
  {
    case SQL_DESC_ALLOC_TYPE:         tiny = SQL_DESC_ALLOC_AUTO;
                                      break;
    case SQL_DESC_ARRAY_STATUS_PTR:   number = (SQLUINTEGER)m_desc_array_status_ptr;
                                      break;
    case SQL_DESC_COUNT:              tiny = m_desc_count;
                                      break;
    case SQL_DESC_ROWS_PROCESSED_PTR: number = (SQLUINTEGER)m_desc_rows_processed_ptr;
                                      break;
    case SQL_DESC_ARRAY_SIZE:         // Does not exist in IRD
    case SQL_DESC_BIND_OFFSET_PTR:    // Does not exist in IRD
    case SQL_DESC_BIND_TYPE:          // Does not exist in IRD
                                      m_ociError.AddError("HY091");
                                      break;
  }
  if(RecNumber < 0 || RecNumber > m_desc_count)
  {
    m_ociError.AddError("07009");
    return SQL_ERROR;
  }
  // RECORD FIELDS
  OracleItems* record = GetItem(RecNumber-1);
  switch(FieldIdentifier)
  {
    case SQL_DESC_AUTO_UNIQUE_VALUE:      // Oracle does not support auto-incrementing columns
                                          number = SQL_FALSE;
                                          break;
    case SQL_DESC_BASE_COLUMN_NAME:       // Not supported by Oracle
    case SQL_DESC_BASE_TABLE_NAME:        // report an empty string
    case SQL_DESC_CATALOG_NAME:           
    case SQL_DESC_SCHEMA_NAME:
    case SQL_DESC_TABLE_NAME:             string = "";
                                          break;
    case SQL_DESC_CASE_SENSITIVE:         switch(record->m_valueType)
                                          {
                                            case CLOB_TYPE:
                                            case STRING_TYPE:
                                            case ROWID_TYPE:   number = SQL_TRUE;
                                                               break;
                                            default:           number = SQL_FALSE;
                                                               break;
                                          }
                                          break;
    case SQL_DESC_CONCISE_TYPE:           break;
    case SQL_DESC_DATETIME_INTERVAL_CODE: switch(record->m_valueType)
                                          {
                                            case DATE_TYPE:
                                            case ANSI_DATE_TYPE:
                                            case DATETIME_TYPE:
                                            case DATETIMEWLTZ_TYPE:
                                            case DATETIMEWTZ_TYPE:    tiny = SQL_CODE_TIMESTAMP;
                                                                      break;
                                            case INTERVALYM_TYPE:     tiny = SQL_CODE_YEAR_TO_MONTH;
                                                                      break;
                                            case INTERVALDS_TYPE:     tiny = SQL_CODE_DAY_TO_SECOND;
                                                                      break;
                                            default:                  tiny = 0;
                                                                      break;
                                          }
                                          break;
    case SQL_DESC_LABEL:                  // In Oracle name and label are one and the same
    case SQL_DESC_NAME:                   string = record->m_desc_name;
                                          break;
    case SQL_DESC_NULLABLE:               tiny = record->m_desc_nullable;
                                          break;
    case SQL_DESC_OCTET_LENGTH:           number = record->m_desc_octet_length;
                                          break;
    case SQL_DESC_PRECISION:              tiny = record->m_desc_precision;
                                          break;
    case SQL_DESC_ROWVER:                 tiny = SQL_FALSE; // Could be ROWID ?
                                          break;
    case SQL_DESC_SEARCHABLE:             if(record->m_valueType == STRING_TYPE ||
                                             record->m_valueType == CLOB_TYPE)
                                          {
                                            tiny = SQL_PRED_SEARCHABLE;
                                          }
                                          else
                                          {
                                            tiny = SQL_PRED_BASIC;
                                          }
                                          break;
    case SQL_DESC_TYPE:                   tiny = ConvertInternalToODBCType(record->m_valueType);
                                          break;
    case SQL_DESC_TYPE_NAME:              string = record->m_desc_type_name;
                                          break;
    case SQL_DESC_UNNAMED:                tiny = record->m_desc_name && record->m_desc_name[0] ? SQL_NAMED : SQL_DESC_UNNAMED;
                                          break;
    case SQL_DESC_UNSIGNED:               switch(record->m_valueType)
                                          {
                                            case NUMBER_TYPE:
                                            case DOUBLE_TYPE: 
                                            case INTEGER_TYPE:  tiny = SQL_FALSE;
                                                                break;
                                            default:            tiny = SQL_TRUE;
                                                                break;
                                          }
                                          break;
    case SQL_DESC_UPDATABLE:              tiny = SQL_ATTR_READWRITE_UNKNOWN;
                                          break;
    case SQL_DESC_PARAMETER_TYPE:         // ??
    case SQL_DESC_OCTET_LENGTH_PTR:       // Not used in IPD's
    case SQL_DESC_DATA_PTR:               // Not used in IPD's
    default:                              m_ociError.AddError("HY091");
                                          break;
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
  if(string)
  {
    return CopyODBCStringToApp((SQLCHAR*)Value,string,BufferLength,(int*)StringLength,&m_ociError);
  }
  return SQL_ERROR;
}

SQLRETURN 
ODBC_IRD::SQLSetDescField(SQLSMALLINT RecNumber
                         ,SQLSMALLINT FieldIdentifier
                         ,SQLPOINTER  Value
                         ,SQLINTEGER  /*BufferLength*/)
{
  // HEADER FIELDS
  switch(FieldIdentifier)
  {
    case SQL_DESC_ARRAY_STATUS_PTR:   m_desc_array_status_ptr = (SQLUSMALLINT*)Value;
                                      return SQL_SUCCESS;
    case SQL_DESC_ROWS_PROCESSED_PTR: m_desc_rows_processed_ptr = (SQLUINTEGER*)Value;
                                      return SQL_SUCCESS;
    case SQL_DESC_ARRAY_SIZE:         // Does not exist in IRD
    case SQL_DESC_BIND_OFFSET_PTR:    // Does not exist in IRD
    case SQL_DESC_BIND_TYPE:          // Does not exist in IRD
                                      m_ociError.AddError("HY091");
                                      return SQL_ERROR;
    case SQL_DESC_ALLOC_TYPE:         // Read only. Cannot be set     
    case SQL_DESC_COUNT:              // Read only. Cannot be set     
                                      m_ociError.AddError("HY016");
                                      return SQL_ERROR;
  }
  if(RecNumber < 0 || RecNumber > m_desc_count)
  {
    m_ociError.AddError("07009");
  }
  // RECORD FIELDS
  if(IsRowDescriptor())
  {
    // Row descriptors are always read-only
    m_ociError.AddError("HY091");
    return SQL_ERROR;
  }
  OracleItems* record = GetItem(RecNumber-1);
  switch(FieldIdentifier)
  {
    case SQL_DESC_CONCISE_TYPE:                 // TODO: Discover CONCISE_TYPE
    case SQL_DESC_DATETIME_INTERVAL_CODE:       // TODO: Discover INTERVAL_CODE
    case SQL_DESC_DATETIME_INTERVAL_PRECISION:  break;
    case SQL_DESC_NAME:                         record->SetDescName((const char*)Value,(int)strlen((char*)Value));
                                                break;
    case SQL_DESC_NUM_PREC_RADIX:               if((int)Value == 0)
                                                {
                                                  record->m_desc_precision = 0;
                                                  record->m_desc_scale = 0;
                                                }
                                                else
                                                {
                                                  // TODO: Don't know what to do with Numeric RADIX
                                                }
                                                break;
    case SQL_DESC_OCTET_LENGTH:                 record->m_desc_octet_length = (int)Value;
                                                break;
    case SQL_DESC_PRECISION:                    record->m_desc_precision = (sb2) Value;
                                                break;
    case SQL_DESC_SCALE:                        record->m_desc_scale = (sb1) Value;
                                                break;
    case SQL_DESC_AUTO_UNIQUE_VALUE:            // Oracle does not support auto-incrementing columns
                                                m_ociError.AddError("HY016");
                                                return SQL_ERROR;
    case SQL_DESC_PARAMETER_TYPE:               // Works only for the APD
    default:                                    m_ociError.AddError("HY091");
                                                return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

SQLRETURN 
ODBC_IRD::SQLSetDescRec(SQLSMALLINT /*RecNumber     */
                       ,SQLSMALLINT /*Type          */
                       ,SQLSMALLINT /*SubType       */
                       ,SQLINTEGER  /*Length        */
                       ,SQLSMALLINT /*Precision     */
                       ,SQLSMALLINT /*Scale         */
                       ,SQLPOINTER  /*Data          */
                       ,SQLINTEGER* /*StringLength  */
                       ,SQLINTEGER* /*Indicator     */)
{
  // Row descriptors are always read-only in ODBC 2.x
  // If you want to set an IPD, use SQLSetDescField
  m_ociError.AddError("HY091");
  return SQL_ERROR;
}

SQLRETURN 
ODBC_IRD::SQLGetDescRec(SQLSMALLINT  RecNumber
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
    // Descriptor field out of range
    m_ociError.AddError("07009");
  }
  OracleItems* record = GetItem(RecNumber-1);
  if(Type)
  {
    *Type = (SQLSMALLINT)ConvertInternalToODBCType(record->m_valueType);

  }
  // Subtype // TODO
  if(SubType)
  {
    *SubType = 0;
  }
  if(Length)
  {
    *Length = record->m_desc_octet_length;
  }
  if(Precision)
  {
    *Precision = (SQLSMALLINT) record->m_desc_precision;
  }
  if(Scale)
  {
    *Scale = (SQLSMALLINT) record->m_desc_scale;
  }
  if(Nullable)
  {
    *Nullable  = (SQLSMALLINT) record->m_desc_nullable;
  }
  // Name
  int len;
  SQLRETURN res = CopyODBCStringToApp(Name,record->m_desc_name,BufferLength,&len,&m_ociError);
  if(StringLength)
  {
    *StringLength = (SQLSMALLINT)len;
  }
  return res;
}

SQLRETURN
ODBC_IRD::CopyFrom(ODBC_IRD *p_source)
{
  // Descriptor
  m_desc_array_status_ptr   = p_source->m_desc_array_status_ptr;
  // IRD
  m_rows_processed          = p_source->m_rows_processed;
  m_desc_rows_processed_ptr = p_source->m_desc_rows_processed_ptr;
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
  for(int i=0; i<total; ++i)
  {
    OracleItems* target = GetItem(i);
    OracleItems* source = p_source->GetItem(i);
    target->CopyFrom(source);
  }
  return SQL_ERROR;
}