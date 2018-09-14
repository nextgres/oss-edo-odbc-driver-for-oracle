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
// Contents:  Implementation of the Error info
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "ErrorInfo.h"
#include "DescriptorIRD.h"
#include "ODBCTypes.h"
#include "Connection.h"

SQLRETURN TranslateOCIReturnCodeToSQLReturnCode(signed int status)
{
  switch (status)
  {
    case OCI_SUCCESS:           return SQL_SUCCESS;
    case OCI_SUCCESS_WITH_INFO: return SQL_SUCCESS_WITH_INFO;
    case OCI_NEED_DATA:         return SQL_NEED_DATA;
    case OCI_NO_DATA:           return SQL_NO_DATA;
    case OCI_ERROR:             return SQL_ERROR;
    case OCI_INVALID_HANDLE:    return SQL_INVALID_HANDLE;
    case OCI_STILL_EXECUTING:   return SQL_STILL_EXECUTING;
    case OCI_CONTINUE:          return OCI_ERROR; 
  }
  return OCI_ERROR;
}

// Standard error text strings for ODBC known errors
struct
{
  char  SQLState[6];        // Standard ODBC SQLSTATE field
  int   InternalErrorCode;  // Internal driver error code
  char* ErrorTextString;    // Text description of code
} 
ODBCStandardErrorText[] =
{
   "01004",0   ,"String data, right truncated"
  ,"01005",0   ,"Binary data, right truncated"
  ,"01S02",0   ,"Option value changed"
  ,"07002",0   ,"COUNT field of SQLBindParameter's incorrect"
  ,"07006",0   ,"Datatype attribute violation / incorrect datatype"
  ,"07009",0   ,"Invalid descriptor index"
  ,"22002",0   ,"Indicator variable required but not supplied"
  ,"22003",0   ,"Numeric value out of range"
  ,"22026",0   ,"String data, length mismatch"
  ,"24000",0   ,"Invalid cursor state (already prepared/executed cursor)"
  ,"3C000",0   ,"Duplicate cursor name"
  ,"42000",0   ,"Syntax error"
  ,"HY000",1567,"Conversion function not found"
  ,"HY001",0   ,"Memory allocation error"
  ,"HY004",0   ,"Unknown oracle data type"
  ,"HY005",0   ,"Invalid parameter type"
  ,"HY008",0   ,"Operation canceled"
  ,"HY010",0   ,"Function sequence error"
  ,"HY016",0   ,"Cannot modify an implementation row descriptor"
  ,"HY021",0   ,"Inconsistent descriptor information"
  ,"HY024",0   ,"Invalid attribute/option value"
  ,"HY090",0   ,"Invalid string or buffer length"
  ,"HY091",0   ,"Invalid descriptor field identifier"
  ,"HY092",8092,"Invalid attribute/option identifier"
  ,"HY095",0   ,"Function type out of range"
  ,"HYC00",8100,"Optional feature not implemented"
  ,"HYD00",0   ,"Level of escape sequence left hanging"
  ,"HYD01",0   ,"Unknown { fn function()} code"
  ,"S1008",0   ,"Operation canceled"
  ,"S1010",0   ,"Function sequence error"
  ,"S1106",0   ,"Fetch type out of range"
  ,"R0000",0   ,"Read-only connection and not a SELECT statement"
  ,"R1001",0   ,"Cannot load the translation library"
  ,"R1002",0   ,"Translation library does not contain all functions"
  ,"R1003",0   ,"Error in translation lib"
};

const char*
FindODBCErrorText(const char *SQLState,int &InternalCode)
{
  for(int i=0,len=sizeof(ODBCStandardErrorText)/sizeof(ODBCStandardErrorText[0]);i < len; ++i)
  {
    if(strcmp(ODBCStandardErrorText[i].SQLState,SQLState) == 0)
    {
      if(InternalCode == 0 && ODBCStandardErrorText[i].InternalErrorCode > 0)
      {
        InternalCode = ODBCStandardErrorText[i].InternalErrorCode;
      }
      return ODBCStandardErrorText[i].ErrorTextString;
    }
  }
  return "";
}

char *OraTypesEnumerationToStringRepresentation[]=
{
   "UNKNOWN"
  ,"DATE"
  ,"NUMBER"
  ,"STRING"
  ,"DATETIME"
  ,"IntervalYM"
  ,"RowID"
  ,"ANSI_DATE"
  ,"DateTimeTZ"
  ,"DateTimeLTZ"
  ,"IntervalDS"
  ,"INTEGER"
  ,"DOUBLE"
  ,"CLOB"
  ,"BLOB"
  ,"LONG"
};

const char*
GetOraTypesEnumerationInString(OraTypesEnumeration Type)
{
  int Len=sizeof(OraTypesEnumerationToStringRepresentation)/sizeof(OraTypesEnumerationToStringRepresentation[0]);
  if(Type>=Len || Type<0)
  {
    return "INTERNAL ERROR";
  }
  else
  {
    return OraTypesEnumerationToStringRepresentation[Type];
  }
}	

#ifdef EDO_LOGGING

const char* 
GetHandleTypeDescription(SQLSMALLINT HT)
{
  switch(HT)
  {
    case SQL_HANDLE_STMT:		return "SQL_HANDLE_STMT";
    case SQL_HANDLE_ENV:		return "SQL_HANDLE_ENV";
    case SQL_HANDLE_DBC:		return "SQL_HANDLE_DBC";
    case SQL_HANDLE_DESC:		return "SQL_HANDLE_DESC";    
    default:             		return "SQL_HANDLE_UNKNOWN";
  }
}

const char*
ERROR_GetConnectAttribute(SQLINTEGER p_attribute)
{
  switch(p_attribute)
  {
    case SQL_ATTR_ACCESS_MODE:			    return "SQL_ATTR_ACCESS_MODE";
    case SQL_ATTR_ASYNC_ENABLE:			    return "SQL_ATTR_ASYNC_ENABLE";
    case SQL_ATTR_AUTO_IPD:             return "SQL_ATTR_AUTO_IPD";
    case SQL_ATTR_AUTOCOMMIT:  		  	  return "SQL_ATTR_AUTOCOMMIT";
    case SQL_ATTR_CONNECTION_TIMEOUT:	  return "SQL_ATTR_ENLIST_IN_DTC";
    case SQL_ATTR_CURRENT_CATALOG:		  return "SQL_ATTR_ENLIST_IN_DTC";
    case SQL_ATTR_DISCONNECT_BEHAVIOR:  return "SQL_ATTR_DISCONNECT_BEHAVIOR";
    case SQL_ATTR_ENLIST_IN_DTC:			  return "SQL_ATTR_ENLIST_IN_DTC";
    case SQL_ATTR_ENLIST_IN_XA:   			return "SQL_ATTR_ENLIST_IN_XA";
    case SQL_ATTR_LOGIN_TIMEOUT:  			return "SQL_ATTR_LOGIN_TIMEOUT";
    case SQL_ATTR_METADATA_ID:          return "SQL_ATTR_METADATA_ID";
    case SQL_ATTR_ODBC_CURSORS:     		return "SQL_ATTR_ODBC_CURSORS";
    case SQL_ATTR_PACKET_SIZE:    			return "SQL_ATTR_PACKET_SIZE";
    case SQL_ATTR_QUIET_MODE:       		return "SQL_ATTR_QUIET_MODE";
    case SQL_ATTR_TRACE:          			return "SQL_ATTR_TRACE";
    case SQL_ATTR_TRACEFILE:  			    return "SQL_ATTR_TRACEFILE";
    case SQL_ATTR_TRANSLATE_LIB:			  return "SQL_ATTR_TRANSLATE_LIB";
    case SQL_ATTR_TRANSLATE_OPTION:			return "SQL_ATTR_TRANSLATE_OPTION";
    case SQL_ATTR_TXN_ISOLATION: 			  return "SQL_ATTR_TXN_ISOLATION";
    case SQL_ATTR_CONNECTION_DEAD:			return "SQL_ATTR_CONNECTION_DEAD";
    case SQL_ATTR_ANSI_APP:             return "SQL_ATTR_ANSI_APP";
    // DRIVER SPECIFIC!
    case SQL_ATTR_EDO_ENSURE_STATISTICS:return "SQL_ATTR_EDO_ENSURE_STATISTICS";
    case SQL_ATTR_EDO_PREFECTH_COUNT:   return "SQL_ATTR_EDO_PREFECTH_COUNT";
    case SQL_ATTR_EDO_PREFETCH_MEMORY:  return "SQL_ATTR_EDO_PREFETCH_MEMORY";
    case SQL_ATTR_EDO_FAILOVER_COUNT:   return "SQL_ATTR_EDO_FAILOVER_COUNT";
    case SQL_ATTR_EDO_FAILOVER_TIME:    return "SQL_ATTR_EDO_FAILOVER_TIME";
    case SQL_ATTR_EDO_TRACE:          	return "SQL_ATTR_EDO_TRACE";
    case SQL_ATTR_EDO_TRACEFILE:  			return "SQL_ATTR_EDO_TRACEFILE";
    default:		                        return "UNKNOWN";
  }
}

const char* 
ERROR_GetEnvAttribute(SQLINTEGER p_attribute)
{
  switch(p_attribute)
  {
    case SQL_ATTR_ODBC_VERSION:			  return "SQL_ATTR_ODBC_VERSION";
    case SQL_ATTR_CONNECTION_POOLING:	return "SQL_ATTR_CONNECTION_POOLING";
    case SQL_ATTR_CP_MATCH:			      return "SQL_ATTR_CP_MATCH";
    case SQL_ATTR_OUTPUT_NTS:         return "SQL_ATTR_OUTPUT_NTS";
    default:		                      return "UNKNOWN";
  }
}

const char*
ERROR_GetStmtAttribute(SQLINTEGER p_attribute)
{
  switch(p_attribute)
  {		
    case SQL_ATTR_APP_ROW_DESC:		return "SQL_ATTR_APP_ROW_DESC";
    case SQL_ATTR_APP_PARAM_DESC: return "SQL_ATTR_APP_PARAM_DESC";
    case SQL_ATTR_IMP_ROW_DESC:	  return "SQL_ATTR_IMP_ROW_DESC";
    case SQL_ATTR_IMP_PARAM_DESC:	return "SQL_ATTR_IMP_PARAM_DESC";
    case SQL_ATTR_METADATA_ID:    return "SQL_ATTR_METADATA_ID";
    case SQL_ATTR_ENABLE_AUTO_IPD:return "SQL_ATTR_ENABLE_AUTO_IPD";
    case SQL_ATTR_NOSCAN:         return "SQL_ATTR_NOSCAN";
    default:                      return "UNKNOWN";
  }
}

#endif