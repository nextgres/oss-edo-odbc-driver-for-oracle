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
//////////////////////////////////////////////////////////////////////////
//
// Contents:  OUTBOUND TRAFIC FROM DATASOURCE TO DRIVER TO APPLICATION
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "DescriptorARD.h"
#include "DescriptorIRD.h"
#include "Statement.h"
#include "ODBCTypes.h"
#include "Environment.h"
#include "Utility.h"

#pragma warning (disable: 4996)
#pragma warning (disable: 4244)

#define LOCAL_BUFFERLEN 100

#define INTVAL_TO_MONTHS(y,m)        (((y)*24)+(m))
#define INTVAL_TO_SECONDS(d,h,m,s)   (((d)*86400)+((h)*3600)+((m)*60)+(s))

SQLRETURN ConvertToSQL_C_CHAR                     (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_SSHORT                   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_USHORT                   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_SLONG                    (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_ULONG                    (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_FLOAT                    (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_DOUBLE                   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_BIT                      (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_STINYINT                 (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_UTINYINT                 (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_SBIGINT                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_UBIGINT                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_TYPE_DATE                (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_TYPE_TIME                (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_TYPE_TIMESTAMP           (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_DAY             (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_DAY_TO_HOUR     (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_DAY_TO_MINUTE   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_YEAR            (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_MONTH           (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_YEAR_TO_MONTH   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_DAY_TO_SECOND   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_HOUR            (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_HOUR_TO_MINUTE  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_HOUR_TO_SECOND  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_MINUTE          (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_MINUTE_TO_SECOND(SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_INTERVAL_SECOND          (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_BINARY                   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_NUMERIC                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertToSQL_C_GUID                     (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);

// SQL_C_XXXX Type to conversion function translation
// The order of the functions impacts on the performance of the driver
// so the order is optimized for performance.

ConversionFunctions ConversionFunctionToODBC_SQL[]=
{
  // Most used datatypes
	SQL_C_CHAR,                       ConvertToSQL_C_CHAR,
  SQL_C_NUMERIC,                    ConvertToSQL_C_NUMERIC,
  SQL_C_DOUBLE,                     ConvertToSQL_C_DOUBLE,
  SQL_C_DATE,                       ConvertToSQL_C_TYPE_DATE,
  SQL_C_TYPE_DATE,                  ConvertToSQL_C_TYPE_DATE,
  SQL_C_TIME,                       ConvertToSQL_C_TYPE_TIME,
  SQL_C_TYPE_TIME,                  ConvertToSQL_C_TYPE_TIME,
  SQL_C_TIMESTAMP,                  ConvertToSQL_C_TYPE_TIMESTAMP,
  SQL_C_TYPE_TIMESTAMP,             ConvertToSQL_C_TYPE_TIMESTAMP,
  SQL_C_BINARY,                     ConvertToSQL_C_BINARY,
  // Less used datatypes
  SQL_C_SSHORT,                     ConvertToSQL_C_SSHORT,
	SQL_C_SHORT,                      ConvertToSQL_C_SSHORT,
	SQL_C_USHORT,                     ConvertToSQL_C_USHORT,
	SQL_C_SLONG,                      ConvertToSQL_C_SLONG,
	SQL_C_LONG,                       ConvertToSQL_C_SLONG,
	SQL_C_ULONG,                      ConvertToSQL_C_ULONG,
	SQL_C_FLOAT,                      ConvertToSQL_C_FLOAT,
	SQL_C_BIT,                        ConvertToSQL_C_BIT,
	SQL_C_STINYINT,                   ConvertToSQL_C_STINYINT,
	SQL_C_TINYINT,                    ConvertToSQL_C_STINYINT,
	SQL_C_UTINYINT,                   ConvertToSQL_C_UTINYINT,
	SQL_C_SBIGINT,                    ConvertToSQL_C_SBIGINT,
	SQL_C_UBIGINT,                    ConvertToSQL_C_UBIGINT,
	SQL_C_INTERVAL_DAY,               ConvertToSQL_C_INTERVAL_DAY,
	SQL_C_INTERVAL_DAY_TO_HOUR,       ConvertToSQL_C_INTERVAL_DAY_TO_HOUR,
	SQL_C_INTERVAL_DAY_TO_MINUTE,     ConvertToSQL_C_INTERVAL_DAY_TO_MINUTE,
	SQL_C_INTERVAL_DAY_TO_SECOND,     ConvertToSQL_C_INTERVAL_DAY_TO_SECOND,
	SQL_C_INTERVAL_HOUR,              ConvertToSQL_C_INTERVAL_HOUR,
	SQL_C_INTERVAL_HOUR_TO_MINUTE,    ConvertToSQL_C_INTERVAL_HOUR_TO_MINUTE,
	SQL_C_INTERVAL_HOUR_TO_SECOND,    ConvertToSQL_C_INTERVAL_HOUR_TO_SECOND,
	SQL_C_INTERVAL_MINUTE,            ConvertToSQL_C_INTERVAL_MINUTE,
	SQL_C_INTERVAL_MINUTE_TO_SECOND,  ConvertToSQL_C_INTERVAL_MINUTE_TO_SECOND,
	SQL_C_INTERVAL_SECOND,            ConvertToSQL_C_INTERVAL_SECOND,
	SQL_C_INTERVAL_YEAR,              ConvertToSQL_C_INTERVAL_YEAR,
	SQL_C_INTERVAL_MONTH,             ConvertToSQL_C_INTERVAL_MONTH,
	SQL_C_INTERVAL_YEAR_TO_MONTH,     ConvertToSQL_C_INTERVAL_YEAR_TO_MONTH,
  SQL_C_GUID,                       ConvertToSQL_C_GUID
};

// CONVERT FROM DATASOURCE TO ODBC FORMAT
SQLRETURN 
DatasourceToDriver(SQLSMALLINT       ODBC_Type
                  ,SQLPOINTER        DataPtr
                  ,SQLLEN            DataLen
                  ,SQLLEN*           StrLen_or_IndPtr
                  ,OracleItems&      Holder
                  ,ODBCErrorHolder&  ociError
                  ,ODBCStatement*    p_statement
                  ,bool              p_getData)
{
  if(Holder.m_indicator == SQL_NULL_DATA || !DataPtr)
  {
    if(StrLen_or_IndPtr)
    {
      if(p_getData)
      {
        Holder.m_pendingLength = SQL_NULL_DATA;
      }
      *StrLen_or_IndPtr = SQL_NULL_DATA;
      return SQL_SUCCESS;
    }
    ociError.AddError("22002");
    return SQL_ERROR;
  }
  if(Holder.m_toFunction && Holder.m_toODBCType == ODBC_Type)
  {
    // Use cached conversion function
    return (*Holder.m_toFunction)(DataPtr,DataLen,(int*)StrLen_or_IndPtr,Holder,ociError,p_statement,p_getData);
  }
	for(int i=0; i < sizeof(ConversionFunctionToODBC_SQL)/sizeof(ConversionFunctionToODBC_SQL[0]); ++i)
	{
		if(ConversionFunctionToODBC_SQL[i].ODBC_Type == ODBC_Type)
    {
      // Cache this conversion function
      Holder.m_toFunction = &ConversionFunctionToODBC_SQL[i].ConversionFunction;
      Holder.m_toODBCType = ODBC_Type;
      return ConversionFunctionToODBC_SQL[i].ConversionFunction(DataPtr
                                                               ,DataLen
                                                               ,(int*)StrLen_or_IndPtr
                                                               ,Holder
                                                               ,ociError
                                                               ,p_statement
                                                               ,p_getData);
    }
	}
  return TypeConversionError(ociError,Holder);
}

//////////////////////////////////////////////////////////////////////////
//
// Conversion Functions
//

SQLRETURN 
ConvertToSQL_C_CHAR(SQLPOINTER         DataPtr
                   ,int                DataLen
                   ,int*               ResultLen
                   ,OracleItems&       Holder
                   ,ODBCErrorHolder&   ociError
                   ,ODBCStatement*     p_statement
                   ,bool               p_getData)
{
	SQLCHAR* TextBuff = (SQLCHAR*)DataPtr;
  char  LocalBuffer[LOCAL_BUFFERLEN] = "";

  // Make result initially empty
  if(ResultLen)
  {
    *ResultLen = 0;
  }
  TextBuff[0] = 0;
  // Initially error result
  SQLRETURN result = SQL_ERROR;
  bool timezone = false;

  switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:        itoa(Holder.Value.int_val,LocalBuffer,10);
                              result = CopyODBCStringToApp(TextBuff
                                                          ,LocalBuffer
                                                          ,DataLen
                                                          ,ResultLen
                                                          ,&ociError
                                                          ,&Holder
                                                          ,p_statement->GetMaxLength()
                                                          ,p_getData);
			                        break;
		case DOUBLE_TYPE:         DoubleToString(LocalBuffer,Holder.Value.double_val);
                              result = CopyODBCStringToApp(TextBuff
                                                          ,LocalBuffer
                                                          ,DataLen
                                                          ,ResultLen
                                                          ,&ociError
                                                          ,&Holder
                                                          ,p_statement->GetMaxLength()
                                                          ,p_getData);
			                        break;
		case DATE_TYPE:           if(DateToString(p_statement->GetDBConnection()
                                             ,&Holder.Value.date_val
                                             ,LocalBuffer
                                             ,LOCAL_BUFFERLEN
                                             ,&ociError) == SQL_SUCCESS)
                              {
                                result = CopyODBCStringToApp(TextBuff
                                                            ,LocalBuffer
                                                            ,DataLen
                                                            ,ResultLen
                                                            ,&ociError
                                                            ,&Holder
                                                            ,p_statement->GetMaxLength()
                                                            ,p_getData);
                              }
			                        break;
		case NUMBER_TYPE:         {
                                ub4 bufSize = LOCAL_BUFFERLEN;
				                        if(OCINumberToText(ociError.GetOwnerErrorObject()
                                                  ,&Holder.Value.number_val           // Input the OCINumber
                                                  ,(const oratext*) "TM9"             // TM = Text minimum
                                                  ,3                                  // Format length
                                                  ,(oratext*)NLS_NUMERIC_CHARACTERS   // NLS_PARAMS use default
                                                  ,NLS_LENGTH                         // Length of NLS PARAMS
                                                  ,&bufSize                           // Buffer size
                                                  ,(oratext*)LocalBuffer) != OCI_SUCCESS)
                                {
                                  LocalBuffer[0] = 0;
                                  ociError.GetErrorFromOCIHandle();
                                  return SQL_ERROR;
                                }
                                result = CopyODBCStringToApp(TextBuff,LocalBuffer,DataLen,ResultLen,&ociError
                                                            ,&Holder
                                                            ,p_statement->GetMaxLength()
                                                            ,p_getData);
			                        }
			                        break;
		case STRING_TYPE:         {
				                        char *str = (char*)	OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                                    ,Holder.Value.string_val);
                                if(* p_statement->m_SQLDataSourceToDriver)
                                {
                                  // TRANSLATION LIB is loaded. So try it.
                                  SWORD  errorLen = 0;
                                  UCHAR  error[257] = "";
                                  UDWORD options = *(p_statement->m_translateOptions);
                                  SWORD  type = SQL_VARCHAR; // OraTypesEnumeration_to_ODBC[Holder.m_valueType].ODBC_Type;
                                  SQLRETURN ret = (**p_statement->m_SQLDataSourceToDriver)(options
                                                                                          ,type
                                                                                          ,str
                                                                                          ,(int)strlen(str)
                                                                                          ,str
                                                                                          ,DataLen
                                                                                          ,(SDWORD*)ResultLen
                                                                                          ,error
                                                                                          ,256
                                                                                          ,&errorLen);
                                  if(ret == SQL_TRUE && error[0])
                                  {
                                    ociError.AddError("R1003",0,(const char*)error);
                                  }
                                }
                                result = CopyODBCStringToApp(TextBuff
                                                            ,str
                                                            ,DataLen
                                                            ,ResultLen
                                                            ,&ociError
                                                            ,&Holder
                                                            ,p_statement->GetMaxLength()
                                                            ,p_getData);
			                        }
			                        break;
    case LONG_TYPE:           {
                                // We know the length of the LONG, so use it!!
                                int len = *((int*)Holder.Value.long_val);
                                int maxlen = p_statement->GetMaxLength();
                                if(maxlen == 0) maxlen = len + 1;
                                result = CopyODBCStringToApp(TextBuff
                                                            ,(char*)(Holder.Value.long_val + sizeof(int))
                                                            ,DataLen
                                                            ,ResultLen
                                                            ,&ociError
                                                            ,&Holder
                                                            ,maxlen
                                                            ,p_getData);
                              }
                              break;
    case CLOB_TYPE:           result = CopyODBCLOBToApp(p_statement->GetDBConnection()->GetOCIConnection()
                                                       ,DataPtr,Holder.Value.lob_val
                                                       ,DataLen
                                                       ,ResultLen
                                                       ,ociError
                                                       ,&Holder
                                                       ,p_statement->GetMaxLength()
                                                       ,p_getData);
                              break;
		case DATETIMEWTZ_TYPE:    // Fall through
		case DATETIMEWLTZ_TYPE:   timezone = true;
    case DATETIME_TYPE:       // Fall through
    case ANSI_DATE_TYPE:      if(TimestampToString(p_statement->GetDBConnection()
                                                  ,Holder.Value.datetime_val
                                                  ,LocalBuffer
                                                  ,LOCAL_BUFFERLEN
                                                  ,timezone
                                                  ,&ociError) == SQL_SUCCESS)
                              {
                                result = CopyODBCStringToApp(TextBuff
                                                            ,LocalBuffer
                                                            ,DataLen
                                                            ,ResultLen
                                                            ,&ociError
                                                            ,&Holder
                                                            ,p_statement->GetMaxLength()
                                                            ,p_getData);
                              }
			                        break;
		case INTERVALYM_TYPE:     // Fall through
		case INTERVALDS_TYPE:     {
                                size_t size = 0;
                                OCIEnv* environment = p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv();
                                if(OCIIntervalToText(environment
                                                    ,ociError.GetOwnerErrorObject()
                                                    ,Holder.Value.interval_val
                                                    ,9
                                                    ,0
                                                    ,(oratext*)LocalBuffer
                                                    ,LOCAL_BUFFERLEN
                                                    ,&size) == OCI_SUCCESS)
                                {
                                  result = CopyODBCStringToApp(TextBuff
                                                              ,LocalBuffer
                                                              ,DataLen
                                                              ,ResultLen
                                                              ,&ociError
                                                              ,&Holder
                                                              ,p_statement->GetMaxLength()
                                                              ,p_getData);
                                }
			                        }
			                        break;
		case ROWID_TYPE:          {
				                        ub2 Len = DataLen;
				                        OCIRowidToChar(Holder.Value.rowid_val,(text*)LocalBuffer,&Len,ociError.GetOwnerErrorObject());
                                result = CopyODBCStringToApp(TextBuff
                                                            ,LocalBuffer
                                                            ,DataLen
                                                            ,ResultLen
                                                            ,&ociError
                                                            ,&Holder
                                                            ,p_statement->GetMaxLength()
                                                            ,p_getData);
			                        }
                              break;
    default:                  return TypeConversionError(ociError,Holder);
	}
	return result;
}

SQLRETURN 
ConvertToSQL_C_SSHORT(SQLPOINTER        DataPtr
                    ,int                /*DataLen*/
                    ,int*               ResultLen
                    ,OracleItems&       Holder
                    ,ODBCErrorHolder&   ociError
                    ,ODBCStatement*     p_statement
                    ,bool               /* p_getData */)
{
  int integerValue = 0;
	SQLSMALLINT& ValueBuff = *((SQLSMALLINT*)DataPtr);
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE: 			integerValue = Holder.Value.int_val;
			                      break;
		case DOUBLE_TYPE:  			integerValue = (SQLINTEGER)Holder.Value.double_val;
			                      break;
		case NUMBER_TYPE: 			if(OCINumberToInt(ociError.GetOwnerErrorObject()
					                                   ,&Holder.Value.number_val
					                                   ,sizeof(integerValue)
					                                   ,OCI_NUMBER_SIGNED
					                                   ,&integerValue) != OCI_SUCCESS)
                            {
                              ociError.GetErrorFromOCIHandle();
                              return SQL_ERROR;
                            }
			                      break;
		case STRING_TYPE:       {
				                      char *Str=(char*)OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                               ,Holder.Value.string_val);
				                      integerValue = atoi(Str);
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
			                        sb4 Year;
			                        sb4 Month;
			                        OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,&Year
				                                              ,&Month
				                                              ,Holder.Value.interval_val);
			                        integerValue = INTVAL_TO_MONTHS(Year,Month);
		                        }
		                        break;
		case INTERVALDS_TYPE:
		                        {
			                        sb4 Day;				
			                        sb4 Hour;
			                        sb4 Minute;
			                        sb4 Second;
			                        sb4 fraction;
			                        OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,&Day		
				                                              ,&Hour
				                                              ,&Minute
				                                              ,&Second
				                                              ,&fraction
				                                              ,Holder.Value.interval_val);
                              integerValue = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
		                        }
		                        break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(integerValue > 0x7fff || integerValue < -0x7fff)
  {
    // Error. Value out of range
    ociError.AddError("22003");
  }
  // This should be safe by now
  ValueBuff = (short) integerValue;
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLSMALLINT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_USHORT(SQLPOINTER      DataPtr
                    ,int              /*DataLen*/
                    ,int*             ResultLen
                    ,OracleItems&     Holder
                    ,ODBCErrorHolder& ociError
                    ,ODBCStatement*   p_statement
                    ,bool             /*p_getData*/)
{
  int integerValue = 0;
	SQLUSMALLINT& ValueBuff = *(SQLUSMALLINT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			integerValue = Holder.Value.int_val;
			                      break;
		case DOUBLE_TYPE: 			integerValue = Holder.Value.double_val;
			                      break;
		case NUMBER_TYPE:       if(OCINumberToInt(ociError.GetOwnerErrorObject()
					                                   ,&Holder.Value.number_val
					                                   ,sizeof(integerValue)
					                                   ,OCI_NUMBER_UNSIGNED
					                                   ,&integerValue) != OCI_SUCCESS)
                            {
                              ociError.GetErrorFromOCIHandle();
                              return SQL_ERROR;
                            }
  			                    break;
		case STRING_TYPE:       {
			                        char *Str=(char*)
				                        OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                  ,Holder.Value.string_val);
                              integerValue = atoi(Str);
		                        }
		                        break;
		case INTERVALYM_TYPE:   {
				                      sb4 Year;
				                      sb4 Month;
				                      OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,&Year
					                                            ,&Month
					                                            ,Holder.Value.interval_val);
				                      integerValue = INTVAL_TO_MONTHS(Year,Month);
			                      }
			                      break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction;
				                      OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,&Day		
					                                            ,&Hour
					                                            ,&Minute
					                                            ,&Second
					                                            ,&fraction
					                                            ,Holder.Value.interval_val);
				                      integerValue = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(integerValue > 0xffff || integerValue < 0)
  {
    // Error. Value out of range
    ociError.AddError("22003");
  }
  // This should be safe by now
  ValueBuff = (unsigned short) integerValue;
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLUSMALLINT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_SLONG(SQLPOINTER       DataPtr
	                  ,int              /*DataLen*/
                    ,int*             ResultLen
	                  ,OracleItems&     Holder
	                  ,ODBCErrorHolder& ociError
                    ,ODBCStatement*   p_statement
                    ,bool             /*p_getData*/)
{
	SQLINTEGER& ValueBuff = *(SQLINTEGER*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:  			ValueBuff=Holder.Value.int_val;
                        			break;
    case DOUBLE_TYPE:   			ValueBuff=Holder.Value.double_val;
			                        break;
		case NUMBER_TYPE:         OCINumberToInt(ociError.GetOwnerErrorObject()
					                                  ,&Holder.Value.number_val
					                                  ,sizeof(ValueBuff)
					                                  ,OCI_NUMBER_SIGNED
					                                  ,&ValueBuff);				
                        			break;
		case STRING_TYPE:         {
				                        char *Str=(char*)
					                        OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                        ,Holder.Value.string_val);
				                        int V=0;				
				                        sscanf(Str,"%d",&V);
				                        ValueBuff=V;
			                        }
			                        break;
		case INTERVALYM_TYPE:     {
				                        sb4 Year;
				                        sb4 Month;
				                        OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                        ,ociError.GetOwnerErrorObject()
					                        ,&Year
					                        ,&Month
					                        ,Holder.Value.interval_val);
				                        ValueBuff = INTVAL_TO_MONTHS(Year,Month);
			                        }
			                        break;
		case INTERVALDS_TYPE:     {
				                        sb4 Day;				
				                        sb4 Hour;
				                        sb4 Minute;
				                        sb4 Second;
				                        sb4 fraction;
				                        OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                              ,ociError.GetOwnerErrorObject()
					                                              ,&Day		
					                                              ,&Hour
					                                              ,&Minute
					                                              ,&Second
					                                              ,&fraction
					                                              ,Holder.Value.interval_val);
				                        ValueBuff = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                        }
			                        break;
    default:                  return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLINTEGER);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_ULONG(SQLPOINTER       DataPtr
                    ,int              /*DataLen*/
                    ,int*             ResultLen
                    ,OracleItems&     Holder
                    ,ODBCErrorHolder& ociError
                    ,ODBCStatement*   p_statement
                    ,bool             /*p_getData*/)
{
	SQLUINTEGER& ValueBuff = *(SQLUINTEGER*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			ValueBuff=Holder.Value.int_val;
			                      break;
		case DOUBLE_TYPE: 			ValueBuff=Holder.Value.double_val;
			                      break;
		case NUMBER_TYPE:       OCINumberToInt(ociError.GetOwnerErrorObject()
					                                ,&Holder.Value.number_val
					                                ,sizeof(ValueBuff)
					                                ,OCI_NUMBER_UNSIGNED
					                                ,&ValueBuff);				
			                      break;
		case STRING_TYPE:       {
				                      char *Str=(char*)
					                      OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                      ,Holder.Value.string_val);
				                      unsigned int V=0;				
				                      sscanf(Str,"%u",&V);
				                      ValueBuff=V;
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
			                        sb4 Year;
			                        sb4 Month;
			                        OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,&Year
				                                              ,&Month
				                                              ,Holder.Value.interval_val);
			                        ValueBuff = INTVAL_TO_MONTHS(Year,Month);
		                        }
		                        break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction;
				                      OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,&Day		
					                                            ,&Hour
					                                            ,&Minute
					                                            ,&Second
					                                            ,&fraction
					                                            ,Holder.Value.interval_val);
				                      ValueBuff = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLUINTEGER);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_FLOAT(SQLPOINTER       DataPtr
	                  ,int              /*DataLen*/
                    ,int*             ResultLen
	                  ,OracleItems&     Holder
	                  ,ODBCErrorHolder& ociError
                    ,ODBCStatement*   p_statement
                    ,bool             /*p_getData*/)
{
	SQLREAL& ValueBuff = *(SQLREAL*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:  			ValueBuff = (float)Holder.Value.int_val;
			                        break;
		case DOUBLE_TYPE:   			ValueBuff = (float)Holder.Value.double_val;
			                        break;
		case NUMBER_TYPE:
				                      OCINumberToReal(ociError.GetOwnerErrorObject()
				                                    ,&Holder.Value.number_val
				                                    ,sizeof(ValueBuff)
				                                    ,&ValueBuff);				
  			                      break;
		case STRING_TYPE:         {
				                        char *Str = (char*)
					                        OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                  ,Holder.Value.string_val);
				                        double value = 0.0;				
                                if(StringToDouble(Str,&value) == false)
                                {
                                  // Numeric value out of range
                                  ociError.AddError("22003");
                                  return SQL_ERROR;
                                }
		                            ValueBuff = value;
  		                        }
			                        break;
		case INTERVALYM_TYPE:     {
				                        sb4 Year;
				                        sb4 Month;
				                        OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                             ,ociError.GetOwnerErrorObject()
					                                             ,&Year
					                                             ,&Month
					                                             ,Holder.Value.interval_val);
				                        ValueBuff = INTVAL_TO_MONTHS(Year,Month);
			                        }
			                        break;
		case INTERVALDS_TYPE:     {
				                        sb4 Day;				
				                        sb4 Hour;
				                        sb4 Minute;
				                        sb4 Second;
				                        sb4 fraction;
				                        OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                               ,ociError.GetOwnerErrorObject()
				                                               ,&Day		
				                                               ,&Hour
				                                               ,&Minute
				                                               ,&Second
				                                               ,&fraction
				                                               ,Holder.Value.interval_val);
				                        ValueBuff = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                        }
			                        break;
    default:                  return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLREAL);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_DOUBLE(SQLPOINTER      DataPtr
                    ,int              /*DataLen*/
                    ,int*             ResultLen
                    ,OracleItems&     Holder
                    ,ODBCErrorHolder& ociError
                    ,ODBCStatement*   p_statement
                    ,bool             /*p_getData*/)
{
	SQLDOUBLE& ValueBuff = *(SQLDOUBLE*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			ValueBuff = Holder.Value.int_val;
			                      break;
		case DOUBLE_TYPE: 			ValueBuff = Holder.Value.double_val;
			                      break;
		case NUMBER_TYPE:				OCINumberToReal(ociError.GetOwnerErrorObject()
				                                  ,&Holder.Value.number_val
				                                  ,sizeof(ValueBuff)
				                                  ,&ValueBuff);				
			                      break;
		case STRING_TYPE:       {
				                      char *Str=(char*) OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                          						                      ,Holder.Value.string_val);
				                      double value = 0.0;				
                              if(StringToDouble(Str,&value) == false)
                              {
                                // Numeric value out of range
                                ociError.AddError("22003");
                                return SQL_ERROR;
                              }
			                        ValueBuff = value;
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
				                      sb4 Year;
				                      sb4 Month;
				                      OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                             ,ociError.GetOwnerErrorObject()
				                                             ,&Year
				                                             ,&Month
				                                             ,Holder.Value.interval_val);
				                      ValueBuff = INTVAL_TO_MONTHS(Year,Month);
			                      }
			                      break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day,Hour,Minute,Second,fraction;
				                      OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                           ,ociError.GetOwnerErrorObject()
					                                           ,&Day		
					                                           ,&Hour
					                                           ,&Minute
					                                           ,&Second
					                                           ,&fraction
					                                           ,Holder.Value.interval_val);
				                      ValueBuff = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLDOUBLE);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_BIT(SQLPOINTER         DataPtr
	                ,int                /*DataLen*/
                  ,int*               ResultLen
	                ,OracleItems&       Holder
	                ,ODBCErrorHolder&   ociError
                  ,ODBCStatement*     p_statement
                  ,bool               /*p_getData*/)
{
	SQLCHAR& ValueBuff = *(SQLCHAR*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			if(Holder.Value.int_val>0 || Holder.Value.int_val<0)
				                      ValueBuff=1;
			                      else
				                      ValueBuff=0;
			                      break;
		case DOUBLE_TYPE: 			if(Holder.Value.double_val>0 || Holder.Value.double_val<0)
				                      ValueBuff=1;
			                      else
				                      ValueBuff=0;
			                      break;
		case NUMBER_TYPE:       {
				                      int Val;
				                      OCINumberToInt(ociError.GetOwnerErrorObject()
					                                  ,&Holder.Value.number_val
					                                  ,sizeof(Val)
					                                  ,OCI_NUMBER_SIGNED
					                                  ,&Val);
				                      if(Val>0 || Val<0)
					                      ValueBuff=1;
				                      else
					                      ValueBuff=0;	
			                      }
			                      break;
		case STRING_TYPE:       {
				                      char *Str=(char*) OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                          						                      ,Holder.Value.string_val);
				                      int Val=0;				
				                      sscanf(Str,"%d",&Val);
				                      if(Val>0 || Val<0)
					                      ValueBuff=1;
				                      else
					                      ValueBuff=0;
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = 1;
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}


SQLRETURN 
ConvertToSQL_C_STINYINT(SQLPOINTER        DataPtr
                       ,int               /*DataLen*/
                       ,int*              ResultLen
                       ,OracleItems&      Holder
                       ,ODBCErrorHolder&  ociError
                       ,ODBCStatement*    p_statement
                       ,bool              /*p_getData*/)
{
	SQLSCHAR& ValueBuff = *(SQLSCHAR*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			ValueBuff = Holder.Value.int_val;
			                      break;
		case DOUBLE_TYPE: 			ValueBuff = Holder.Value.double_val;
			                      break;
		case NUMBER_TYPE:       OCINumberToInt(ociError.GetOwnerErrorObject()
				                                  ,&Holder.Value.number_val
				                                  ,sizeof(ValueBuff)
				                                  ,OCI_NUMBER_SIGNED
				                                  ,&ValueBuff);				
			                      break;
		case STRING_TYPE:       {
				                      char* Str = (char*) OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                                  ,Holder.Value.string_val);
				                      int V=0;				
				                      sscanf(Str,"%d",&V);
				                      ValueBuff=V;
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
				                      sb4 Year;
				                      sb4 Month;
				                      OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,&Year
				                                              ,&Month
				                                              ,Holder.Value.interval_val);
				                      ValueBuff = INTVAL_TO_MONTHS(Year,Month);
			                      }
			                      break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction;
				                      OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                             ,ociError.GetOwnerErrorObject()
				                                             ,&Day		
					                                           ,&Hour
					                                           ,&Minute
					                                           ,&Second
					                                           ,&fraction
					                                           ,Holder.Value.interval_val);
				                      ValueBuff = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = 1;
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_UTINYINT(SQLPOINTER        DataPtr
	                     ,int               /*DataLen*/
                       ,int*              ResultLen
	                     ,OracleItems&      Holder
	                     ,ODBCErrorHolder&  ociError
                       ,ODBCStatement*    p_statement
                       ,bool              /*p_getData*/)
{
	SQLCHAR& ValueBuff=*(SQLCHAR*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			ValueBuff=Holder.Value.int_val;
			                      break;
		case DOUBLE_TYPE: 			ValueBuff=Holder.Value.double_val;
			                      break;
		case NUMBER_TYPE:       OCINumberToInt(ociError.GetOwnerErrorObject()
					                                ,&Holder.Value.number_val
					                                ,sizeof(ValueBuff)
					                                ,OCI_NUMBER_UNSIGNED
					                                ,&ValueBuff);				
			                      break;
		case STRING_TYPE:       {
				                      char *Str=(char*) OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                                ,Holder.Value.string_val);
				                      unsigned int V=0;				
				                      sscanf(Str,"%u",&V);
				                      ValueBuff=V;
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
			                        sb4 Year;
			                        sb4 Month;
			                        OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,&Year
				                                              ,&Month
				                                              ,Holder.Value.interval_val);
			                        ValueBuff = INTVAL_TO_MONTHS(Year,Month);
		                        }
		                        break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction;
				                      OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,&Day		
					                                            ,&Hour
					                                            ,&Minute
					                                            ,&Second
					                                            ,&fraction
					                                            ,Holder.Value.interval_val);
				                      ValueBuff = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = 1;
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_SBIGINT(SQLPOINTER         DataPtr
	                    ,int                /*DataLen*/
                      ,int*               ResultLen
	                    ,OracleItems&       Holder
	                    ,ODBCErrorHolder&   ociError
                      ,ODBCStatement*     p_statement
                      ,bool               /*p_getData*/)
{
	SQLBIGINT& ValueBuff = *(SQLBIGINT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			ValueBuff = Holder.Value.int_val;
			                      break;
		case DOUBLE_TYPE: 			ValueBuff = Holder.Value.double_val;
			                      break;
		case NUMBER_TYPE:       {
				                      sb8 V = 0;
				                      OCINumberToInt(ociError.GetOwnerErrorObject()
			                                      ,&Holder.Value.number_val
			                                      ,8
                                            ,OCI_NUMBER_SIGNED
			                                      ,&V);
				                      ValueBuff = V;
			                      }
			                      break;
		case STRING_TYPE:       {
				                      char *Str = (char*)OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                            						                      ,Holder.Value.string_val);
				                      SQLBIGINT V = 0;				
				                      sscanf(Str,"%I64i",&V);
				                      ValueBuff = V;
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
			                        sb4 Year;
			                        sb4 Month;
			                        OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,&Year
				                                              ,&Month
				                                              ,Holder.Value.interval_val);
			                        ValueBuff = INTVAL_TO_MONTHS(Year,Month);
		                        }
		                        break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction;
				                      OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                           ,ociError.GetOwnerErrorObject()
					                                           ,&Day		
					                                           ,&Hour
					                                           ,&Minute
					                                           ,&Second
					                                           ,&fraction
					                                           ,Holder.Value.interval_val);
				                      ValueBuff = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLBIGINT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_UBIGINT(SQLPOINTER         DataPtr
	                    ,int                /*DataLen*/
                      ,int*               ResultLen
	                    ,OracleItems&       Holder
	                    ,ODBCErrorHolder&   ociError
                      ,ODBCStatement*     p_statement
                      ,bool               /*p_getData*/)
{
	SQLUBIGINT& ValueBuff = *(SQLUBIGINT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			ValueBuff=Holder.Value.int_val;
			                      break;
		case DOUBLE_TYPE: 			ValueBuff=Holder.Value.double_val;
			                      break;
		case NUMBER_TYPE:       {
			                        ub8 V = 0;
			                        OCINumberToInt(ociError.GetOwnerErrorObject()
				                                    ,&Holder.Value.number_val
				                                    ,8
				                                    ,OCI_NUMBER_UNSIGNED
                                            ,&V);
			                        ValueBuff = V;				
		                        }
		                        break;
		case STRING_TYPE:       {
				                      char *Str=(char*) OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                                ,Holder.Value.string_val);
				                      ub8 V = 0;				
				                      sscanf(Str,"%I64u",&V);
				                      ValueBuff=V;
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
			                        sb4 Year;
			                        sb4 Month;
			                        OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,&Year
				                                              ,&Month
				                                              ,Holder.Value.interval_val);
			                        ValueBuff = INTVAL_TO_MONTHS(Year,Month);
		                        }
		                        break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction;
				                      OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,&Day		
					                                            ,&Hour
					                                            ,&Minute
					                                            ,&Second
					                                            ,&fraction
					                                            ,Holder.Value.interval_val);
				                      ValueBuff = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLUBIGINT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_TYPE_DATE(SQLPOINTER         DataPtr
	                      ,int                /*DataLen*/
                        ,int*               ResultLen
	                      ,OracleItems&       Holder
	                      ,ODBCErrorHolder&   ociError
                        ,ODBCStatement*     p_statement
                        ,bool               /*p_getData*/)
{
	SQL_DATE_STRUCT& ValueBuff = *(SQL_DATE_STRUCT*)DataPtr;
	ValueBuff.year=0;
	ValueBuff.month=0;
	ValueBuff.day=0;
	switch(Holder.m_valueType)
	{
		case DATE_TYPE:       {
				                    sb2 Year;
				                    ub1 Month;
				                    ub1 Day;
                    				
				                    OCIDateGetDate(&Holder.Value.date_val,&Year,&Month,&Day);
				                    ValueBuff.year  = Year;
				                    ValueBuff.month = Month;
				                    ValueBuff.day   = Day;				
			                    }			
			                    break;
		case STRING_TYPE:     {
                            OCIDate date;
                            sb2 Year;
                            ub1 Month;
                            ub1 Day;
                            ODBCConnection* conn = p_statement->GetDBConnection();

                            char *Str=(char*) OCIStringPtr(conn->GetOwnerODBCEnv()->GetOwnerEnv()
                                                          ,Holder.Value.string_val);
                            OCIDateFromText(ociError.GetOwnerErrorObject()
                                           ,(OraText*)Str
                                           ,(int)strlen(Str)
                                           ,(OraText*)conn->GetNLSDateFormat().GetString()
                                           ,conn->GetNLSDateFormat().GetLength()
                                           ,NULL            // Language
                                           ,0               // Length of language
                                           ,&date);
                            OCIDateGetDate(&date,&Year,&Month,&Day);
                            ValueBuff.year  = Year;
                            ValueBuff.month = Month;
                            ValueBuff.day   = Day;				
                          }
                          break;
		case DATETIME_TYPE:
		case DATETIMEWTZ_TYPE:
		case DATETIMEWLTZ_TYPE:
		case ANSI_DATE_TYPE:  {				
				                    sb2 Year;
				                    ub1 Month;
				                    ub1 Day;
                    				
				                    OCIDateTimeGetDate(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                    ,ociError.GetOwnerErrorObject()
					                                    ,Holder.Value.datetime_val
					                                    ,&Year,&Month,&Day);
				                    ValueBuff.year  = Year;
				                    ValueBuff.month = Month;
				                    ValueBuff.day   = Day;
			                    }
			                    break;
    default:              return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_DATE_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_TYPE_TIME(SQLPOINTER         DataPtr
	                      ,int                /*DataLen*/
                        ,int*               ResultLen
	                      ,OracleItems&       Holder
	                      ,ODBCErrorHolder&   ociError
                        ,ODBCStatement*     p_statement
                        ,bool               /*p_getData*/)
{
	SQL_TIME_STRUCT &ValueBuff=*(SQL_TIME_STRUCT*)DataPtr;
	ValueBuff.hour   = 0;
	ValueBuff.minute = 0;
	ValueBuff.second = 0;
	switch(Holder.m_valueType)
	{
		case DATE_TYPE:       {
				                    ub1 Hour;
				                    ub1 Minute;
				                    ub1 Second;
                    				
				                    OCIDateGetTime(&Holder.Value.date_val,&Hour,&Minute,&Second);
				                    ValueBuff.hour   = Hour;
				                    ValueBuff.minute = Minute;
				                    ValueBuff.second = Second;
			                    }		
			                    break;
    case STRING_TYPE:     {
                            char buffer[50];
                            OCIDateTime* dateTime = (OCIDateTime*)buffer;
                            ub1 Hour;
                            ub1 Min;
                            ub1 Sec;
                            ub4 Frac;
                            ODBCConnection* conn = p_statement->GetDBConnection();
                            OCIEnv* environment  = conn->GetOwnerODBCEnv()->GetOwnerEnv();
                            char *Str = (char*)OCIStringPtr(environment,Holder.Value.string_val);
                            OCIDateTimeFromText(environment
                                               ,ociError.GetOwnerErrorObject()
                                               ,(OraText*)Str
                                               ,strlen(Str)
                                               ,(OraText*)conn->GetNLSTimestampFormat().GetString()
                                               ,conn->GetNLSTimestampFormat().GetLength()
                                               ,NULL            // Language
                                               ,0               // Length of language
                                               ,dateTime);
                            OCIDateTimeGetTime(environment
                                              ,ociError.GetOwnerErrorObject()
                                              ,dateTime
                                              ,&Hour,&Min,&Sec,&Frac);
                            ValueBuff.hour   = Hour;
                            ValueBuff.minute = Min;
                            ValueBuff.second = Sec;
                          }
                          break;
		case DATETIME_TYPE:
		case DATETIMEWTZ_TYPE:
		case DATETIMEWLTZ_TYPE:
		case ANSI_DATE_TYPE:
			                    {				
				                    ub1 Hour;
				                    ub1 Minute;
				                    ub1 Second;
				                    ub4 fSec;
				                    OCIDateTimeGetTime(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                    ,ociError.GetOwnerErrorObject()
					                                    ,Holder.Value.datetime_val
					                                    ,&Hour,&Minute,&Second,&fSec);
                    				
				                    ValueBuff.hour   = Hour;
				                    ValueBuff.minute = Minute;
				                    ValueBuff.second = Second;
			                    }
			                    break;
    default:              return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_TIME_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_TYPE_TIMESTAMP(SQLPOINTER        DataPtr
	                           ,int               /*DataLen*/
                             ,int*              ResultLen
	                           ,OracleItems&      Holder
	                           ,ODBCErrorHolder&  ociError
                             ,ODBCStatement*    p_statement
                             ,bool              /*p_getData*/)
{
	SQL_TIMESTAMP_STRUCT& ValueBuff = *(SQL_TIMESTAMP_STRUCT*)DataPtr;
  memset(&ValueBuff,0,sizeof(SQL_TIMESTAMP_STRUCT));
	switch(Holder.m_valueType)
	{
		case DATE_TYPE:     {
				                  sb2 Year;
				                  ub1 Month;
				                  ub1 Day;
				                  ub1 Hour;
				                  ub1 Minute;
				                  ub1 Second;
                  				
				                  OCIDateGetDate(&Holder.Value.date_val,&Year,&Month,&Day);
				                  OCIDateGetTime(&Holder.Value.date_val,&Hour,&Minute,&Second);
                  				
				                  ValueBuff.year=Year;
				                  ValueBuff.month=Month;
				                  ValueBuff.day=Day;
				                  ValueBuff.hour=Hour;
				                  ValueBuff.minute=Minute;
				                  ValueBuff.second=Second;
			                  }
			                  break;
    case STRING_TYPE:   {
                          char buffer[50];
                          OCIDateTime* dateTime = (OCIDateTime*)buffer;
                          sb2 Year;
                          ub1 Month;
                          ub1 Day,Hour,Min,Sec;
                          ub4 Frac;
                          char *Str=(char*) OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,Holder.Value.string_val);
                          StringToTimestamp(p_statement->GetDBConnection()
                                           ,Str
                                           ,false
                                           ,dateTime
                                           ,&ociError);
                          OCIDateTimeGetDate(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                            ,ociError.GetOwnerErrorObject()
                                            ,dateTime
                                            ,&Year,&Month,&Day);
                          OCIDateTimeGetTime(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                            ,ociError.GetOwnerErrorObject()
                                            ,dateTime
                                            ,&Hour,&Min,&Sec,&Frac);
                          ValueBuff.year     = Year;
                          ValueBuff.month    = Month;
                          ValueBuff.day      = Day;
                          ValueBuff.hour     = Hour;
                          ValueBuff.minute   = Min;
                          ValueBuff.second   = Sec;
                          ValueBuff.fraction = Frac;
                        }
                        break;
		case DATETIME_TYPE:
		case DATETIMEWTZ_TYPE:
		case DATETIMEWLTZ_TYPE:
		case ANSI_DATE_TYPE:
			                  {				
				                  sb2 Year;
				                  ub1 Month;
				                  ub1 Day;
                  				
				                  ub1 Hour;
				                  ub1 Minute;
				                  ub1 Second;
				                  ub4 fSec;
				                  OCIDateTimeGetDate(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                            ,ociError.GetOwnerErrorObject()
                                            ,Holder.Value.datetime_val
                                            ,&Year,&Month,&Day);
				                  OCIDateTimeGetTime(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                    ,ociError.GetOwnerErrorObject()
				                                    ,Holder.Value.datetime_val
				                                    ,&Hour,&Minute,&Second,&fSec);
				                  ValueBuff.year     = Year;
				                  ValueBuff.month    = Month;
				                  ValueBuff.day      = Day;
				                  ValueBuff.hour     = Hour;
				                  ValueBuff.minute   = Minute;
				                  ValueBuff.second   = Second;
				                  ValueBuff.fraction = fSec;
			                  }
			                  break;
    default:            return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_TIMESTAMP_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_YEAR(SQLPOINTER         DataPtr
                            ,int                /*DataLen*/
                            ,int*               ResultLen
                            ,OracleItems&       Holder
                            ,ODBCErrorHolder&   ociError
                            ,ODBCStatement*     p_statement
                            ,bool               /*p_getData*/)
{
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type = SQL_IS_YEAR;
	ValueBuff.interval_sign = SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALYM_TYPE:   {
				                      sb4 Year;
				                      sb4 Month;
				                      OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,&Year
					                                            ,&Month
					                                            ,Holder.Value.interval_val);
				                      if(Year>=0)
				                      {
					                      ValueBuff.intval.year_month.year=Year;
					                      ValueBuff.intval.year_month.month=0;
				                      }
                              else
				                      {
					                      ValueBuff.intval.year_month.year=-Year;
					                      ValueBuff.intval.year_month.month=0;
					                      ValueBuff.interval_sign=SQL_TRUE;
				                      }
			                      }			
			                      break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction;
				                      OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,&Day		
					                                            ,&Hour
					                                            ,&Minute
					                                            ,&Second
					                                            ,&fraction
					                                            ,Holder.Value.interval_val);
				                      if(Day>=0)
				                      {
					                      ValueBuff.intval.year_month.year  = Day / 365;
					                      ValueBuff.intval.year_month.month = 0;
				                      }
                              else
				                      {
					                      ValueBuff.intval.year_month.year  = -Day/365;
					                      ValueBuff.intval.year_month.month = 0;
					                      ValueBuff.interval_sign=SQL_TRUE;
				                      }
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_MONTH(SQLPOINTER        DataPtr
 	                           ,int               /*DataLen*/
                             ,int*              ResultLen
	                           ,OracleItems&      Holder
	                           ,ODBCErrorHolder&  ociError
                             ,ODBCStatement*    p_statement
                             ,bool              /*p_getData*/)
{
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_MONTH;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALYM_TYPE: {
				                    sb4 Year;
				                    sb4 Month;
				                    OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,&Year
					                                          ,&Month
					                                          ,Holder.Value.interval_val);
				                    if(Year>=0)
				                    {
					                    ValueBuff.intval.year_month.year  = 0;
					                    ValueBuff.intval.year_month.month = Year*12+Month;
				                    }
                            else
				                    {
					                    ValueBuff.intval.year_month.year  = 0;
					                    ValueBuff.intval.year_month.month = -(Year*12+Month);
					                    ValueBuff.interval_sign=SQL_TRUE;
				                    }				
			                    }
			                    break;
    default:              return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_YEAR_TO_MONTH(SQLPOINTER        DataPtr
	                                   ,int               /*DataLen*/
                                     ,int*              ResultLen
	                                   ,OracleItems&      Holder
	                                   ,ODBCErrorHolder&  ociError
                                     ,ODBCStatement*    p_statement
                                     ,bool              /*p_getData*/)
{
  sb4 Year;
  sb4 Month;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_YEAR_TO_MONTH;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALYM_TYPE:   OCIIntervalGetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,&Year
					                                          ,&Month
					                                          ,Holder.Value.interval_val);
				                    if(Year >= 0)
				                    {
					                    ValueBuff.intval.year_month.year  = Year;
					                    ValueBuff.intval.year_month.month = Month;
				                    }
                            else
				                    {
					                    ValueBuff.intval.year_month.year  = -Year;
					                    ValueBuff.intval.year_month.month = -Month;
					                    ValueBuff.interval_sign = SQL_TRUE;
				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_DAY(SQLPOINTER        DataPtr
	                         ,int               /*DataLen*/
                           ,int*              ResultLen
	                         ,OracleItems&      Holder
	                         ,ODBCErrorHolder&  ociError
                           ,ODBCStatement*    p_statement
                           ,bool              /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_DAY;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,&Day		
					                                          ,&Hour
					                                          ,&Minute
					                                          ,&Second
					                                          ,&fraction
					                                          ,Holder.Value.interval_val);
				                    if(Day >= 0)
				                    {
					                    ValueBuff.intval.day_second.day = Day;
				                    }
                            else
				                    {
					                    ValueBuff.intval.day_second.day = -Day;
					                    ValueBuff.interval_sign = SQL_TRUE;
				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_HOUR(SQLPOINTER       DataPtr
	                          ,int              /*DataLen*/
                            ,int*             ResultLen
	                          ,OracleItems&     Holder
	                          ,ODBCErrorHolder& ociError
                            ,ODBCStatement*   p_statement
                            ,bool             /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_HOUR;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,&Day		
					                                          ,&Hour
					                                          ,&Minute
					                                          ,&Second
					                                          ,&fraction
					                                          ,Holder.Value.interval_val);
				                    if(Day >= 0)
				                    {
					                    ValueBuff.intval.day_second.hour = Day*24+Hour;
				                    }
                            else
				                    {
					                    ValueBuff.intval.day_second.hour = -(Day*24+Hour);
					                    ValueBuff.interval_sign = SQL_TRUE;
				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_MINUTE(SQLPOINTER       DataPtr
	                            ,int              /*DataLen*/
                              ,int*             ResultLen
                              ,OracleItems&     Holder
	                            ,ODBCErrorHolder& ociError
                              ,ODBCStatement*   p_statement
                              ,bool             /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_MINUTE;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                         ,ociError.GetOwnerErrorObject()
					                                         ,&Day		
					                                         ,&Hour
					                                         ,&Minute
					                                         ,&Second
					                                         ,&fraction
					                                         ,Holder.Value.interval_val);
				                    if(Day>=0)
				                    {
					                    ValueBuff.intval.day_second.minute = Day*24*60+Hour*60+Minute;
				                    }
                            else
				                    {
					                    ValueBuff.intval.day_second.minute = -(Day*24*60+Hour*60+Minute);
					                    ValueBuff.interval_sign = SQL_TRUE;
 				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_SECOND(SQLPOINTER       DataPtr
	                            ,int              /*DataLen*/
                              ,int*             ResultLen
	                            ,OracleItems&     Holder
	                            ,ODBCErrorHolder& ociError
                              ,ODBCStatement*   p_statement
                              ,bool             /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_SECOND;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                           ,ociError.GetOwnerErrorObject()
				                                           ,&Day		
				                                           ,&Hour
				                                           ,&Minute
				                                           ,&Second
				                                           ,&fraction
				                                           ,Holder.Value.interval_val);
			                      if(Day>=0)
			                      {
				                      ValueBuff.intval.day_second.second = INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
			                      }
                            else
			                      {
				                      ValueBuff.intval.day_second.second = -INTVAL_TO_SECONDS(Day,Hour,Minute,Second);
				                      ValueBuff.interval_sign = SQL_TRUE;
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_DAY_TO_HOUR(SQLPOINTER        DataPtr
	                                 ,int               /*DataLen*/
                                   ,int*              ResultLen
	                                 ,OracleItems&      Holder
	                                 ,ODBCErrorHolder&  ociError
                                   ,ODBCStatement*    p_statement
                                   ,bool              /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_DAY_TO_HOUR;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                         ,ociError.GetOwnerErrorObject()
					                                         ,&Day		
					                                         ,&Hour
					                                         ,&Minute
					                                         ,&Second
					                                         ,&fraction
					                                         ,Holder.Value.interval_val);
				                    if(Day >= 0)
				                    {
					                    ValueBuff.intval.day_second.day  = Day;
					                    ValueBuff.intval.day_second.hour = Hour;
				                    }
                            else
				                    {
					                    ValueBuff.intval.day_second.day  = -Day;
					                    ValueBuff.intval.day_second.hour = -Hour;
					                    ValueBuff.interval_sign = SQL_TRUE;
				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_DAY_TO_MINUTE(SQLPOINTER        DataPtr
                                     ,int               /*DataLen*/
                                     ,int*              ResultLen
	                                   ,OracleItems&      Holder
	                                   ,ODBCErrorHolder&  ociError
                                     ,ODBCStatement*    p_statement
                                     ,bool              /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT &ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_DAY_TO_MINUTE;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                         ,ociError.GetOwnerErrorObject()
					                                         ,&Day		
					                                         ,&Hour
					                                         ,&Minute
					                                         ,&Second
					                                         ,&fraction
					                                         ,Holder.Value.interval_val);
				                    if(Day >= 0)
				                    {
					                    ValueBuff.intval.day_second.day    = Day;
					                    ValueBuff.intval.day_second.hour   = Hour;
					                    ValueBuff.intval.day_second.minute = Minute;
				                    }
                            else
				                    {
					                    ValueBuff.intval.day_second.day    = -Day;
					                    ValueBuff.intval.day_second.hour   = -Hour;
					                    ValueBuff.intval.day_second.minute = -Minute;
					                    ValueBuff.interval_sign = SQL_TRUE;
				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_DAY_TO_SECOND(SQLPOINTER        DataPtr
                                     ,int               /*DataLen*/
                                     ,int*              ResultLen
                                     ,OracleItems&      Holder
                                     ,ODBCErrorHolder&  ociError
                                     ,ODBCStatement*    p_statement
                                     ,bool              /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_DAY_TO_SECOND;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                         ,ociError.GetOwnerErrorObject()
					                                         ,&Day		
					                                         ,&Hour
					                                         ,&Minute
					                                         ,&Second
					                                         ,&fraction
					                                         ,Holder.Value.interval_val);
				                    if(Day >= 0)
				                    {
					                    ValueBuff.intval.day_second.day    = Day;
					                    ValueBuff.intval.day_second.hour   = Hour;
					                    ValueBuff.intval.day_second.minute = Minute;
					                    ValueBuff.intval.day_second.second = Second;
				                    }
                            else
				                    {
					                    ValueBuff.intval.day_second.day    = -Day;
					                    ValueBuff.intval.day_second.hour   = -Hour;
					                    ValueBuff.intval.day_second.minute = -Minute;
					                    ValueBuff.intval.day_second.second = -Second;
					                    ValueBuff.interval_sign = SQL_TRUE;
				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_HOUR_TO_MINUTE(SQLPOINTER       DataPtr
	                                    ,int              /*DataLen*/
                                      ,int*             ResultLen
	                                    ,OracleItems&     Holder
	                                    ,ODBCErrorHolder& ociError
                                      ,ODBCStatement*   p_statement
                                      ,bool             /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type = SQL_IS_HOUR_TO_MINUTE;
	ValueBuff.interval_sign = SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
		                                               ,ociError.GetOwnerErrorObject()
		                                               ,&Day		
		                                               ,&Hour
		                                               ,&Minute
		                                               ,&Second
		                                               ,&fraction
		                                               ,Holder.Value.interval_val);
				                    if(Day>=0)
				                    {
					                    ValueBuff.intval.day_second.hour   = Day*24+Hour;
					                    ValueBuff.intval.day_second.minute = Minute;
				                    }
                            else
				                    {
					                    ValueBuff.intval.day_second.hour   = -(Day*24+Hour);
					                    ValueBuff.intval.day_second.minute = -Minute;
					                    ValueBuff.interval_sign = SQL_TRUE;
				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_HOUR_TO_SECOND(SQLPOINTER       DataPtr
	                                    ,int              /*DataLen*/
                                      ,int*             ResultLen
	                                    ,OracleItems&     Holder
	                                    ,ODBCErrorHolder& ociError
                                      ,ODBCStatement*   p_statement
                                      ,bool             /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type = SQL_IS_HOUR_TO_SECOND;
	ValueBuff.interval_sign = SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE:   OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                         ,ociError.GetOwnerErrorObject()
					                                         ,&Day		
					                                         ,&Hour
					                                         ,&Minute
					                                         ,&Second
					                                         ,&fraction
					                                         ,Holder.Value.interval_val);
				                    if(Day>=0)
				                    {
					                    ValueBuff.intval.day_second.hour   = Day*24 + Hour;
					                    ValueBuff.intval.day_second.minute = Minute;
					                    ValueBuff.intval.day_second.second = Second;
				                    }
                            else
				                    {
					                    ValueBuff.intval.day_second.hour   = -(Day*24 + Hour);
					                    ValueBuff.intval.day_second.minute = -Minute;
					                    ValueBuff.intval.day_second.second = -Second;
					                    ValueBuff.interval_sign = SQL_TRUE;
				                    }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_INTERVAL_MINUTE_TO_SECOND(SQLPOINTER         DataPtr
	                                      ,int                /*DataLen*/
                                        ,int*               ResultLen
	                                      ,OracleItems&       Holder
	                                      ,ODBCErrorHolder&   ociError
                                        ,ODBCStatement*     p_statement
                                        ,bool               /*p_getData*/)
{
  sb4 Day;				
  sb4 Hour;
  sb4 Minute;
  sb4 Second;
  sb4 fraction;

  SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	memset(&ValueBuff,0,sizeof(ValueBuff));
	ValueBuff.interval_type=SQL_IS_MINUTE_TO_SECOND;
	ValueBuff.interval_sign=SQL_FALSE;
	switch(Holder.m_valueType)
	{
		case INTERVALDS_TYPE: OCIIntervalGetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                        ,ociError.GetOwnerErrorObject()
					                                        ,&Day		
					                                        ,&Hour
					                                        ,&Minute
					                                        ,&Second
					                                        ,&fraction
					                                        ,Holder.Value.interval_val);
				                  if(Day >= 0)
				                  {
					                  ValueBuff.intval.day_second.hour   = 0;
					                  ValueBuff.intval.day_second.minute = (Day * 24 * 60) + (Hour * 60) + Minute;
					                  ValueBuff.intval.day_second.second = Second;
				                  }
                          else
				                  {
					                  ValueBuff.intval.day_second.hour   = 0;
					                  ValueBuff.intval.day_second.minute = -(Day*24*60+Hour*60+Minute);
					                  ValueBuff.intval.day_second.second = -Second;
					                  ValueBuff.interval_sign = SQL_TRUE;
				                  }
			                    break;
    default:              return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_INTERVAL_STRUCT);
  }
  Holder.m_pendingLength = SQL_NULL_DATA;
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_BINARY(SQLPOINTER        DataPtr
                     ,int               DataLen
                     ,int*              ResultLen
                     ,OracleItems&      Holder
                     ,ODBCErrorHolder&  ociError
                     ,ODBCStatement*    p_statement
                     ,bool              p_getData)
{
  unsigned int len = 0;
  SQLRETURN result = SQL_ERROR;

  switch(Holder.m_valueType)
  {
    case LONGRAW_TYPE:    // Fall through
    case RAW_TYPE:        result = CopyODBCRAWToApp(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                   ,Holder.Value.raw_val
                                                   ,DataPtr
                                                   ,DataLen
                                                   ,ResultLen
                                                   ,&ociError
                                                   ,&Holder
                                                   ,p_statement->GetMaxLength()
                                                   ,p_getData);
                          break;
    case LONG_TYPE:       len = *((int*)Holder.Value.long_val);
                          result = CopyODBCStringToApp((SQLCHAR*)DataPtr
                                                      ,(char*)(Holder.Value.long_val + sizeof(int))
                                                      ,DataLen
                                                      ,ResultLen
                                                      ,&ociError
                                                      ,&Holder
                                                      ,p_statement->GetMaxLength()
                                                      ,p_getData);
                          break;
    case BLOB_TYPE:       result = CopyODBCLOBToApp(p_statement->GetDBConnection()->GetOCIConnection()
                                                   ,DataPtr,Holder.Value.lob_val
                                                   ,DataLen
                                                   ,ResultLen
                                                   ,ociError
                                                   ,&Holder
                                                   ,p_statement->GetMaxLength()
                                                   ,p_getData);
                          break;
    default:              return TypeConversionError(ociError,Holder);
  }
  return result;
}

SQLRETURN 
ConvertToSQL_C_NUMERIC(SQLPOINTER        DataPtr
                      ,int               DataLen
                      ,int*              ResultLen
                      ,OracleItems&      Holder
                      ,ODBCErrorHolder&  ociError
                      ,ODBCStatement*    p_statement
                      ,bool              /*p_getData*/)
{
  char LocalBuffer[LOCAL_BUFFERLEN];
  char* str = LocalBuffer;
  ub4   bufSize = LOCAL_BUFFERLEN;
  if(DataLen < sizeof(SQL_NUMERIC_STRUCT))
  {
    return TypeConversionError(ociError,Holder);
  }
  switch(Holder.m_valueType)
  {
    case INTEGER_TYPE:itoa(Holder.Value.int_val,LocalBuffer,10);
                      break;
    case NUMBER_TYPE: if(OCINumberToText(ociError.GetOwnerErrorObject()
                                        ,&Holder.Value.number_val           // Input the OCINumber
                                        ,(const oratext*) "TM9"             // TM = Text minimum
                                        ,3                                  // Format length
                                        ,(oratext*)NLS_NUMERIC_CHARACTERS   // NLS_PARAMS use default
                                        ,NLS_LENGTH                         // Length of NLS PARAMS
                                        ,&bufSize                           // Buffer size
                                        ,(oratext*)LocalBuffer) != OCI_SUCCESS)
                      {
                        ociError.GetErrorFromOCIHandle();
                        return SQL_ERROR;
                      }
                      break;
    case DOUBLE_TYPE: DoubleToString(LocalBuffer,Holder.Value.double_val);
                      break;
    case STRING_TYPE: str = (char*)	OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                ,Holder.Value.string_val);
                      break;
    default:          // For all other types
                      return TypeConversionError(ociError,Holder);
  }
  // Perform NUMERIC conversion on basis of "str"
  // Must set precision/scale beforehand
  SQL_NUMERIC_STRUCT* numeric = (SQL_NUMERIC_STRUCT*)DataPtr;
  numeric->precision = Holder.m_desc_precision;
  numeric->scale     = Holder.m_desc_scale;
  if((Holder.m_desc_precision == 0) && 
    ((Holder.m_desc_scale == 0) || (Holder.m_desc_scale == -127)))
  {
    // Take precision and scale from the string
    int   len = (int)strlen(str);
    char* pos = strchr(str,'.');
    numeric->precision = len;
    numeric->scale     = 0;
    if(pos)
    {
      numeric->scale = len - (pos - str) - 1;
      numeric->precision--;
    }
    if(numeric->scale > 32)
    {
      numeric->scale = 32;
      *(pos + numeric->scale + 1) = 0;
    }
  }
  int overflow  = 0;
  StringToNumeric(str,numeric,&overflow);
  if(overflow)
  {
    // Numeric value out of range
    memset(numeric,0,sizeof(SQL_NUMERIC_STRUCT));
    ociError.AddError("22003");
    return SQL_ERROR;
  }
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_NUMERIC_STRUCT);
  }
  return SQL_SUCCESS;
}

SQLRETURN 
ConvertToSQL_C_GUID(SQLPOINTER        DataPtr
                   ,int               DataLen
                   ,int*              ResultLen
                   ,OracleItems&      Holder
                   ,ODBCErrorHolder&  ociError
                   ,ODBCStatement*    p_statement
                   ,bool              /*p_getData*/)
{
  if(Holder.m_valueType == STRING_TYPE)
  {
    SQLGUID* guid = (SQLGUID*)DataPtr;
    if(DataLen >= 16)
    {
      char* string = (char*)OCIStringPtr(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                        ,Holder.Value.string_val);
      int scannum = sscanf(string
                          ,"%lX-%hX-%hX-%hhX%hhX-%hhX%hhX%hhX%hhX%hhX%hhX"
                          ,&guid->Data1 
                          ,&guid->Data2 
                          ,&guid->Data3 
                          ,&guid->Data4[0]
                          ,&guid->Data4[1]
                          ,&guid->Data4[2]
                          ,&guid->Data4[3]
                          ,&guid->Data4[4]
                          ,&guid->Data4[5]
                          ,&guid->Data4[6]
                          ,&guid->Data4[7]);
      if(scannum == 11)
      {
        if(ResultLen)
        {
          *ResultLen = 16;
        }
        return SQL_SUCCESS;
      }
    }
  }
  return TypeConversionError(ociError,Holder);
}
