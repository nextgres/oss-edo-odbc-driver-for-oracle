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
//////////////////////////////////////////////////////////////////////////
//
// Contents:  Missing OCI declarations in Oracle 9i
//            Prototypes for the Conversion functions
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once

#include <stdio.h>
#include <string.h>
#include <oci.h>
#include <sql.h>
#include <sqlext.h>

// Missing from the standard Oracle 9.x headers!!
#ifdef  __cplusplus
extern "C" {
sword OCIRowidToChar(OCIRowid* rowidDesc, 
                     OraText*  outbfp, 
                     ub2*      outbflp, 
                     OCIError* errhp);
}
#endif
class OracleItems;
class ODBCStatement;

// Type define of conversion functions
// 	
// 	DataPtr     - Ptr to conversion value buffer
// 	DataLen     - Len of DataPtr
//  ResultLen   - Len of actual data copied
// 	Holder      - Holder of oracle value
// 	ociError    - holder of OCIError object
// 	p_statement  - ptr to statement in which the conversion takes place
// 
//  returns SQL_SUCCESS for success, and other ODBC error codes for error results
//
typedef SQLRETURN (*ConversionToFunction)(SQLPOINTER        DataPtr
	                                       ,int               DataLen
                                         ,int*              ResultLen
	                                       ,OracleItems&      Holder
	                                       ,ODBCErrorHolder&  ociError
	                                       ,ODBCStatement*    p_statement
                                         ,bool              p_getData);
	
//! struct for conversion function to SQL ODBC type codes
typedef struct tagConversionFunctions
{
	SQLSMALLINT          ODBC_Type;
	ConversionToFunction ConversionFunction;
}
ConversionFunctions;

extern ConversionFunctions ConversionFunctionToODBC_SQL[];
extern ConversionFunctions ConversionFunctionFromODBC_SQL[];

//  CONVERSION function to SQL ODBC
// 	
// 	ODBC_Type   - SQL ODBC type code
// 	DataPtr     - Ptr to conversion value buffer
// 	DataLen     - Len of DataPtr
// 	Holder      - Holder of oracle value
// 	ociError    - holder of OCIError object
// 	p_statement - ptr to statement in which the conversion takes place
//  p_getData   - Streaming mode for SQLGetData
// 
// 	return SQL_SUCCESS for success, and other ODBC error codes for error results

SQLRETURN 
DatasourceToDriver(SQLSMALLINT      ODBC_Type
                  ,SQLPOINTER       DataPtr
                  ,int              DataLen
                  ,SQLINTEGER*      StrLen_or_IndPtr
                  ,OracleItems&     Holder
                  ,ODBCErrorHolder& ociError
                  ,ODBCStatement*   p_statement
                  ,bool             p_getData);

SQLRETURN 
DriverToDatasource(SQLSMALLINT      ODBC_Type
                  ,SQLPOINTER       DataPtr
                  ,int              DataLen
                  ,SQLINTEGER*      StrLen_or_IndPtr
                  ,OracleItems&     Holder
                  ,ODBCErrorHolder& ociError
                  ,ODBCStatement*   p_statement
                  ,bool             p_getData);
	

