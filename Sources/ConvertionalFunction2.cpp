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
// Contents:  INBOUND TRAFFIC FROM APPLICATION TO DRIVER TO DATASOURCE
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "DescriptorIRD.h"
#include "DescriptorARD.h"
#include "Statement.h"
#include "ODBCTypes.h"
#include "Environment.h"
#include "Utility.h"
#include <locale.h>

#pragma warning (disable: 4996)

#define LOCAL_BUFFERLEN 100

SQLRETURN ConvertFromSQL_C_CHAR                     (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_SSHORT                   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_USHORT                   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_SLONG	                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_ULONG	                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_FLOAT	                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_DOUBLE	                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_BIT                      (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_STINYINT                 (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_UTINYINT                 (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_SBIGINT                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_UBIGINT                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_TYPE_DATE                (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_TYPE_TIME                (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_TYPE_TIMESTAMP           (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_DAY             (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_DAY_TO_HOUR     (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_DAY_TO_MINUTE   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_YEAR            (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_MONTH           (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_YEAR_TO_MONTH   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_DAY_TO_SECOND   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_HOUR            (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_HOUR_TO_MINUTE  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_HOUR_TO_SECOND  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_MINUTE          (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_MINUTE_TO_SECOND(SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_INTERVAL_SECOND          (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_GUID                     (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_BINARY                   (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
SQLRETURN ConvertFromSQL_C_NUMERIC                  (SQLPOINTER DataPtr,int DataLen,int* ResultLen,OracleItems& Holder,ODBCErrorHolder& ociError,ODBCStatement* p_statement,bool p_getData);
	
// SQL_C_XXXX Type to conversion function translation
// The order of the functions impacts on the performance of the driver
// so the order is optimized for performance.

ConversionFunctions ConversionFunctionFromODBC_SQL[]=
{
  // Most used datatypes
	SQL_C_CHAR,                       ConvertFromSQL_C_CHAR,
  SQL_C_NUMERIC,                    ConvertFromSQL_C_NUMERIC,
  SQL_C_DOUBLE,                     ConvertFromSQL_C_DOUBLE,
  SQL_C_DATE,                       ConvertFromSQL_C_TYPE_DATE,
  SQL_C_TYPE_DATE,                  ConvertFromSQL_C_TYPE_DATE,
  SQL_C_TIME,                       ConvertFromSQL_C_TYPE_TIME,
  SQL_C_TYPE_TIME,                  ConvertFromSQL_C_TYPE_TIME,
  SQL_C_TIMESTAMP,                  ConvertFromSQL_C_TYPE_TIMESTAMP,
  SQL_C_TYPE_TIMESTAMP,             ConvertFromSQL_C_TYPE_TIMESTAMP,
  SQL_C_BINARY,                     ConvertFromSQL_C_BINARY,
  // Less used datatypes
  SQL_C_SSHORT,                     ConvertFromSQL_C_SSHORT,
	SQL_C_SHORT,                      ConvertFromSQL_C_SSHORT,
	SQL_C_USHORT,                     ConvertFromSQL_C_USHORT,
	SQL_C_SLONG,                      ConvertFromSQL_C_SLONG,
	SQL_C_LONG,                       ConvertFromSQL_C_SLONG,
	SQL_C_ULONG,                      ConvertFromSQL_C_ULONG,
	SQL_C_FLOAT,                      ConvertFromSQL_C_FLOAT,
	SQL_C_BIT,                        ConvertFromSQL_C_BIT,
	SQL_C_STINYINT,                   ConvertFromSQL_C_STINYINT,
	SQL_C_TINYINT,                    ConvertFromSQL_C_STINYINT,
	SQL_C_UTINYINT,                   ConvertFromSQL_C_UTINYINT,
	SQL_C_SBIGINT,                    ConvertFromSQL_C_SBIGINT,
	SQL_C_UBIGINT,                    ConvertFromSQL_C_UBIGINT,
	SQL_C_INTERVAL_DAY,               ConvertFromSQL_C_INTERVAL_DAY,
	SQL_C_INTERVAL_DAY_TO_HOUR,       ConvertFromSQL_C_INTERVAL_DAY_TO_HOUR,
	SQL_C_INTERVAL_DAY_TO_MINUTE,     ConvertFromSQL_C_INTERVAL_DAY_TO_MINUTE,
	SQL_C_INTERVAL_DAY_TO_SECOND,     ConvertFromSQL_C_INTERVAL_DAY_TO_SECOND,
	SQL_C_INTERVAL_HOUR,              ConvertFromSQL_C_INTERVAL_HOUR,
	SQL_C_INTERVAL_HOUR_TO_MINUTE,    ConvertFromSQL_C_INTERVAL_HOUR_TO_MINUTE,
	SQL_C_INTERVAL_HOUR_TO_SECOND,    ConvertFromSQL_C_INTERVAL_HOUR_TO_SECOND,
	SQL_C_INTERVAL_MINUTE,            ConvertFromSQL_C_INTERVAL_MINUTE,
	SQL_C_INTERVAL_MINUTE_TO_SECOND,  ConvertFromSQL_C_INTERVAL_MINUTE_TO_SECOND,
	SQL_C_INTERVAL_SECOND,            ConvertFromSQL_C_INTERVAL_SECOND,
	SQL_C_INTERVAL_YEAR,              ConvertFromSQL_C_INTERVAL_YEAR,
	SQL_C_INTERVAL_MONTH,             ConvertFromSQL_C_INTERVAL_MONTH,
	SQL_C_INTERVAL_YEAR_TO_MONTH,     ConvertFromSQL_C_INTERVAL_YEAR_TO_MONTH,
  SQL_C_GUID,                       ConvertFromSQL_C_GUID
};

SQLRETURN 
DriverToDatasource(SQLSMALLINT       ODBC_Type
                  ,SQLPOINTER        DataPtr
                  ,SQLLEN            DataLen
                  ,SQLLEN*           StrLen_or_IndPtr
                  ,OracleItems&      Holder
                  ,ODBCErrorHolder&  ociError
                  ,ODBCStatement*    p_statement
                  ,bool              p_getData)
{
	if(StrLen_or_IndPtr)
	{
		if(*StrLen_or_IndPtr == SQL_NULL_DATA)
		{
			Holder.m_indicator = -1;
			return SQL_SUCCESS;
		}
		else
    {
      Holder.m_indicator = (sb2)*StrLen_or_IndPtr;
    }			
	}
	if(!DataPtr)
	{
		Holder.m_indicator = SQL_NULL_DATA;
		return SQL_SUCCESS;
	}
	for(short i=0;i<sizeof(ConversionFunctionFromODBC_SQL)/sizeof(ConversionFunctionFromODBC_SQL[0]);++i)
	{
		if(ConversionFunctionFromODBC_SQL[i].ODBC_Type == ODBC_Type)
    {
			return ConversionFunctionFromODBC_SQL[i].ConversionFunction(DataPtr
                                                                 ,(int) DataLen
                                                                 ,(int*)StrLen_or_IndPtr
                                                                 ,Holder
                                                                 ,ociError
                                                                 ,p_statement
                                                                 ,p_getData);
    }
	}
  return TypeConversionError(ociError,Holder);
}

char NullStr[]="";


SQLRETURN 
ConvertFromSQL_C_CHAR(SQLPOINTER       DataPtr
                     ,int              DataLen
                     ,int*             ResultLen
                     ,OracleItems&     Holder
                     ,ODBCErrorHolder& ociError
                     ,ODBCStatement*   p_statement
                     ,bool             /*p_putData*/)
{
	char *TextBuff;
	TextBuff=(char*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:                Holder.Value.int_val = 0;
			                                if(sscanf(TextBuff,"%d",&Holder.Value.int_val)!=1)
			                                {
				                                ociError.Clear();
				                                ociError.AddError("HY000",0,"To integer conversion error");
				                                return SQL_ERROR;
			                                }
			                                break;
		case DOUBLE_TYPE:                 {
				                                double value = 0.0;
                                        if(StringToDouble(TextBuff,&value) == false)
				                                {
					                                ociError.AddError("22003");
					                                return SQL_ERROR;
				                                }
				                                Holder.Value.double_val = value;
			                                }
			                                break;
		case DATE_TYPE:                   return StringToDate(p_statement->GetDBConnection()
                                                         ,(char*)TextBuff
                                                         ,&Holder.Value.date_val
                                                         ,&ociError);
    case NUMBER_TYPE:                 {
                                        char pattern[SQLNUM_MAXLEN+1];
                                        MakeNumericPattern(TextBuff,pattern);
                                        if(OCINumberFromText(ociError.GetOwnerErrorObject()
                                                            ,(const oratext*)TextBuff
                                                            ,(ub4)    strlen(TextBuff)
                                                            ,(const oratext*)pattern
                                                            ,(ub4)    strlen(pattern)
                                                            ,(const oratext*)NLS_NUMERIC_CHARACTERS
                                                            ,                NLS_LENGTH
                                                            ,&Holder.Value.number_val) != OCI_SUCCESS)
                                        {
                                          ociError.GetErrorFromOCIHandle();
                                          return SQL_ERROR;
                                        }
                                      }
			                                break;
    case STRING_TYPE:                 if(*p_statement->m_SQLDriverToDataSource)
                                      {
                                        // TRANSLATION LIB is loaded. So try it.
                                        SWORD  errorLen = 0;
                                        UCHAR  error[257] = "";
                                        UDWORD options = *(p_statement->m_translateOptions);
                                        SWORD  type = SQL_VARCHAR;
                                        SQLRETURN ret = (SQLRETURN)
                                          (**p_statement->m_SQLDriverToDataSource)(options
                                                                                  ,type
                                                                                  ,TextBuff
                                                                                  ,(int)strlen(TextBuff)
                                                                                  ,TextBuff
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
                                      {
                                        OCIEnv* environment = p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv();
                                        int length = (int)strlen(TextBuff) + 1;
                                        return CopyODBCStringToRDBMS(environment
                                                                    ,TextBuff
                                                                    ,length
                                                                    ,ResultLen
                                                                    ,&ociError
                                                                    ,Holder
                                                                    ,p_statement->GetMaxLength());
                                      }
			                                break;
    case CLOB_TYPE:                   return CopyODBCLOBToRDBMS(p_statement->GetDBConnection()->GetOCIConnection()
                                                               ,Holder.Value.lob_val
                                                               ,TextBuff
                                                               ,DataLen
                                                               ,ResultLen
                                                               ,ociError
                                                               ,&Holder);
		case DATETIME_TYPE:               // Fall through
    case ANSI_DATE_TYPE:              return StringToTimestamp(p_statement->GetDBConnection()
                                                              ,(char*)TextBuff
                                                              ,false
                                                              ,Holder.Value.datetime_val
                                                              ,&ociError);
		case DATETIMEWTZ_TYPE:            // Fall through
		case DATETIMEWLTZ_TYPE:           return StringToTimestamp(p_statement->GetDBConnection()
                                                              ,(char*)TextBuff
                                                              ,true
                                                              ,Holder.Value.datetime_val
                                                              ,&ociError);
		case INTERVALYM_TYPE:             // Fall through
    case INTERVALDS_TYPE:             if(OCIIntervalFromText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                            ,ociError.GetOwnerErrorObject()
                                                            ,(oratext*)TextBuff
                                                            ,strlen(TextBuff)
                                                            ,Holder.Value.interval_val) != OCI_SUCCESS)
		                                  {
			                                  ociError.Clear();
			                                  ociError.AddError("HY000",0,"To INTERVAL conversion error");
			                                  return SQL_ERROR;
		                                  }
		                                  break;
    default:                          return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_SSHORT(SQLPOINTER       DataPtr
	                     ,int              /*DataLen*/
                       ,int*             ResultLen
	                     ,OracleItems&     Holder
	                     ,ODBCErrorHolder& ociError
                       ,ODBCStatement*   p_statement
                       ,bool             /*p_getData*/)
{
	SQLSMALLINT &ValueBuff=*(SQLSMALLINT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			  Holder.Value.int_val = ValueBuff;
			                        break;
		case DOUBLE_TYPE:   			Holder.Value.double_val=ValueBuff;
			                        break;
		case NUMBER_TYPE:   			if(OCINumberFromInt(ociError.GetOwnerErrorObject()
					                                       ,&ValueBuff
					                                       ,sizeof(ValueBuff)
					                                       ,OCI_NUMBER_SIGNED
					                                       ,&Holder.Value.number_val) != OCI_SUCCESS)
                              {
                                ociError.GetErrorFromOCIHandle();
                                return SQL_ERROR;
	                            }
	                            break;
		case STRING_TYPE:         {
				                        char Str[41];
				                        sprintf_s(Str,40,"%d",ValueBuff);
				                        OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                       ,ociError.GetOwnerErrorObject()
						                                       ,(OraText*)Str
						                                       ,(ub4)strlen(Str)
						                                       ,&Holder.Value.string_val);	
			                        }
			                        break;
		case INTERVALYM_TYPE:     {
				                        sb4 Year;
				                        sb4 Month;
				                        Year  = ValueBuff / 12;
				                        Month = ValueBuff % 12;
				                        OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                             ,ociError.GetOwnerErrorObject()
					                                             ,Year
					                                             ,Month
					                                             ,Holder.Value.interval_val);				
			                        }
			                        break;
		case INTERVALDS_TYPE:     {
				                        sb4 Day;				
				                        sb4 Hour;
				                        sb4 Minute;
				                        sb4 Second;
				                        sb4 fraction = 0;
				                        Day    = ValueBuff/(24*60*60);
				                        Hour   = (ValueBuff-Day*(24*60*60))/(60*60);
				                        Minute = (ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
				                        Second = (ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
				                        OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                             ,ociError.GetOwnerErrorObject()
					                                             ,Day		
					                                             ,Hour
					                                             ,Minute
					                                             ,Second
					                                             ,fraction
					                                             ,Holder.Value.interval_val);				
			                        }
			                        break;
    default:                  return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLSMALLINT);
  }
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_USHORT(SQLPOINTER        DataPtr
	                      ,int              /*DataLen*/
                        ,int*             ResultLen
	                      ,OracleItems&     Holder
	                      ,ODBCErrorHolder& ociError
                        ,ODBCStatement*   p_statement
                        ,bool             /*p_getData*/)
{
	SQLUSMALLINT &ValueBuff=*(SQLUSMALLINT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:  			Holder.Value.int_val = ValueBuff;
			                        break;
		case DOUBLE_TYPE: 			  Holder.Value.double_val = ValueBuff;
			                        break;
		case NUMBER_TYPE:			  	if(OCINumberFromInt(ociError.GetOwnerErrorObject()
					                                       ,&ValueBuff
					                                       ,sizeof(ValueBuff)
					                                       ,OCI_NUMBER_UNSIGNED
					                                       ,&Holder.Value.number_val) != OCI_SUCCESS)
                              {
                                ociError.GetErrorFromOCIHandle();
                                return SQL_ERROR;
                              }
			                        break;
		case STRING_TYPE:         {
				                        char Str[40];
				                        sprintf(Str,"%u",ValueBuff);
				                        OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                        ,ociError.GetOwnerErrorObject()
						                                        ,(OraText*)Str
						                                        ,(ub4)strlen(Str)
						                                        ,&Holder.Value.string_val);
			                        }
			                        break;
		case INTERVALYM_TYPE:     {
				                        sb4 Year;
				                        sb4 Month;
				                        Year=ValueBuff/12;
				                        Month=ValueBuff-Year*12;
				                        OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                              ,ociError.GetOwnerErrorObject()
					                                              ,Year
					                                              ,Month
					                                              ,Holder.Value.interval_val);				
			                        }
			                        break;
		case INTERVALDS_TYPE:     {
				                        sb4 Day;				
				                        sb4 Hour;
				                        sb4 Minute;
				                        sb4 Second;
				                        sb4 fraction = 0;
				                        Day    =  ValueBuff/(24*60*60);
				                        Hour   = (ValueBuff-Day*(24*60*60))/(60*60);
				                        Minute = (ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
				                        Second = (ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
				                        OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                             ,ociError.GetOwnerErrorObject()
					                                             ,Day		
					                                             ,Hour
					                                             ,Minute
					                                             ,Second
					                                             ,fraction
					                                             ,Holder.Value.interval_val);				
			                        }
			                        break;
    default:                  return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLSMALLINT);
  }
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_SLONG(SQLPOINTER       DataPtr
                      ,int              /*DataLen*/
                      ,int*             ResultLen
                      ,OracleItems&     Holder
                      ,ODBCErrorHolder& ociError
                      ,ODBCStatement*   p_statement
                      ,bool             /*p_getData*/)
{
	SQLINTEGER &ValueBuff=*(SQLINTEGER*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			Holder.Value.int_val=ValueBuff;
			                      break;
		case DOUBLE_TYPE: 			Holder.Value.double_val=ValueBuff;
			                      break;
		case NUMBER_TYPE: 			if(OCINumberFromInt(ociError.GetOwnerErrorObject()
					                                      ,&ValueBuff
					                                      ,sizeof(ValueBuff)
					                                      ,OCI_NUMBER_SIGNED
					                                      ,&Holder.Value.number_val) != OCI_SUCCESS)
                            {
                              ociError.GetErrorFromOCIHandle();
                              return SQL_ERROR;
                      			}
			                      break;
		case STRING_TYPE:       {
				                      char Str[40];
				                      sprintf(Str,"%d",ValueBuff);
				                      OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                         ,ociError.GetOwnerErrorObject()
				                                         ,(OraText*)Str
				                                         ,(ub4)strlen(Str)
				                                         ,&Holder.Value.string_val);
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
				                      sb4 Year;
				                      sb4 Month;
				                      Year  = ValueBuff / 12;
				                      Month = ValueBuff % 12;
				                      OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,Year
					                                            ,Month
					                                            ,Holder.Value.interval_val);				
			                      }
			                      break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction = 0;
				                      Day    =  ValueBuff/(24*60*60);
				                      Hour   = (ValueBuff-Day*(24*60*60))/(60*60);
				                      Minute = (ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
				                      Second = (ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
				                      OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,Day		
					                                            ,Hour
					                                            ,Minute
					                                            ,Second
					                                            ,fraction
					                                            ,Holder.Value.interval_val);				
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLINTEGER);
  }
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_ULONG(SQLPOINTER       DataPtr
	                    ,int              /*DataLen*/
                      ,int*             ResultLen
	                    ,OracleItems&     Holder
	                    ,ODBCErrorHolder& ociError
                      ,ODBCStatement*   p_statement
                      ,bool             /*p_getData*/)
{
	SQLUINTEGER &ValueBuff=*(SQLUINTEGER*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			Holder.Value.int_val = ValueBuff;
			                      break;
		case DOUBLE_TYPE:			  Holder.Value.double_val = ValueBuff;
			                      break;
		case NUMBER_TYPE:			  if(OCINumberFromInt(ociError.GetOwnerErrorObject()
					                                     ,&ValueBuff
					                                     ,sizeof(ValueBuff)
					                                     ,OCI_NUMBER_UNSIGNED
					                                     ,&Holder.Value.number_val) != OCI_SUCCESS)
                            {
                              ociError.GetErrorFromOCIHandle();
                              return SQL_ERROR;
	                          }
	                          break;		
		case STRING_TYPE:       {
				                      char Str[40];
				                      sprintf(Str,"%u",ValueBuff);
				                      OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                      ,ociError.GetOwnerErrorObject()
						                                      ,(OraText*)Str
						                                      ,(ub4)strlen(Str)
						                                      ,&Holder.Value.string_val);
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
				                      sb4 Year;
				                      sb4 Month;
				                      Year  = ValueBuff / 12;
				                      Month = ValueBuff % 12;
				                      OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,Year
					                                            ,Month
					                                            ,Holder.Value.interval_val);				
			                      }
			                      break;
		case INTERVALDS_TYPE:   {
			                        sb4 Day;				
			                        sb4 Hour;
			                        sb4 Minute;
			                        sb4 Second;
			                        sb4 fraction = 0;
			                        Day    = ValueBuff/(24*60*60);
			                        Hour   = (ValueBuff-Day*(24*60*60))/(60*60);
			                        Minute = (ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
			                        Second = (ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
			                        OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,Day		
				                                              ,Hour
				                                              ,Minute
				                                              ,Second
				                                              ,fraction
				                                              ,Holder.Value.interval_val);				
		                        }
		                        break;
    default:                return TypeConversionError(ociError,Holder);
	}
  if(ResultLen)
  {
    *ResultLen = sizeof(SQLUINTEGER);
  }
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_FLOAT(SQLPOINTER       DataPtr
	                    ,int              /*DataLen*/
                      ,int*             /*ResultLen*/
	                    ,OracleItems&     Holder
	                    ,ODBCErrorHolder& ociError
                      ,ODBCStatement*   p_statement
                      ,bool             /*p_putData*/)
{
	SQLREAL &ValueBuff=*(SQLREAL*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			Holder.Value.int_val = (int)ValueBuff;
			                      break;
		case DOUBLE_TYPE:			  Holder.Value.double_val=ValueBuff;
			                      break;
		case NUMBER_TYPE:			  if(OCINumberFromReal(ociError.GetOwnerErrorObject()
					                                      ,&ValueBuff
					                                      ,sizeof(ValueBuff)
					                                      ,&Holder.Value.number_val) != OCI_SUCCESS)
                            {
                              ociError.GetErrorFromOCIHandle();
                              return SQL_ERROR;
		                        }
		                        break;
		case STRING_TYPE:       {
				                      char Str[40];
                              FloatToString(Str,ValueBuff);
				                      OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                      ,ociError.GetOwnerErrorObject()
						                                      ,(OraText*)Str
						                                      ,(ub4)strlen(Str)
						                                      ,&Holder.Value.string_val);
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
				                      sb4 Year;
				                      sb4 Month;
				                      Year  = (sb4) ValueBuff / 12;
				                      Month = (sb4) ValueBuff % 12;
				                      OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,Year
					                                            ,Month
					                                            ,Holder.Value.interval_val);				
			                      }
			                      break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day;				
				                      sb4 Hour;
				                      sb4 Minute;
				                      sb4 Second;
				                      sb4 fraction = 0;
				                      Day   = (sb4) ValueBuff/(24*60*60);
				                      Hour  = (sb4)(ValueBuff-Day*(24*60*60))/(60*60);
				                      Minute= (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
				                      Second= (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
				                      OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,Day		
					                                            ,Hour
					                                            ,Minute
					                                            ,Second
					                                            ,fraction
					                                            ,Holder.Value.interval_val);				
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_DOUBLE(SQLPOINTER        DataPtr
	                     ,int               /*DataLen*/
                       ,int*              /*ResultLen*/
	                     ,OracleItems&      Holder
	                     ,ODBCErrorHolder&  ociError
                       ,ODBCStatement*    p_statement
                       ,bool              /*p_puData*/)
{
	SQLDOUBLE& ValueBuff = *(SQLDOUBLE*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			Holder.Value.int_val = (int)ValueBuff;
			                      break;
		case DOUBLE_TYPE:			  Holder.Value.double_val = ValueBuff;
			                      break;
		case NUMBER_TYPE:			  if(OCINumberFromReal(ociError.GetOwnerErrorObject()
					                                      ,&ValueBuff
					                                      ,sizeof(ValueBuff)
					                                      ,&Holder.Value.number_val) != OCI_SUCCESS)
                            {
                              ociError.GetErrorFromOCIHandle();
                              return SQL_ERROR;
		                        }
		                        break;
		case STRING_TYPE:       {
				                      char Str[40];
                              DoubleToString(Str,ValueBuff);
				                      OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                      ,ociError.GetOwnerErrorObject()
						                                      ,(OraText*)Str
						                                      ,(ub4)strlen(Str)
						                                      ,&Holder.Value.string_val);
			                      }
			                      break;
		case INTERVALYM_TYPE:   {
				                      sb4 Year;
				                      sb4 Month;
				                      Year  = (sb4)ValueBuff / 12;
				                      Month = (sb4)ValueBuff % 12;
				                      OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                              ,ociError.GetOwnerErrorObject()
				                                              ,Year
				                                              ,Month
				                                              ,Holder.Value.interval_val);				
			                      }
			                      break;
		case INTERVALDS_TYPE:   {
				                      sb4 Day      = 0;				
				                      sb4 Hour     = 0;
				                      sb4 Minute   = 0;
				                      sb4 Second   = 0;
				                      sb4 fraction = 0;
				                      Day    = (sb4)ValueBuff/(24*60*60);
				                      Hour   = (sb4)(ValueBuff-Day*(24*60*60))/(60*60);
				                      Minute = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
				                      Second = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
				                      OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                            ,ociError.GetOwnerErrorObject()
					                                            ,Day		
					                                            ,Hour
					                                            ,Minute
					                                            ,Second
					                                            ,fraction
					                                            ,Holder.Value.interval_val);				
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_BIT(SQLPOINTER       DataPtr
	                  ,int              /*DataLen*/
                    ,int*             /*ResultLen*/
	                  ,OracleItems&     Holder
	                  ,ODBCErrorHolder& ociError
                    ,ODBCStatement*   p_statement
                    ,bool             /*p_putData*/)
{
	SQLCHAR &ValueBuff=*(SQLCHAR*)DataPtr;
	switch(Holder.m_valueType)
	{
    case INTEGER_TYPE:			Holder.Value.int_val = (ValueBuff == 0) ? 0 : 1;
			                      break;
    case DOUBLE_TYPE:       Holder.Value.double_val = (ValueBuff == 0) ? 0.0 : 1.0;
			                      break;
		case NUMBER_TYPE:       {
                              int Val = (ValueBuff == 0) ? 0 : 1;
				                      OCINumberFromInt(ociError.GetOwnerErrorObject()
				                                      ,&Val
				                                      ,sizeof(Val)
				                                      ,OCI_NUMBER_UNSIGNED
				                                      ,&Holder.Value.number_val);			
			                      }
			                      break;
		case STRING_TYPE:       {
				                      char Str[40];				
				                      sprintf(Str,"%d",ValueBuff);
				                      OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                      ,ociError.GetOwnerErrorObject()
						                                      ,(OraText*)Str
						                                      ,(ub4)strlen(Str)
						                                      ,&Holder.Value.string_val);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_STINYINT(SQLPOINTER        DataPtr
                         ,int               /*DataLen*/
                         ,int*              /*ResultLen*/
                         ,OracleItems&      Holder
                         ,ODBCErrorHolder&  ociError
                         ,ODBCStatement*    p_statement
                         ,bool              /*p_putData*/)
{
	SQLSCHAR& ValueBuff = *(SQLSCHAR*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			Holder.Value.int_val = (char) ValueBuff;
			                      break;
		case DOUBLE_TYPE: 			Holder.Value.double_val = (char) ValueBuff;
			                      break;
		case NUMBER_TYPE:       {
				                      sb1 Val = ValueBuff;
				                      if(OCINumberFromInt(ociError.GetOwnerErrorObject()
					                                      ,&Val
					                                      ,sizeof(Val)
					                                      ,OCI_NUMBER_SIGNED
					                                      ,&Holder.Value.number_val) != OCI_SUCCESS)
                              {
                                ociError.GetErrorFromOCIHandle();
                                return SQL_ERROR;
  			                      }
                            }
	  		                    break;
		case STRING_TYPE:       {
			                        char Str[40];				
			                        sprintf(Str,"%d",ValueBuff);
			                        OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                        ,ociError.GetOwnerErrorObject()
					                                        ,(OraText*)Str
					                                        ,(ub4)strlen(Str)
					                                        ,&Holder.Value.string_val);
	                          }
			                      break;
		case INTERVALYM_TYPE:   {
                              sb4 Year;
                              sb4 Month;
                              Year  = (sb4)ValueBuff / 12;
                              Month = (sb4)ValueBuff % 12;
                              OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                     ,ociError.GetOwnerErrorObject()
                                                     ,Year
                                                     ,Month
                                                     ,Holder.Value.interval_val);				
                            }
                            break;
    case INTERVALDS_TYPE:   {
                                sb4 Day      = 0;				
                                sb4 Hour     = 0;
                                sb4 Minute   = 0;
                                sb4 Second   = 0;
                                sb4 fraction = 0;
                                Day    = (sb4) ValueBuff/(24*60*60);
                                Hour   = (sb4)(ValueBuff-Day*(24*60*60))/(60*60);
                                Minute = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
                                Second = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
                                OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,ociError.GetOwnerErrorObject()
                                                        ,Day		
                                                        ,Hour
                                                        ,Minute
                                                        ,Second
                                                        ,fraction
                                                        ,Holder.Value.interval_val);				
                              }
                              break;
    default:                  return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_UTINYINT( SQLPOINTER       DataPtr
	                        ,int              /*DataLen*/
                          ,int*             /*ResultLen*/
	                        ,OracleItems&     Holder
	                        ,ODBCErrorHolder& ociError
                          ,ODBCStatement*   p_statement
                          ,bool             /*p_putData*/)
{
	UCHAR& ValueBuff = *(UCHAR*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			Holder.Value.int_val = ValueBuff;
			                      break;
		case DOUBLE_TYPE: 			Holder.Value.double_val = ValueBuff;
			                      break;
		case NUMBER_TYPE:       {
				                      int Val=ValueBuff;
				                      if(OCINumberFromInt(ociError.GetOwnerErrorObject()
					                                       ,&Val
					                                       ,sizeof(Val)
					                                       ,OCI_NUMBER_UNSIGNED
					                                       ,&Holder.Value.number_val) != OCI_SUCCESS)
                              {
                                ociError.GetErrorFromOCIHandle();
                                return SQL_ERROR;
			                        }
                            }
			                      break;
		case STRING_TYPE:       {
			                        char Str[40];				
			                        sprintf(Str,"%u",ValueBuff);
			                        OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                        ,ociError.GetOwnerErrorObject()
					                                        ,(OraText*)Str
					                                        ,(ub4)strlen(Str)
					                                        ,&Holder.Value.string_val);
		                        }
	                          break;
    case INTERVALYM_TYPE:   {
                              sb4 Year;
                              sb4 Month;
                              Year  = (sb4)ValueBuff / 12;
                              Month = (sb4)ValueBuff % 12;
                              OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                      ,ociError.GetOwnerErrorObject()
                                                      ,Year
                                                      ,Month
                                                      ,Holder.Value.interval_val);				
                            }
                            break;
    case INTERVALDS_TYPE:   {
                              sb4 Day      = 0;				
                              sb4 Hour     = 0;
                              sb4 Minute   = 0;
                              sb4 Second   = 0;
                              sb4 fraction = 0;
                              Day    = (sb4) ValueBuff/(24*60*60);
                              Hour   = (sb4)(ValueBuff-Day*(24*60*60))/(60*60);
                              Minute = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
                              Second = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
                              OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                      ,ociError.GetOwnerErrorObject()
                                                      ,Day		
                                                      ,Hour
                                                      ,Minute
                                                      ,Second
                                                      ,fraction
                                                      ,Holder.Value.interval_val);				
                            }
                            break;
    default:                return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_SBIGINT(SQLPOINTER       DataPtr
	                      ,int              /*DataLen*/
                        ,int*             /*ResultLen*/
	                      ,OracleItems&     Holder
	                      ,ODBCErrorHolder& ociError
                        ,ODBCStatement*   p_statement
                        ,bool             /*p_putData*/)
{
	SQLBIGINT& ValueBuff = *(SQLBIGINT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			Holder.Value.int_val = (int)ValueBuff;
			                      break;
		case DOUBLE_TYPE: 			Holder.Value.double_val = (double)ValueBuff;
			                      break;
		case NUMBER_TYPE:       {
				                      SQLBIGINT Val = ValueBuff;
				                      if(OCINumberFromInt(ociError.GetOwnerErrorObject()
					                                       ,&Val
					                                       ,sizeof(Val)
					                                       ,OCI_NUMBER_SIGNED
					                                       ,&Holder.Value.number_val) != OCI_SUCCESS)
                              {
                                ociError.GetErrorFromOCIHandle();
                                return SQL_ERROR;
  			                      }
                            }
   		                      break;
		case STRING_TYPE:       {
			                        char Str[40];				
			                        sprintf(Str,"%I64d",ValueBuff);
			                        OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                        ,ociError.GetOwnerErrorObject()
					                                        ,(OraText*)Str
					                                        ,(ub4) strlen(Str)
					                                        ,&Holder.Value.string_val);
		                        }
		                        break;
    case INTERVALYM_TYPE:   {
                              sb4 Year;
                              sb4 Month;
                              Year  = (sb4)ValueBuff / 12;
                              Month = (sb4)ValueBuff % 12;
                              OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                      ,ociError.GetOwnerErrorObject()
                                                      ,Year
                                                      ,Month
                                                      ,Holder.Value.interval_val);				
                            }
                            break;
    case INTERVALDS_TYPE:   {
                              sb4 Day      = 0;				
                              sb4 Hour     = 0;
                              sb4 Minute   = 0;
                              sb4 Second   = 0;
                              sb4 fraction = 0;
                              Day    = (sb4) ValueBuff/(24*60*60);
                              Hour   = (sb4)(ValueBuff-Day*(24*60*60))/(60*60);
                              Minute = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
                              Second = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
                              OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                      ,ociError.GetOwnerErrorObject()
                                                      ,Day		
                                                      ,Hour
                                                      ,Minute
                                                      ,Second
                                                      ,fraction
                                                      ,Holder.Value.interval_val);				
                            }
                            break;
    default:                return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_UBIGINT(SQLPOINTER       DataPtr
	                      ,int              /*DataLen*/
                        ,int*             /*ResultLen*/
	                      ,OracleItems&     Holder
	                      ,ODBCErrorHolder& ociError
                        ,ODBCStatement*   p_statement
                        ,bool             /*p_putData*/)
{
	SQLUBIGINT& ValueBuff = *(SQLUBIGINT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:			Holder.Value.int_val = (int)ValueBuff;
			                      break;
		case DOUBLE_TYPE:			  Holder.Value.double_val = (double)ValueBuff;
			                      break;
		case NUMBER_TYPE:       {
				                      SQLUBIGINT Val = ValueBuff;
				                      if(OCINumberFromInt(ociError.GetOwnerErrorObject()
				                                         ,&Val
				                                         ,sizeof(Val)
				                                         ,OCI_NUMBER_UNSIGNED
				                                         ,&Holder.Value.number_val) != OCI_SUCCESS)
                              {
                                ociError.GetErrorFromOCIHandle();
                                return SQL_ERROR;
  			                      }
                            }
	  		                    break;
  	case STRING_TYPE:       {
		                          char Str[40];				
		                          sprintf(Str,"%I64u",ValueBuff);
		                          OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                          ,ociError.GetOwnerErrorObject()
				                                          ,(OraText*)Str
				                                          ,(ub4)strlen(Str)
				                                          ,&Holder.Value.string_val);
	                          }
	                          break;
    case INTERVALYM_TYPE:   {
                              sb4 Year;
                              sb4 Month;
                              Year  = (sb4)ValueBuff / 12;
                              Month = (sb4)ValueBuff % 12;
                              OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                      ,ociError.GetOwnerErrorObject()
                                                      ,Year
                                                      ,Month
                                                      ,Holder.Value.interval_val);				
                            }
                            break;
    case INTERVALDS_TYPE:   {
                              sb4 Day      = 0;				
                              sb4 Hour     = 0;
                              sb4 Minute   = 0;
                              sb4 Second   = 0;
                              sb4 fraction = 0;
                              Day    = (sb4) ValueBuff/(24*60*60);
                              Hour   = (sb4)(ValueBuff-Day*(24*60*60))/(60*60);
                              Minute = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60))/60;
                              Second = (sb4)(ValueBuff-Day*(24*60*60)-Hour*(60*60)-Minute*60);
                              OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                      ,ociError.GetOwnerErrorObject()
                                                      ,Day		
                                                      ,Hour
                                                      ,Minute
                                                      ,Second
                                                      ,fraction
                                                      ,Holder.Value.interval_val);				
                            }
                            break;
    default:                return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_TYPE_DATE(SQLPOINTER       DataPtr
	                        ,int              /*DataLen*/
                          ,int*             /*ResultLen*/
	                        ,OracleItems&     Holder
	                        ,ODBCErrorHolder& ociError
                          ,ODBCStatement*   p_statement
                          ,bool             /*p_putData*/)
{
	SQL_DATE_STRUCT &ValueBuff=*(SQL_DATE_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case DATE_TYPE:         {
				                      sb2 Year  = ValueBuff.year;
				                      ub1 Month = (ub1)ValueBuff.month;
				                      ub1 Day   = (ub1)ValueBuff.day;
                      				
				                      OCIDateSetDate(&Holder.Value.date_val,Year,Month,Day);				
				                      OCIDateSetTime(&Holder.Value.date_val,0,0,0);
			                      }			
			                      break;
		case DATETIME_TYPE:
		case DATETIMEWTZ_TYPE:
		case DATETIMEWLTZ_TYPE:
		case ANSI_DATE_TYPE:
			                      {				
				                      sb2 Year  =ValueBuff.year;
				                      ub1 Month = (ub1)ValueBuff.month;
				                      ub1 Day   = (ub1)ValueBuff.day;
                      				
				                      OCIDateTimeConstruct(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                      ,ociError.GetOwnerErrorObject()
						                                      ,Holder.Value.datetime_val
						                                      ,Year,Month,Day
						                                      //,Hour,Minute,Second,fSec
						                                      ,0,0,0,0
						                                      ,(OraText*)&NullStr
						                                      ,0);
			                      }
			                      break;
    default:                return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_TYPE_TIME(SQLPOINTER       DataPtr
	                        ,int              /*DataLen*/
                          ,int*             /*ResultLen*/
	                        ,OracleItems&     Holder
	                        ,ODBCErrorHolder& ociError
                          ,ODBCStatement*   p_statement
                          ,bool             /*p_putData*/)
{
	SQL_TIME_STRUCT &ValueBuff=*(SQL_TIME_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case INTEGER_TYPE:
		case DOUBLE_TYPE:       ociError.Clear();
			                      ociError.AddError("HY000",0,"Type conversion error");
			                      return SQL_ERROR;
		case DATE_TYPE:
			                      {
				                      ub1 Hour   = (ub1)ValueBuff.hour;
				                      ub1 Minute = (ub1)ValueBuff.minute;
				                      ub1 Second = (ub1)ValueBuff.second;
                      				
				                      OCIDateSetTime(&Holder.Value.date_val,Hour,Minute,Second);
				                      OCIDateSetDate(&Holder.Value.date_val,0,0,0);					
			                      }		
			                      break;
		case NUMBER_TYPE:
		case STRING_TYPE:			  ociError.Clear();
		                        ociError.AddError("HY000",0,"Type conversion error");
		                        return SQL_ERROR;
		case DATETIME_TYPE:
		case DATETIMEWTZ_TYPE:
		case DATETIMEWLTZ_TYPE:
		case ANSI_DATE_TYPE:    {				
			                        ub1 Hour   = (ub1)ValueBuff.hour;
			                        ub1 Minute = (ub1)ValueBuff.minute;
			                        ub1 Second = (ub1)ValueBuff.second;
			                        ub4 fSec   = 0;
			                        sb2 Year   = 0;
			                        ub1 Month  = 0;
			                        ub1 Day    = 0;
                      				
			                        OCIDateTimeConstruct(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                        ,ociError.GetOwnerErrorObject()
					                                        ,Holder.Value.datetime_val
					                                        ,Year,Month,Day
					                                        ,Hour,Minute,Second,fSec
					                                        ,(OraText*)&NullStr
					                                        ,0);				
		                        }
		                        break;
    default:                return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_TYPE_TIMESTAMP(SQLPOINTER        DataPtr
                               ,int               /*DataLen*/
                               ,int*              /*ResultLen*/
                               ,OracleItems&      Holder
                               ,ODBCErrorHolder&  ociError
                               ,ODBCStatement*    p_statement
                               ,bool              /*p_putData*/)
{
	SQL_TIMESTAMP_STRUCT& ValueBuff = *(SQL_TIMESTAMP_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
		case DATE_TYPE:
			                    {
				                    sb2 Year=ValueBuff.year;
				                    ub1 Month = (ub1)ValueBuff.month;
				                    ub1 Day   = (ub1)ValueBuff.day;
				                    ub1 Hour  = (ub1)ValueBuff.hour;
				                    ub1 Minute= (ub1)ValueBuff.minute;
				                    ub1 Second= (ub1)ValueBuff.second;
                    				
				                    OCIDateSetDate(&Holder.Value.date_val,Year,Month,Day);
				                    OCIDateSetTime(&Holder.Value.date_val,Hour,Minute,Second);				
			                    }
			                    break;
		case DATETIME_TYPE:
		case DATETIMEWTZ_TYPE:
		case DATETIMEWLTZ_TYPE:
		case ANSI_DATE_TYPE:
			                    {				
				                    sb2 Year=ValueBuff.year;
				                    ub1 Month  = (ub1)ValueBuff.month;
				                    ub1 Day    = (ub1)ValueBuff.day;
				                    ub1 Hour   = (ub1)ValueBuff.hour;
				                    ub1 Minute = (ub1)ValueBuff.minute;
				                    ub1 Second = (ub1)ValueBuff.second;
				                    ub4 fSec   = ValueBuff.fraction;
				                    OCIDateTimeConstruct(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
						                                    ,ociError.GetOwnerErrorObject()
						                                    ,Holder.Value.datetime_val
						                                    ,Year,Month,Day
						                                    ,Hour,Minute,Second,fSec
						                                    ,(OraText*)&NullStr
						                                    ,0);
			                    }
			                    break;
    default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_YEAR(SQLPOINTER       DataPtr
	                            ,int              /*DataLen*/
                              ,int*             ResultLen
	                            ,OracleItems&     Holder
	                            ,ODBCErrorHolder& ociError
                              ,ODBCStatement*   p_statement
                              ,bool             /*p_putData*/)
{
	SQL_INTERVAL_STRUCT &ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[20];
                            itoa(ValueBuff.intval.year_month.year,localBuffer,10);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,(int)strlen(localBuffer)
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALYM_TYPE: {				
			                      sb4 Year=0;
			                      sb4 Month=0;
			                      if(ValueBuff.interval_sign!=SQL_TRUE)
			                      {
				                      Year=ValueBuff.intval.year_month.year;					
			                      }
                            else
			                      {
				                      Year = - (signed)(ValueBuff.intval.year_month.year);					
			                      }
			                      OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                            ,ociError.GetOwnerErrorObject()
				                                            ,Year
				                                            ,Month
				                                            ,Holder.Value.interval_val);				
		                      }			
		                      break;		
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_MONTH(SQLPOINTER       DataPtr
                               ,int              /*DataLen*/
                               ,int*             ResultLen
                               ,OracleItems&     Holder
                               ,ODBCErrorHolder& ociError
                               ,ODBCStatement*   p_statement
                               ,bool             /*p_putData*/)
{
	SQL_INTERVAL_STRUCT &ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[20];
                            itoa(ValueBuff.intval.year_month.month,localBuffer,10);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,(int)strlen(localBuffer)
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALYM_TYPE: {
				                    sb4 Year=0;
				                    sb4 Month=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Year=ValueBuff.intval.year_month.month/12;					
					                    Month=ValueBuff.intval.year_month.month-(Year*12);
				                    }
                            else
				                    {
					                    Year  = -(signed)(ValueBuff.intval.year_month.month/12);
					                    Month = -(signed)(ValueBuff.intval.year_month.month-(Year*12));
				                    }
				                    OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Year
					                                          ,Month
					                                          ,Holder.Value.interval_val);				
			                    }
			                    break;		
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_YEAR_TO_MONTH(SQLPOINTER        DataPtr
	                                     ,int               /*DataLen*/
                                       ,int*              ResultLen
	                                     ,OracleItems&      Holder
	                                     ,ODBCErrorHolder&  ociError
                                       ,ODBCStatement*    p_statement
                                       ,bool              /*p_putData*/)
{
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[LOCAL_BUFFERLEN];
                            int  len = sprintf_s(localBuffer,LOCAL_BUFFERLEN,"%d-%d"
                                                ,ValueBuff.intval.year_month.year
                                                ,ValueBuff.intval.year_month.month);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,len + 1
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALYM_TYPE: {
				                    sb4 Year=0;
				                    sb4 Month=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Year=ValueBuff.intval.year_month.year;
					                    Month=ValueBuff.intval.year_month.month;
				                    }
                            else
				                    {
					                    Year  = -(signed)(ValueBuff.intval.year_month.year);
					                    Month = -(signed)(ValueBuff.intval.year_month.month);
				                    }
				                    OCIIntervalSetYearMonth(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Year
					                                          ,Month
					                                          ,Holder.Value.interval_val);				
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_DAY(SQLPOINTER        DataPtr
	                           ,int               /*DataLen*/
                             ,int*              ResultLen
	                           ,OracleItems&      Holder
	                           ,ODBCErrorHolder&  ociError
                             ,ODBCStatement*    p_statement
                             ,bool              /*p_putData*/)
{
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{		
    case STRING_TYPE:     {
                            char localBuffer[20];
                            itoa(ValueBuff.intval.day_second.hour,localBuffer,10);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,(int)strlen(localBuffer)
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
				                    sb4 Day=0;				
				                    sb4 Hour=0;
				                    sb4 Minute=0;
				                    sb4 Second=0;
				                    sb4 fraction=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Day = ValueBuff.intval.day_second.day;
				                    }
				                    else
				                    {
					                    Day = -(signed)(ValueBuff.intval.day_second.day);
				                    }
				                    OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Day		
					                                          ,Hour
					                                          ,Minute
					                                          ,Second
					                                          ,fraction
					                                          ,Holder.Value.interval_val);				
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_HOUR(SQLPOINTER       DataPtr
	                            ,int              /*DataLen*/
                              ,int*             ResultLen
	                            ,OracleItems&     Holder
	                            ,ODBCErrorHolder& ociError
                              ,ODBCStatement*   p_statement
                              ,bool             /*p_putData*/)
{
	SQL_INTERVAL_STRUCT& ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[20];
                            itoa(ValueBuff.intval.day_second.hour,localBuffer,10);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,(int)strlen(localBuffer)
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
				                    sb4 Day=0;				
				                    sb4 Hour=0;
				                    sb4 Minute=0;
				                    sb4 Second=0;
				                    sb4 fraction=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Day=ValueBuff.intval.day_second.hour/24;
					                    Hour=ValueBuff.intval.day_second.hour-(Day*24);
				                    }
				                    else
				                    {
					                    Day  = -(signed)(ValueBuff.intval.day_second.hour/24);
					                    Hour = -(signed)(ValueBuff.intval.day_second.hour-(Day*24));
				                    }
				                    OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Day		
					                                          ,Hour
					                                          ,Minute
					                                          ,Second
					                                          ,fraction
					                                          ,Holder.Value.interval_val);
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_MINUTE(SQLPOINTER       DataPtr
	                              ,int              /*DataLen*/
                                ,int*             ResultLen
	                              ,OracleItems&     Holder
	                              ,ODBCErrorHolder& ociError
                                ,ODBCStatement*   p_statement
                                ,bool             /*p_putData*/)
{
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[20];
                            itoa(ValueBuff.intval.day_second.minute,localBuffer,10);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,(int)strlen(localBuffer)
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
		                        sb4 Day=0;				
		                        sb4 Hour=0;
		                        sb4 Minute=0;
		                        sb4 Second=0;
		                        sb4 fraction=0;
		                        if(ValueBuff.interval_sign!=SQL_TRUE)
		                        {
			                        Day=ValueBuff.intval.day_second.minute/(24*60);
			                        Hour=(ValueBuff.intval.day_second.minute-(Day*24*60))/60;
			                        Minute=ValueBuff.intval.day_second.minute-(Day*24*60)-(Hour*60);
		                        }
		                        else
		                        {
			                        Day   =-(signed)(ValueBuff.intval.day_second.minute/(24*60));
			                        Hour  =-(signed)(ValueBuff.intval.day_second.minute-(Day*24*60))/60;
			                        Minute=-(signed)(ValueBuff.intval.day_second.minute-(Day*24*60)-(Hour*60));
		                        }
		                        OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
			                                              ,ociError.GetOwnerErrorObject()
			                                              ,Day		
			                                              ,Hour
			                                              ,Minute
			                                              ,Second
			                                              ,fraction
			                                              ,Holder.Value.interval_val);				
	                        }
	                        break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_SECOND(SQLPOINTER       DataPtr
	                              ,int              /*DataLen*/
                                ,int*             ResultLen
	                              ,OracleItems&     Holder
	                              ,ODBCErrorHolder& ociError
                                ,ODBCStatement*   p_statement
                                ,bool             /*p_putData*/)
{
	SQL_INTERVAL_STRUCT &ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[20];
                            itoa(ValueBuff.intval.day_second.second,localBuffer,10);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,(int)strlen(localBuffer)
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
				                    sb4 Day=0;				
				                    sb4 Hour=0;
				                    sb4 Minute=0;
				                    sb4 Second=0;
				                    sb4 fraction=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Day    =  ValueBuff.intval.day_second.second/(24*60*60);
					                    Hour   = (ValueBuff.intval.day_second.second-(Day*24*60*60))/(60*60);
					                    Minute = (ValueBuff.intval.day_second.second-(Day*24*60*60)-(Hour*60*60))/60;
					                    Second =  ValueBuff.intval.day_second.second-(Day*24*60*60)-(Hour*60*60)-(Minute*60);
				                    }
				                    else
				                    {
					                    Day    = -(signed)(ValueBuff.intval.day_second.second/(24*60*60));
					                    Hour   = -(signed)(ValueBuff.intval.day_second.second-(Day*24*60*60))/(60*60);
					                    Minute = -(signed)(ValueBuff.intval.day_second.second-(Day*24*60*60)-(Hour*60*60))/60;
					                    Second = -(signed)(ValueBuff.intval.day_second.second-(Day*24*60*60)-(Hour*60*60)-(Minute*60));
				                    }
				                    OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Day		
					                                          ,Hour
					                                          ,Minute
					                                          ,Second
					                                          ,fraction
					                                          ,Holder.Value.interval_val);					
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_DAY_TO_HOUR(SQLPOINTER        DataPtr
	                                   ,int               /*DataLen*/
                                     ,int*              ResultLen
	                                   ,OracleItems&      Holder
	                                   ,ODBCErrorHolder&  ociError
                                     ,ODBCStatement*    p_statement
                                     ,bool              /*p_putData*/)
{
	SQL_INTERVAL_STRUCT &ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[LOCAL_BUFFERLEN];
                            int  len = sprintf_s(localBuffer,LOCAL_BUFFERLEN,"%d %d"
                                                ,ValueBuff.intval.day_second.day
                                                ,ValueBuff.intval.day_second.hour);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,len + 1
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
				                    sb4 Day=0;				
				                    sb4 Hour=0;
				                    sb4 Minute=0;
				                    sb4 Second=0;
				                    sb4 fraction=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Day=ValueBuff.intval.day_second.day;
					                    Hour=ValueBuff.intval.day_second.hour;					
				                    }
				                    else
				                    {
					                    Day  = -(signed)(ValueBuff.intval.day_second.day);
					                    Hour = -(signed)(ValueBuff.intval.day_second.hour);
				                    }
				                    OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Day		
					                                          ,Hour
					                                          ,Minute
					                                          ,Second
					                                          ,fraction
					                                          ,Holder.Value.interval_val);				
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_DAY_TO_MINUTE(SQLPOINTER       DataPtr
	                                     ,int              /*DataLen*/
                                       ,int*             ResultLen
	                                     ,OracleItems&     Holder
	                                     ,ODBCErrorHolder& ociError
                                       ,ODBCStatement*   p_statement
                                       ,bool             /*p_putData*/)
{
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[LOCAL_BUFFERLEN];
                            int  len = sprintf_s(localBuffer,LOCAL_BUFFERLEN,"%d %d:%d"
                                                ,ValueBuff.intval.day_second.day
                                                ,ValueBuff.intval.day_second.hour
                                                ,ValueBuff.intval.day_second.minute);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,len + 1
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
				                    sb4 Day=0;				
				                    sb4 Hour=0;
				                    sb4 Minute=0;
				                    sb4 Second=0;
				                    sb4 fraction=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Day=ValueBuff.intval.day_second.day;
					                    Hour=ValueBuff.intval.day_second.hour;
					                    Minute=ValueBuff.intval.day_second.minute;
				                    }
				                    else
				                    {
					                    Day    = -(signed)(ValueBuff.intval.day_second.day);
					                    Hour   = -(signed)(ValueBuff.intval.day_second.hour);
					                    Minute = -(signed)(ValueBuff.intval.day_second.minute);
				                    }
				                    OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Day		
					                                          ,Hour
					                                          ,Minute
					                                          ,Second
					                                          ,fraction
					                                          ,Holder.Value.interval_val);
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_DAY_TO_SECOND(SQLPOINTER        DataPtr
	                                     ,int               /*DataLen*/
                                       ,int*              ResultLen
	                                     ,OracleItems&      Holder
	                                     ,ODBCErrorHolder&  ociError
                                       ,ODBCStatement*    p_statement
                                       ,bool              /*p_putData*/)
{
	SQL_INTERVAL_STRUCT &ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[LOCAL_BUFFERLEN];
                            int  len = sprintf_s(localBuffer,LOCAL_BUFFERLEN,"%d %d:%d:%d"
                                                ,ValueBuff.intval.day_second.day
                                                ,ValueBuff.intval.day_second.hour
                                                ,ValueBuff.intval.day_second.minute
                                                ,ValueBuff.intval.day_second.second);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,len + 1
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
				                    sb4 Day=0;				
				                    sb4 Hour=0;
				                    sb4 Minute=0;
				                    sb4 Second=0;
				                    sb4 fraction=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Day=ValueBuff.intval.day_second.day;
					                    Hour=ValueBuff.intval.day_second.hour;
					                    Minute=ValueBuff.intval.day_second.minute;
					                    Second=ValueBuff.intval.day_second.second;
				                    }
				                    else
				                    {
					                    Day    = -(signed)(ValueBuff.intval.day_second.day);
					                    Hour   = -(signed)(ValueBuff.intval.day_second.hour);
					                    Minute = -(signed)(ValueBuff.intval.day_second.minute);
					                    Second = -(signed)(ValueBuff.intval.day_second.second);
				                    }
				                    OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Day		
					                                          ,Hour
					                                          ,Minute
					                                          ,Second
					                                          ,fraction
					                                          ,Holder.Value.interval_val);
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_HOUR_TO_MINUTE(SQLPOINTER       DataPtr
	                                      ,int              /*DataLen*/
                                        ,int*             ResultLen
	                                      ,OracleItems&     Holder
	                                      ,ODBCErrorHolder& ociError
                                        ,ODBCStatement*   p_statement
                                        ,bool             /*p_putData*/)
{
	SQL_INTERVAL_STRUCT &ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[LOCAL_BUFFERLEN];
                            int  len = sprintf_s(localBuffer,LOCAL_BUFFERLEN,"%d:%d"
                                                ,ValueBuff.intval.day_second.hour
                                                ,ValueBuff.intval.day_second.minute);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,len + 1
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
				                    sb4 Day=0;				
				                    sb4 Hour=0;
				                    sb4 Minute=0;
				                    sb4 Second=0;
				                    sb4 fraction=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Day=ValueBuff.intval.day_second.hour/24;
					                    Hour=ValueBuff.intval.day_second.hour-Day*24;
					                    Minute=ValueBuff.intval.day_second.minute;
				                    }
				                    else
				                    {
					                    Day    = -(signed)(ValueBuff.intval.day_second.hour/24);
					                    Hour   = -(signed)(ValueBuff.intval.day_second.hour-Day*24);
					                    Minute = -(signed)(ValueBuff.intval.day_second.minute);
				                    }
				                    OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                         ,ociError.GetOwnerErrorObject()
					                                         ,Day		
					                                         ,Hour
					                                         ,Minute
					                                         ,Second
					                                         ,fraction
					                                         ,Holder.Value.interval_val);
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_HOUR_TO_SECOND(SQLPOINTER       DataPtr
	                                      ,int              /*DataLen*/
                                        ,int*             ResultLen
	                                      ,OracleItems&     Holder
	                                      ,ODBCErrorHolder& ociError
                                        ,ODBCStatement*   p_statement
                                        ,bool             /*p_putData*/)
{
	SQL_INTERVAL_STRUCT &ValueBuff=*(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[LOCAL_BUFFERLEN];
                            int  len = sprintf_s(localBuffer,LOCAL_BUFFERLEN,"%d:%d:%d"
                                                ,ValueBuff.intval.day_second.hour
                                                ,ValueBuff.intval.day_second.minute
                                                ,ValueBuff.intval.day_second.second);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,len + 1
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
				                    sb4 Day=0;				
				                    sb4 Hour=0;
				                    sb4 Minute=0;
				                    sb4 Second=0;
				                    sb4 fraction=0;
				                    if(ValueBuff.interval_sign!=SQL_TRUE)
				                    {
					                    Day=ValueBuff.intval.day_second.hour/24;
					                    Hour=ValueBuff.intval.day_second.hour-Day*24;
					                    Minute=ValueBuff.intval.day_second.minute;
					                    Second=ValueBuff.intval.day_second.second;
				                    }
				                    else
				                    {
					                    Day    = -(signed)(ValueBuff.intval.day_second.hour/24);
					                    Hour   = -(signed)(ValueBuff.intval.day_second.hour-Day*24);
					                    Minute = -(signed)(ValueBuff.intval.day_second.minute);
					                    Second = -(signed)(ValueBuff.intval.day_second.second);
				                    }
				                    OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
					                                          ,ociError.GetOwnerErrorObject()
					                                          ,Day		
					                                          ,Hour
					                                          ,Minute
					                                          ,Second
					                                          ,fraction
					                                          ,Holder.Value.interval_val);
			                    }
			                    break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_INTERVAL_MINUTE_TO_SECOND(SQLPOINTER       DataPtr
                                          ,int              /*DataLen*/
                                          ,int*             ResultLen
                                          ,OracleItems&     Holder
                                          ,ODBCErrorHolder& ociError
                                          ,ODBCStatement*   p_statement
                                          ,bool             /*p_getData*/)
{
	SQL_INTERVAL_STRUCT& ValueBuff = *(SQL_INTERVAL_STRUCT*)DataPtr;
	switch(Holder.m_valueType)
	{
    case STRING_TYPE:     {
                            char localBuffer[LOCAL_BUFFERLEN];
                            int  len = sprintf_s(localBuffer,LOCAL_BUFFERLEN,"%d:%d"
                                                ,ValueBuff.intval.day_second.minute
                                                ,ValueBuff.intval.day_second.second);
                            return CopyODBCStringToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                        ,localBuffer
                                                        ,len + 1
                                                        ,ResultLen
                                                        ,&ociError
                                                        ,Holder
                                                        ,p_statement->GetMaxLength());
                          }
                          break;
		case INTERVALDS_TYPE: {
			                      sb4 Day=0;				
			                      sb4 Hour=0;
			                      sb4 Minute=0;
			                      sb4 Second=0;
			                      sb4 fraction=0;
			                      if(ValueBuff.interval_sign != SQL_TRUE)
			                      {
				                      Day    =  ValueBuff.intval.day_second.minute/(24*60);
				                      Hour   = (ValueBuff.intval.day_second.minute-Day*24*60)/60;
				                      Minute =  ValueBuff.intval.day_second.minute-Day*24*60-Hour*60;
				                      Second =  ValueBuff.intval.day_second.second;
			                      }
			                      else
			                      {
				                      Day    = - (signed)(ValueBuff.intval.day_second.minute/(24*60));
				                      Hour   = - (signed)(ValueBuff.intval.day_second.minute-Day*24*60)/60;
				                      Minute = - (signed)(ValueBuff.intval.day_second.minute-Day*24*60-Hour*60);
				                      Second = - (signed)(ValueBuff.intval.day_second.second);
			                      }
			                      OCIIntervalSetDaySecond(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
				                                           ,ociError.GetOwnerErrorObject()
				                                           ,Day		
				                                           ,Hour
				                                           ,Minute
				                                           ,Second
				                                           ,fraction
				                                           ,Holder.Value.interval_val);
		                      }
		                      break;
		default:              return TypeConversionError(ociError,Holder);
	}
	return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_GUID(SQLPOINTER       DataPtr
                     ,int              /*DataLen*/
                     ,int*             /*ResultLen*/
                     ,OracleItems&     Holder
                     ,ODBCErrorHolder& ociError
                     ,ODBCStatement*   p_statement
                     ,bool             /*p_getData*/)
{
  SQLGUID* guid = (SQLGUID*)DataPtr;
  char localBuffer[LOCAL_BUFFERLEN];
  switch(Holder.m_valueType)
  {
    case STRING_TYPE: sprintf_s(localBuffer
                               ,LOCAL_BUFFERLEN
                               ,"%lX-%X-%X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X"
                               ,guid->Data1
                               ,guid->Data2
                               ,guid->Data3
                               ,guid->Data4[0]
                               ,guid->Data4[1]
                               ,guid->Data4[2]
                               ,guid->Data4[3]
                               ,guid->Data4[4]
                               ,guid->Data4[5]
                               ,guid->Data4[6]
                               ,guid->Data4[7]);
                      OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                          ,ociError.GetOwnerErrorObject()
                                          ,(OraText*)localBuffer
                                          ,(ub4)strlen(localBuffer)
                                          ,&Holder.Value.string_val);
                      break;
    default:          return TypeConversionError(ociError,Holder);
  }
  return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_NUMERIC(SQLPOINTER       DataPtr
                        ,int              /*DataLen*/
                        ,int*             ResultLen
                        ,OracleItems&     Holder
                        ,ODBCErrorHolder& ociError
                        ,ODBCStatement*   p_statement
                        ,bool             /*p_getData*/)
{
  SQL_NUMERIC_STRUCT* numeric = (SQL_NUMERIC_STRUCT*)DataPtr;
  SQLCHAR buffer [SQLNUM_MAXLEN+1];
  char    pattern[SQLNUM_MAXLEN+1];
  char*   numbegin = NULL;
  int truncate = 0;

  memset(buffer,0,SQLNUM_MAXLEN+1);
  NumericToString(numeric
                 ,&buffer[SQLNUM_MAXLEN]
                 ,(SQLCHAR**)&numbegin
                 ,numeric->precision
                 ,numeric->scale
                 ,&truncate);
  if(truncate)
  {
    // Empty result
    numbegin = (char*)buffer;
    buffer[0] = 0;
  }
  switch(Holder.m_valueType)
  {
    case INTEGER_TYPE:  Holder.Value.int_val=0;
                        if(sscanf(numbegin,"%d",&Holder.Value.int_val)!=1)
                        {
                          ociError.AddError("HY000",0,"To integer conversion error");
                          return SQL_ERROR;
                        }
                        break;
    case DOUBLE_TYPE:   {
                          double value;
                          if(StringToDouble(numbegin,&value) == false)
                          {
                            // Numeric value out of range
                            ociError.AddError("22003");
                            return SQL_ERROR;
                          }
                          Holder.Value.double_val = value;
                        }
                        break;
    case NUMBER_TYPE:		MakeNumericPattern(numbegin,pattern);
                        if(OCINumberFromText(ociError.GetOwnerErrorObject()
                                            ,(const oratext*)numbegin
                                            ,(ub4)    strlen(numbegin)
                                            ,(const oratext*)pattern
                                            ,(ub4)    strlen(pattern)
                                            ,(const oratext*)NLS_NUMERIC_CHARACTERS
                                            ,(ub4)           NLS_LENGTH
                                            ,&Holder.Value.number_val) != OCI_SUCCESS)
                        {
                          ociError.GetErrorFromOCIHandle();
                          return SQL_ERROR;
                        }
                        break;
    case STRING_TYPE:   OCIStringAssignText(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                           ,ociError.GetOwnerErrorObject()
                                           ,(OraText*)numbegin
                                           ,(ub4)strlen(numbegin)
                                           ,&Holder.Value.string_val);				
                        break;
    default:            return TypeConversionError(ociError,Holder);
  }
  if(ResultLen)
  {
    *ResultLen = sizeof(SQL_NUMERIC_STRUCT);
  }
  return SQL_SUCCESS;
}

SQLRETURN 
ConvertFromSQL_C_BINARY(SQLPOINTER       DataPtr
                       ,int              DataLen
                       ,int*             ResultLen
                       ,OracleItems&     Holder
                       ,ODBCErrorHolder& ociError
                       ,ODBCStatement*   p_statement
                       ,bool             /*p_putData*/)
{
  switch(Holder.m_valueType)
  {
    case LONG_TYPE:     return CopyODBCBufferToRDBMS((char*)(Holder.Value.long_val + sizeof(int))
                                                    ,*((int*)Holder.Value.long_val)
                                                    ,(char*)DataPtr
                                                    ,DataLen
                                                    ,ResultLen
                                                    ,ociError
                                                    ,Holder);
    case RAW_TYPE:      // Fall through
    case LONGRAW_TYPE:  return CopyODBCRAWToRDBMS(p_statement->GetDBConnection()->GetOwnerODBCEnv()->GetOwnerEnv()
                                                 ,Holder.Value.raw_val
                                                 ,DataPtr
                                                 ,DataLen
                                                 ,ResultLen
                                                 ,&ociError
                                                 ,p_statement->GetMaxLength());
    case BLOB_TYPE:     return CopyODBCLOBToRDBMS(p_statement->GetDBConnection()->GetOCIConnection()
                                                 ,Holder.Value.lob_val
                                                 ,DataPtr
                                                 ,DataLen
                                                 ,ResultLen
                                                 ,ociError
                                                 ,&Holder);
    default:            return TypeConversionError(ociError,Holder);
  }
}
