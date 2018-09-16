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
// Contents:  MetaData for the ODBC Main Module
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "MetaData.h"

MetaQuery metaQueries[] =
{
  { META_TYPE, SQL_VARCHAR,
               "SELECT 'VARCHAR2'\n"    // TYPE_NAME
               "      ,12\n"            // DATA_TYPE = SQL_VARCHAR 
               "      ,4000\n"          // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,'max length'\n"  // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,3\n"             // SEARCHABLE = SQL_SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'VARCHAR2'\n"    // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,12\n"            // SQL_DATA_TYPE = SQL_VARCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" },
  { META_TYPE, SQL_WVARCHAR,
               "SELECT 'NVARCHAR2'\n"    // TYPE_NAME
               "      ,-9\n"            // DATA_TYPE = SQL_VARCHAR 
               "      ,2000\n"          // COLUMN_SIZE (max)
               "      ,'N'''\n"         // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,'max length'\n"  // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,3\n"             // SEARCHABLE = SQL_SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'NVARCHAR2'\n"   // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-9\n"            // SQL_DATA_TYPE = SQL_WVARCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_CHAR,
               "SELECT 'CHAR'\n"        // TYPE_NAME
               "      ,1\n"             // DATA_TYPE = SQL_CHAR 
               "      ,2000\n"          // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,'max length'\n"  // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,3\n"             // SEARCHABLE = SQL_SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'CHAR'\n"        // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,1\n"             // SQL_DATA_TYPE = SQL_CHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_WCHAR,
               "SELECT 'NCHAR'\n"       // TYPE_NAME
               "      ,-8\n"            // DATA_TYPE = SQL_WCHAR 
               "      ,1000\n"          // COLUMN_SIZE (max)
               "      ,'N'''\n"         // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,'max length'\n"  // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,3\n"             // SEARCHABLE = SQL_SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'NCHAR'\n"       // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-8\n"            // SQL_DATA_TYPE = SQL_WCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_DECIMAL,
               "SELECT 'DECIMAL'\n"     // TYPE_NAME
               "      ,3\n"             // DATA_TYPE = SQL_DECIMAL
               "      ,38\n"            // COLUMN_SIZE (max)
               "      ,''\n"            // LITERAL_PREFIX
               "      ,''\n"            // LITERAL_SUFFIX
               "      ,'precision,scale'\n"      // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = TRUE
               "      ,3\n"             // SEARCHABLE = SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'DECIMAL'\n"     // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,38\n"            // MAXIMUM_SCALE = 0
               "      ,3\n"             // SQL_DATA_TYPE = SQL_DECIMAL
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,2\n"             // NUM_PREC_RADIX = BINARY IEEEE type
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_NUMERIC,
               "SELECT 'NUMBER'\n"      // TYPE_NAME
               "      ,2\n"             // DATA_TYPE = SQL_NUMERIC
               "      ,38\n"            // COLUMN_SIZE (max)
               "      ,''\n"            // LITERAL_PREFIX
               "      ,''\n"            // LITERAL_SUFFIX
               "      ,'precision,scale'\n"      // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = TRUE
               "      ,3\n"             // SEARCHABLE = SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'NUMBER'\n"     // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,38\n"            // MAXIMUM_SCALE = 0
               "      ,2\n"             // SQL_DATA_TYPE = SQL_NUMERIC
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,2\n"             // NUM_PREC_RADIX = BINARY IEEEE type
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_DOUBLE,
               "SELECT 'DOUBLE PRECISION'\n"     // TYPE_NAME
               "      ,8\n"             // DATA_TYPE = SQL_DOUBLE
               "      ,15\n"            // COLUMN_SIZE (max)
               "      ,''\n"            // LITERAL_PREFIX
               "      ,''\n"            // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = TRUE
               "      ,3\n"             // SEARCHABLE = SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'DOUBLE PRECISION'\n"     // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,15\n"            // MAXIMUM_SCALE = 0
               "      ,8\n"             // SQL_DATA_TYPE = SQL_DOUBLE
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,2\n"             // NUM_PREC_RADIX = BINARY IEEEE type
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_TYPE_TIMESTAMP,
               "SELECT 'DATE'\n"        // TYPE_NAME
               "      ,93\n"            // DATA_TYPE = SQL_TIMESTAMP
               "      ,19\n"            // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = FALSE
               "      ,3\n"             // SEARCHABLE = SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'DATE'\n"        // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,11\n"            // SQL_DATA_TYPE = SQL_TIMESTAMP
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = BINARY IEEEE type
               "      ,6\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_TYPE_TIMESTAMP,
               "SELECT 'TIMESTAMP'\n"   // TYPE_NAME
               "      ,91\n"            // DATA_TYPE = SQL_TYPE_TIMESTAMP
               "      ,26\n"            // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = FALSE
               "      ,3\n"             // SEARCHABLE = SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'TIMESTAMP'\n"   // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,91\n"            // SQL_DATA_TYPE = SQL_TYPE_TIMESTAMP
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = BINARY IEEEE type
               "      ,6\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_LONGVARCHAR,
               "SELECT 'LONG'\n"        // TYPE_NAME
               "      ,-1\n"            // DATA_TYPE = SQL_LONGVARCHAR
               "      ,2147483647\n"    // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,0\n"             // SEARCHABLE = SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'LONG'\n"        // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-1\n"            // SQL_DATA_TYPE = SQL_LONGVARCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_INTERVAL_YEAR_TO_MONTH,
               "SELECT 'INTERVAL YEAR TO MONTH'\n"        // TYPE_NAME
               "      ,107\n"           // DATA_TYPE = SQL_INTERVAL_YEAR_TO_MONTH (100 + YEAR TO MONTH)
               "      ,0\n"             // COLUMN_SIZE (max)
               "      ,'INTERVAL'''\n"  // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,'year'\n"        // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = TRUE
               "      ,2\n"             // SEARCHABLE = SQL_ALL_EXCEPT_LIKE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'INTERVAL YEAR TO MONTH'\n"        // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,107\n"           // SQL_DATA_TYPE = SQL_INTERVAL_YEAR_TO_MONTH
               "      ,7\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,2\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_INTERVAL_DAY_TO_SECOND,
               "SELECT 'INTERVAL DAY TO SECOND'\n"        // TYPE_NAME
               "      ,110\n"           // DATA_TYPE = SQL_INTERVAL_DAY_TO_SECOND (100 + DAY_TO_SECOND)
               "      ,0\n"             // COLUMN_SIZE (max)
               "      ,'INTERVAL'''\n"  // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,'day,second'\n"  // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = TRUE
               "      ,2\n"             // SEARCHABLE = SQL_ALL_EXCEPT_LIKE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'INTERVAL DAY TO MONTH'\n"        // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,110\n"           // SQL_DATA_TYPE = SQL_INTERVAL_YEAR_TO_MONTH
               "      ,10\n"            // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,6\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_BINARY,
               "SELECT 'RAW'\n"         // TYPE_NAME
               "      ,-2\n"            // DATA_TYPE = SQL_BINARY
               "      ,2000\n"          // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,'max length'\n"  // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = TRUE
               "      ,3\n"             // SEARCHABLE = SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'RAW'\n"         // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-2\n"            // SQL_DATA_TYPE = SQL_BINARY
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_C_VARBOOKMARK,
               "SELECT 'ROWID'\n"       // TYPE_NAME
               "      ,-2\n"            // DATA_TYPE = SQL_VARBOOKMARK
               "      ,18\n"            // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,0\n"             // NULLABLE = SQL_NOT_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = FALSE
               "      ,1\n"             // SEARCHABLE = SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'ROWID'\n"       // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-2\n"            // SQL_DATA_TYPE = SQL_VARBOOKMARK
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_LONGVARBINARY,
               "SELECT 'LONG RAW'\n"    // TYPE_NAME
               "      ,-4\n"            // DATA_TYPE = SQL_LONGVARCHAR
               "      ,2147483647\n"    // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = TRUE
               "      ,0\n"             // SEARCHABLE = NOT SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'LONG RAW'\n"    // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-4\n"            // SQL_DATA_TYPE = SQL_LONGVARCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_LONGVARBINARY,
               "SELECT 'BFILE'\n"       // TYPE_NAME
               "      ,-4\n"            // DATA_TYPE = SQL_LONGVARCHAR
               "      ,2147483647\n"    // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,0\n"             // SEARCHABLE = NOT SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'BFILE'\n"       // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-4\n"            // SQL_DATA_TYPE = SQL_LONGVARCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_LONGVARCHAR,
               "SELECT 'CLOB'\n"        // TYPE_NAME
               "      ,-1\n"            // DATA_TYPE = SQL_LONGVARCHAR
               "      ,2147483647\n"    // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,0\n"             // SEARCHABLE = NOT SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'CLOB'\n"        // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-1\n"            // SQL_DATA_TYPE = SQL_LONGVARCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_WLONGVARCHAR,
               "SELECT 'NCLOB'\n"       // TYPE_NAME
               "      ,-10\n"           // DATA_TYPE = SQL_WLONGVARCHAR
               "      ,2147483647\n"    // COLUMN_SIZE (max)
               "      ,'N'''\n"         // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,0\n"             // SEARCHABLE = NOT SEARCHABLE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'NCLOB'\n"       // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-10\n"           // SQL_DATA_TYPE = SQL_WLONGVARCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_LONGVARBINARY,
               "SELECT 'BLOB'\n"        // TYPE_NAME
               "      ,-4\n"            // DATA_TYPE = SQL_LONGVARCHAR
               "      ,2147483647\n"    // COLUMN_SIZE (max)
               "      ,''''\n"          // LITERAL_PREFIX
               "      ,''''\n"          // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,1\n"             // CASE_SENSITIVE = TRUE
               "      ,0\n"             // SEARCHABLE = TRUE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'BLOB'\n"        // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,-4\n"            // SQL_DATA_TYPE = SQL_LONGVARCHAR
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,0\n"             // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_FLOAT,
               "SELECT 'BINARY_DOUBLE'\n" // TYPE_NAME
               "      ,6\n"             // DATA_TYPE = SQL_FLOAT
               "      ,15\n"            // COLUMN_SIZE (max)
               "      ,''\n"            // LITERAL_PREFIX
               "      ,''\n"            // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = FALSE
               "      ,3\n"             // SEARCHABLE = TRUE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'BINARY_DOUBLE'\n"   // LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,6\n"             // SQL_DATA_TYPE = SQL_FLOAT
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,10\n"            // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }
 ,{ META_TYPE, SQL_REAL,
               "SELECT 'BINARY_FLOAT'\n"// TYPE_NAME
               "      ,7\n"             // DATA_TYPE = SQL_FLOAT
               "      ,7\n"             // COLUMN_SIZE (max)
               "      ,''\n"            // LITERAL_PREFIX
               "      ,''\n"            // LITERAL_SUFFIX
               "      ,''\n"            // CREATE_PARAMS
               "      ,1\n"             // NULLABLE = SQL_NULLABLE
               "      ,0\n"             // CASE_SENSITIVE = FALSE
               "      ,3\n"             // SEARCHABLE = TRUE
               "      ,0\n"             // UNSIGNED = FALSE
               "      ,0\n"             // FIXED_PRED_SCALE = FALSE
               "      ,0\n"             // AUTO_UNIQUE_VALUE = FALSE
               "      ,'BINARY_FLOAT'\n"// LOCAL_TYPE_NAME
               "      ,0\n"             // MINIMUM_SCALE = 0
               "      ,0\n"             // MAXIMUM_SCALE = 0
               "      ,7\n"             // SQL_DATA_TYPE = SQL_FLOAT
               "      ,0\n"             // SQL_DATETIME_SUB
               "      ,10\n"            // NUM_PREC_RADIX = 0
               "      ,0\n"             // INTERVAL_PRECISION
               "  FROM DUAL" }

  // TABLES
 ,{ META_TABLE, 0,
               "SELECT CASE '@@CATALOG'\n"
               "            WHEN '@' THEN (SELECT UPPER(value)\n"
               "                             FROM v$parameter\n"
               "                            WHERE name = 'db_name')\n"
               "            ELSE '@CATALOG'\n"
               "            END  as table_cat\n"
               "      ,o.owner       as table_schem\n"
               "      ,o.object_name as table_name\n"
               "      ,o.object_type as table_type\n"
//               "      ,''            as remarks\n"
               "      ,(SELECT m.comments\n"
               "          FROM all_tab_comments@CATALOG m\n"
               "         WHERE o.owner       = m.owner\n"
               "           and o.object_name = m.table_name )  as remarks\n"
               "  FROM all_objects@CATALOG o\n"
               " WHERE o.object_type IN ('TABLE','VIEW','SYNONYM')" } 
 ,{ META_CATALOGS, 1,
               "SELECT Upper(Value) as table_cat\n"
               "      ,''           as table_schem\n"
               "      ,''           as table_name\n"
               "      ,''           as table_type\n"
               "      ,'Database'   as remarks\n"
               "  FROM v$parameter o\n"
               " WHERE name = 'db_name'\n" }
 ,{ META_CATALOGS, 2,
               "SELECT db_link as table_cat\n"
               "      ,''      as table_schem\n"
               "      ,''      as table_name\n"
               "      ,''      as table_type\n"
               "      ,'Other database' as remarks\n"
               "  FROM dba_db_links" }
 ,{ META_SCHEMAS, 0,
               "SELECT ''            as table_cat\n"
// Querying of schemata in different catalogs is not supported by ODBC!
//                "SELECT CASE '@@CATALOG'\n"
//                "            WHEN '@' THEN (SELECT UPPER(value)\n"
//                "                             FROM v$parameter\n"
//                "                            WHERE name = 'db_name')\n"
//                "            ELSE '@CATALOG'\n"
//                "            END  as table_cat\n"
               "      ,o.username    as table_schem\n"
               "      ,''            as table_name\n"
               "      ,''            as table_type\n"
               "      ,'User schema' as remarks\n"
               "  FROM all_users o" }
 ,{ META_TABLETYPES, 1,
               "SELECT ''            as table_cat\n"
               "      ,''            as table_schem\n"
               "      ,''            as table_name\n"
               "      ,'TABLE'       as table_type\n"
               "      ,''            as remarks\n"
               "  FROM DUAL" }
 ,{ META_TABLETYPES, 2,
               "SELECT ''            as table_cat\n"
               "      ,''            as table_schem\n"  // SCHEMA
               "      ,''            as table_name\n"   // NAME
               "      ,'VIEW'        as table_type\n"   // TYPE
               "      ,''            as remarks\n"      // REMARKS
               "  FROM DUAL" }
 ,{ META_TABLETYPES, 3,
               "SELECT ''            as table_cat\n"
               "      ,''            as table_schem\n"  // SCHEMA
               "      ,''            as table_name\n"   // NAME
               "      ,'SYNONYM'     as table_type\n"   // TYPE
               "      ,''            as remarks\n"      // REMARKS
               "  FROM DUAL" }
 ,{ META_COLUMNS, 1,
               "SELECT CASE '@@CATALOG'\n"
               "            WHEN '@' THEN (SELECT UPPER(value)\n"
               "                             FROM v$parameter\n"
               "                            WHERE name = 'db_name')\n"
               "            ELSE '@CATALOG'\n"
               "            END  as table_cat\n"
               "      ,c.owner         as table_schem\n"
               "      ,c.table_name    as table_name\n"
               "      ,c.column_name   as column_name\n"
               "      ,decode(SubStr(c.data_type,1,5)\n"
               "            ,'VARCH',     12\n"
               "            ,'NVARC',     -9\n"
               "            ,'NUMBE',      3\n"
               "            ,'FLOAT',      8\n"
               "            ,'LONG',      -1\n"
               "            ,'NCHAR',decode(c.data_type,'NCHAR',-8,'NCHAR VARYING',-9)\n"
               "            ,'DATE',      11\n"
               "            ,'RAW',       -3\n"
               "            ,'LONG ',     -4\n"
               "            ,'CHAR',       1\n"
               "            ,'NCLOB',    -10\n"
               "            ,'CLOB',      -1\n"
               "            ,'BLOB',      -4\n"
               "            ,'TIME(',     92\n"
               "            ,'TIMES',     93\n"
               "            ,'INTER',Decode(SubStr(c.data_type,1,12)\n"
               "                          ,'INTERVAL YEA',107,'INTERVAL DAY',110)\n"
               "            ,0)        as data_type\n"
               "      ,c.data_type     as type_name\n"
               "      ,decode(data_type\n"
               "             ,'DATE',16\n"
               "             ,'FLOAT',8\n"
               "             ,'LONG RAW',2147483647\n"
               "             ,'LONG',    2147483647\n"
               "             ,'CLOB',    2147483647\n"
               "             ,'BLOB',    2147483647\n"
               "             ,'BFILE',   2147483647\n"
               "             ,'NUMBER',NVL(data_precision+2,40)\n"
               "              ,data_length) as column_size\n"
               "      ,c.data_length   as buffer_length\n"
               "      ,c.data_scale    as decimal_digits\n"
               "      ,CASE c.data_type\n"
               "            WHEN 'FLOAT'  THEN  2\n"
               "            WHEN 'NUMBER' THEN 10\n"
               "            ELSE  0  END as num_prec_radix\n"
               "      ,CASE c.nullable\n"
               "            WHEN 'Y' THEN 1\n"
               "            ELSE  0  END as nullable\n"
               "      ,(SELECT m.comments\n"
               "          FROM all_col_comments m\n"
               "         WHERE c.owner       = m.owner\n"
               "           AND c.table_name  = m.table_name\n"
               "           AND c.column_name = m.column_name ) as remarks\n"
               "      ,''            as column_def\n"
               "      ,0             as sql_data_type\n"
               "      ,CASE SubStr(c.data_type,1,12)\n"
               "            WHEN 'INTERVAL YEA' THEN 7\n"
               "            WHEN 'INTERVAL DAY' THEN 10\n"
               "            ELSE 0 END as sql_datatime_sub\n"
               "      ,c.char_col_decl_length as char_octet_length\n"
               "      ,c.column_id       as ordinal_position\n"
               "      ,CASE c.nullable\n"
               "            WHEN 'Y' THEN 1\n"
               "            ELSE  0  END as is_nullable\n"
               "  FROM all_tab_columns@CATALOG  c\n"
               " WHERE 1 = 1" }
 ,{ META_PRIMARYS, 0,
               "SELECT CASE '@@CATALOG'\n"
               "            WHEN '@' THEN (SELECT UPPER(value)\n"
               "                             FROM v$parameter\n"
               "                            WHERE name = 'db_name')\n"
               "            ELSE '@CATALOG'\n"
               "            END  as table_cat\n"
               "      ,c.owner   as table_schem\n"
               "      ,c.table_name\n"
               "      ,i.column_name\n"
               "      ,i.column_position as key_seq\n"
               "      ,c.constraint_name as pk_name\n"
               "  FROM all_constraints@CATALOG c\n"
               "      ,all_ind_columns@CATALOG i\n"
               "      ,all_objects@CATALOG o\n"
               " WHERE c.table_name      = i.table_name\n"
               "   AND c.owner           = i.table_owner\n"
               "   AND o.object_name     = i.table_name\n"
               "   AND o.owner           = i.table_owner\n"
               "   AND c.index_name      = i.index_name\n"
               "   AND c.constraint_type = 'P'\n"
               "   AND o.object_type     = 'TABLE'" }
 ,{ META_FOREIGNS, 0,
               "SELECT CASE '@@CATALOG'\n"
               "            WHEN '@' THEN (SELECT UPPER(value)\n"
               "                             FROM v$parameter\n"
               "                            WHERE name = 'db_name')\n"
               "            ELSE '@CATALOG'\n"
               "            END       as pktable_cat\n"
               "      ,pk.owner       as pktable_schem\n"
               "      ,pk.table_name  as pktable_name\n"
               "      ,pc.column_name as pkcolumn_name\n"
               "      ,NULL           as fktable_cat\n"
               "      ,fk.owner       as fktable_schem\n"
               "      ,fk.table_name  as fktable_name\n"
               "      ,fc.column_name as fkcolumn_name\n"
               "      ,pc.position    as key_seq\n"
               "      ,NULL           as update_rule\n"
               "      ,decode(fk.delete_rule, 'CASCADE', 0, 'NO ACTION', 3) as delete_rule\n"
               "      ,fk.constraint_name as fk_name\n"
               "      ,pk.constraint_name as pk_name\n"
               "      ,Decode(fk.deferred,'IMMEDIATE',1,0) as deferability\n"
               "  FROM all_constraints@CATALOG  pk\n"
               "      ,all_cons_columns@CATALOG pc\n"
               "      ,all_constraints@CATALOG  fk\n"
               "      ,all_cons_columns@CATALOG fc\n"
               " WHERE pk.owner           = pc.owner\n"
               "   and pk.constraint_name = pc.constraint_name\n"
               "   and fk.owner           = fc.owner\n"
               "   and fk.constraint_name = fc.constraint_name\n"
               "   and pk.constraint_type ='P'\n"
               "   and fk.constraint_type ='R'\n"
               "   and pk.constraint_name = fk.r_constraint_name\n"
               "   and pk.owner           = fk.owner\n"
               "   and pc.position        = fc.position\n" }
 ,{ META_STATISTICS, 1,
               "SELECT ''               as table_cat\n"
               "      ,o.owner          as table_schem\n"
               "      ,o.table_name     as table_name\n"
               "      ,TO_NUMBER(NULL)  as non_unique\n"
               "      ,''               as index_qualifier\n"
               "      ,''               as index_name\n"
               "      ,0                as type\n"
               "      ,TO_NUMBER(NULL)  as ordinal_position\n"
               "      ,''               as column_name\n"
               "      ,''               as asc_or_desc\n"
               "      ,num_rows         as cardinality\n"
               "      ,blocks           as pages\n"
               "      ,NULL             as filter_condition\n"
               "  FROM all_tables o\n"
               " WHERE 1 = 1" }
 ,{ META_STATISTICS, 2,
               "SELECT ''\n"
               "      ,o.table_owner\n"
               "      ,o.table_name\n"
               "      ,decode(o.uniqueness,'UNIQUE',0,1)\n"
               "      ,o.owner\n"
               "      ,o.index_name\n"
               "      ,3\n"
               "      ,b.column_position\n"
               "      ,b.column_name\n"
               "      ,'A'\n"
               "      ,o.distinct_keys\n"
               "      ,o.leaf_blocks\n"
               "      ,NULL\n"
               "  FROM all_indexes o\n"
               "      ,all_ind_columns b\n"
               " WHERE o.owner      = b.index_owner\n"
               "   AND o.index_name = b.index_name\n" }
 ,{ META_STATISTICS, 3,
               "SELECT ''\n"
               "      ,c.table_owner\n"
               "      ,c.table_name\n"
               "      ,decode(o.uniqueness,'UNIQUE',0,1)\n"
               "      ,o.owner\n"
               "      ,o.index_name\n"
               "      ,3\n"
               "      ,b.column_position\n"
               "      ,b.column_name\n"
               "      ,'A'\n"
               "      ,o.distinct_keys\n"
               "      ,o.leaf_blocks\n"
               "      ,NULL\n"
               "  FROM all_indexes o\n"
               "      ,all_ind_columns b\n"
               "      ,all_synonyms c\n"
               " WHERE c.table_name  = o.table_name\n"
               "   AND c.table_owner = o.table_owner\n"
               "   AND o.owner       = b.index_owner\n"
               "   AND o.index_name  = b.index_name" }
 ,{ META_SPECIAL, 1,
               "SELECT 2        as scope\n" 
               "      ,'ROWID'  as column_name\n" 
               "      ,1        as data_type\n"
               "      ,'ROWID'  as type_name\n"
               "      ,NULL     as column_size\n"
               "      ,18       as buffer_length\n"
               "      ,NULL     as decimal_digits\n"
               "      ,2        as pseudo_column\n"
               "  FROM all_tables@CATALOG o\n"
               " WHERE 1=1\n" }
 ,{ META_SPECIAL, 2,
               "SELECT 2\n"
               "      ,'ROWID'\n"
               "      ,1\n"
               "      ,'ROWID'\n"
               "      ,NULL\n"
               "      ,18\n"
               "      ,NULL\n"
               "      ,2\n"
               "  FROM all_tables@CATALOG o\n"
               "      ,all_synonyms@CATALOG s\n"
               " WHERE o.table_name = s.table_name\n" }
 ,{ META_TABLEPRIVS, 1, 
               "SELECT ''           as table_cat\n"
               "      ,b.owner      as table_schem\n"
               "      ,a.table_name\n"
               "      ,a.grantor\n"
               "      ,a.grantee\n"
               "      ,a.privilege\n"
               "      ,a.grantable  as is_grantable\n"
               "  FROM all_tab_privs a\n"
               "      ,all_tables b\n"
               " WHERE a.table_schema = b.owner\n"
               "   AND a.table_name   = b.table_name\n" } 

 ,{ META_TABLEPRIVS, 2,
               "SELECT ''\n"
               "       ,b.owner\n"
               "       ,b.synonym_name\n"
               "       ,p.grantor\n"
               "       ,p.grantee\n"
               "       ,p.privilege\n"
               "       ,p.grantable\n"
               "   FROM all_tab_privs p\n"
               "       ,all_tables t\n" 
               "       ,all_synonyms b\n" 
               "  WHERE b.table_name   = t.table_name\n" 
               "    AND b.table_owner  = t.owner\n" 
               "    AND p.table_schema = b.table_owner\n" 
               "    AND p.table_name   = b.table_name\n"  }
 ,{ META_TABLEPRIVS, 3, 
               "SELECT ''\n" 
               "      ,owner\n" 
               "      ,table_name\n" 
               "      ,owner\n"
               "      ,owner\n"
               "      ,'SELECT'\n" 
               "      ,'YES'\n" 
               "  FROM all_tables b\n" 
               " WHERE 1=1" }
 ,{ META_TABLEPRIVS, 4,
               "SELECT ''\n" 
               "      ,owner\n" 
               "      ,table_name\n" 
               "      ,owner\n"
               "      ,owner\n"
               "      ,'INSERT'\n" 
               "      ,'YES'\n" 
               "  FROM all_tables b\n" 
               " WHERE 1=1" }
 ,{ META_TABLEPRIVS, 5,
               "SELECT ''\n" 
               "      ,owner\n" 
               "      ,table_name\n" 
               "      ,owner\n"
               "      ,owner\n"
               "      ,'UPDATE'\n" 
               "      ,'YES'\n" 
               "  FROM all_tables b\n" 
               " WHERE 1=1" }
 ,{ META_TABLEPRIVS, 6,
               "SELECT ''\n" 
               "      ,owner\n" 
               "      ,table_name\n" 
               "      ,owner\n"
               "      ,owner\n"
               "      ,'DELETE'\n" 
               "      ,'YES'\n" 
               "  FROM all_tables b\n" 
               " WHERE 1=1" }
 ,{ META_TABLEPRIVS, 7,
               "SELECT ''\n" 
               "      ,owner\n" 
               "      ,table_name\n" 
               "      ,owner\n"
               "      ,owner\n"
               "      ,'REFERENCES'\n" 
               "      ,'YES'\n" 
               "  FROM all_tables b\n" 
               " WHERE 1=1" }
 ,{ META_COLUMNPRIVS, 1,
               "SELECT ''           as table_cat\n"
               "      ,b.owner      as table_schem\n"
               "      ,a.table_name\n"
               "      ,a.column_name\n"
               "      ,a.grantor\n"
               "      ,a.grantee\n"
               "      ,a.privilege\n"
               "      ,a.grantable  as is_grantable\n"
               "  FROM all_col_privs a\n"
               "     , all_tab_columns b\n"
               " WHERE a.table_schema = b.owner\n"
               "   AND a.table_name   = b.table_name\n"
               "   AND a.column_name  = b.column_name\n" }
 ,{ META_COLUMNPRIVS, 2,
               "SELECT ''\n"
               "      ,b.owner\n"
               "      ,b.synonym_name\n"
               "      ,p.column_name\n"
               "      ,p.grantor\n"
               "      ,p.grantee\n"
               "      ,p.privilege\n"
               "      ,p.grantable\n"
               "  FROM all_col_privs p\n"
               "      ,all_tab_columns c\n"
               "      ,all_synonyms b\n"
               " WHERE b.table_name   = c.table_name\n"
               "   AND b.table_owner  = c.owner\n"
               "   AND p.table_schema = b.table_owner\n"
               "   AND p.table_name   = b.table_name\n"
               "   AND p.column_name  = c.column_name\n" }
 ,{ META_COLUMNPRIVS, 3,
               "SELECT ''\n"
               "      ,b.owner\n"
               "      ,b.table_name\n"
               "      ,b.column_name\n"
               "      ,b.owner\n"
               "      ,b.owner\n"
               "      ,'SELECT'\n"
               "      ,'YES'\n"
               "  FROM all_tab_columns b\n"
               " WHERE 1=1\n" } 
 ,{ META_COLUMNPRIVS, 4,
               "SELECT ''\n"
               "      ,b.owner\n"
               "      ,b.table_name\n"
               "      ,b.column_name\n"
               "      ,b.owner\n"
               "      ,b.owner\n"
               "      ,'UPDATE'\n"
               "      ,'YES'\n"
               "  FROM all_tab_columns b\n"
               " WHERE 1=1\n" } 
 ,{ META_COLUMNPRIVS, 5,
               "SELECT ''\n"
               "      ,b.owner\n"
               "      ,b.table_name\n"
               "      ,b.column_name\n"
               "      ,b.owner\n"
               "      ,b.owner\n"
               "      ,'REFERENCES'\n"
               "      ,'YES'\n"
               "  FROM all_tab_columns b\n"
               " WHERE 1=1\n" } 
 ,{ META_PROCEDURES, 1,
               "SELECT CASE '@@CATALOG'\n"
               "            WHEN '@' THEN (SELECT UPPER(value)\n"
               "                             FROM v$parameter\n"
               "                            WHERE name = 'db_name')\n"
               "            ELSE '@CATALOG'\n"
               "            END  as procedure_cat\n"
               "      ,b.owner   as procedure_schem\n"
               "      ,decode (b.object_type,'PACKAGE',CONCAT(CONCAT(b.object_name,'.'), a.object_name)\n"
               "              ,b.object_name) as procedure_name\n"
               "      ,(SELECT Count(*)\n"
               "          FROM all_arguments a\n"
               "         WHERE a.object_id = b.object_id\n"
               "           AND a.position  > 0\n"
               "           AND a.in_out IN ('IN','IN/OUT'))    as num_input_params\n"
               "      ,(SELECT Count(*)\n"
               "          FROM all_arguments a\n"
               "         WHERE a.object_id = b.object_id\n"
               "           AND a.position  > 0\n"
               "           AND a.in_out IN ('OUT','IN/OUT'))   as num_output_params\n"
               "      ,(SELECT Count(*)\n"
               "          FROM all_arguments a\n"
               "         WHERE a.object_id = b.object_id\n"
               "           AND a.position  > 0\n"
               "           AND a.data_type = ('PL/SQL TABLE')) as num_result_sets\n"
               "      ,NULL    as remarks\n"
               "      ,decode (b.object_type,'PACKAGE',decode(a.position,0,2,1,1,0),decode(b.object_type,'PROCEDURE',1,'FUNCTION',2,0)) as procedure_type\n"
               "  FROM all_arguments@CATALOG a\n"
               "      ,all_objects@CATALOG b\n"
               " WHERE ( b.object_type = 'PROCEDURE' OR\n"
               "         b.object_type = 'FUNCTION'  OR\n"
               "         b.object_type = 'PACKAGE'   )\n"
               "   AND b.object_id = a.object_id\n"
               "   AND a.sequence  = 1\n" }
 ,{ META_PROCCOLUMNS, 1,
               "SELECT CASE '@@CATALOG'\n"
               "            WHEN '@' THEN (SELECT UPPER(value)\n"
               "                             FROM v$parameter\n"
               "                            WHERE name = 'db_name')\n"
               "            ELSE '@CATALOG'\n"
               "            END  as procedure_cat\n"
               "      ,a.owner   as procedure_schem\n"
               "      ,decode (b.object_type,'PACKAGE',CONCAT(CONCAT(b.object_name,'.'),a.object_name),b.object_name) as procedure_name\n"
               "      ,decode(a.position,0,'RETURN_VALUE',a.argument_name) as colunn_name\n"
               "      ,decode(a.position,0,5,decode(a.in_out,'IN',1,'IN/OUT',2,'OUT',4)) as column_type\n"
               "      ,decode(SubStr(a.data_type,1,5)\n"
               "            ,'VARCH',     12\n"
               "            ,'NVARC',     -9\n"
               "            ,'NUMBE',      3\n"
               "            ,'FLOAT',      8\n"
               "            ,'LONG',      -1\n"
               "            ,'NCHAR',decode(a.data_type,'NCHAR',-8,'NCHAR VARYING',-9)\n"
               "            ,'DATE',      11\n"
               "            ,'RAW',       -3\n"
               "            ,'LONG ',     -4\n"
               "            ,'CHAR',       1\n"
               "            ,'NCLOB',    -10\n"
               "            ,'CLOB',      -1\n"
               "            ,'BLOB',      -4\n"
               "            ,'TIME(',     92\n"
               "            ,'TIMES',     93\n"
               "            ,'INTER',Decode(SubStr(a.data_type,1,12)\n"
               "                          ,'INTERVAL YEA',107,'INTERVAL DAY',110)\n"
               "            ,0)         as data_type\n"
               "      ,a.data_type      as type_name\n"
               "      ,a.data_precision as column_size\n"
               "      ,a.data_length    as buffer_length\n"
               "      ,a.data_scale     as decimal_digits\n"
               "      ,a.radix          as num_prec_radix\n"
               "      ,2                as nullable\n"
               "      ,''               as remarks\n"
               "      ,''               as column_def\n"
               "      ,0                as sql_data_type\n"
               "      ,0                as sql_datetime_sub\n"
               "      ,''               as char_octet_length\n"
               "      ,a.position       as ordinal_position\n"
               "      ,''               as is_nullable\n"
               "  FROM all_arguments@CATALOG a\n"
               "      ,all_objects@CATALOG b\n"
               " WHERE (b.object_type = 'PROCEDURE' OR b.object_type = 'FUNCTION')\n"
               "   AND b.object_id  = a.object_id\n"
               "   AND a.data_level = 0" }
};

CString
GetMetaQuery(int p_type,int p_subtype,bool p_order)
{
  CString result;
  for(int n = 0; n < sizeof(metaQueries) / sizeof(metaQueries[0]); ++n)
  {
    if(metaQueries[n].m_type == p_type)
    {
      if(p_subtype == 0 || metaQueries[n].m_subType == p_subtype)
      {
        if(!result.IsEmpty())
        {
          result += "\nUNION\n";
        }
        result += metaQueries[n].m_query;
      }
    }
  }
  if(p_order)
  {
    result += "\nORDER BY 1";
  }
  return result;
}

CString
GetMetaQueryTables(int      p_type
                  ,bool     p_metaDataID
                  ,CString& p_server
                  ,CString& p_catalog
                  ,CString& p_schemas
                  ,CString& p_names
                  ,CString& p_types)
{
  CString query = GetMetaQuery(p_type,0,false);

  if(p_server == p_catalog || p_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_catalog = "";
  }
  query.Replace("@CATALOG",p_catalog);

  if(!p_schemas.IsEmpty())
  {
    query += "\n   AND o.owner " + GetMetaOperator(p_metaDataID,p_schemas) + " '" + p_schemas + "'";
  }
  if(!p_names.IsEmpty())
  {
    query += "\n   AND o.object_name " + GetMetaOperator(p_metaDataID,p_names) + " '" + p_names + "'";
  }
  if(!p_types.IsEmpty())
  {
    query += "\n   AND o.object_type " + GetMetaOperator(p_metaDataID,p_types) + " '" + p_types + "'";
  }
  query += "\n ORDER BY 4,2,3";
  return query;
}

CString
GetMetaQueryColumns(int      p_type
                   ,bool     p_metaDataID
                   ,CString& p_server
                   ,CString& p_catalog
                   ,CString& p_schemas
                   ,CString& p_names
                   ,CString& p_columns)
{
  CString query = GetMetaQuery(p_type,0,false);
  if(p_server == p_catalog || p_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_catalog = "";
  }
  query.Replace("@CATALOG",p_catalog);

  if(!p_schemas.IsEmpty())
  {
    query += "\n   AND c.owner " + GetMetaOperator(p_metaDataID,p_schemas) + " '" + p_schemas + "'";
  }
  if(!p_names.IsEmpty())
  {
    query += "\n   AND c.table_name " + GetMetaOperator(p_metaDataID,p_names) + " '" + p_names + "'";
  }
  if(!p_columns.IsEmpty())
  {
    query += "\n   AND c.column_name " + GetMetaOperator(p_metaDataID,p_columns) + " '" + p_columns + "'";
  }
  query += "\n ORDER BY c.owner,c.table_name,c.column_id";
  return query;
}


CString
GetMetaQueryForeign(bool     p_metaDataID
                   ,CString& p_server
                   ,CString& p_pk_catalog
                   ,CString& p_pk_schemas
                   ,CString& p_pk_tables
                   ,CString& /*p_fk_catalog*/
                   ,CString& p_fk_schemas
                   ,CString& p_fk_tables)
{
  CString query = GetMetaQuery(META_FOREIGNS,0,false);

  // We deliberately ignore the catalog of the foreign key
  // Oracle is not capable to do a primary/foreign relation
  // in more than one catalog. So they should always be the same
  if(p_server == p_pk_catalog || p_pk_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_pk_catalog = "";
  }
  query.Replace("@CATALOG",p_pk_catalog);

  if(!p_pk_schemas.IsEmpty())
  {
    query += "\n   AND pk.owner " + GetMetaOperator(p_metaDataID,p_pk_schemas) + " '" + p_pk_schemas + "'";
  }
  if(!p_pk_tables.IsEmpty())
  {
    query += "\n   AND pk.table_name " + GetMetaOperator(p_metaDataID,p_pk_tables) + " '" + p_pk_tables + "'";
  }
  if(!p_fk_schemas.IsEmpty())
  {
    query += "\n   AND fk.owner " + GetMetaOperator(p_metaDataID,p_fk_schemas) + " '" + p_fk_schemas + "'";
  }
  if(!p_fk_tables.IsEmpty())
  {
    query += "\n   AND fk.table_name " + GetMetaOperator(p_metaDataID,p_fk_tables) + " '" + p_fk_tables + "'";
  }
  query += "\n ORDER BY 2,3,9";
  return query;
}

CString
GetMetaQueryStatistics(bool     p_metaDataID
                      ,CString& p_server
                      ,CString& p_catalog
                      ,CString& p_schema
                      ,CString& p_table
                      ,int      p_unique)
{
  CString query;
  if(p_server == p_catalog || p_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_catalog = "";
  }
  // SQL_INDEX_UNIQUE or SQL_INDEX_ALL
  CString unique   = (p_unique == SQL_INDEX_UNIQUE) ? "\n   AND o.uniqueness = 'UNIQUE'" : "";
  CString schemas1 = "\n   AND o.owner "         + GetMetaOperator(p_metaDataID,p_schema) + " '" + p_schema + "'";
  CString schemas2 = "\n   AND o.table_owner "   + GetMetaOperator(p_metaDataID,p_schema) + " '" + p_schema + "'";
  CString tables   = "\n   AND o.table_name "    + GetMetaOperator(p_metaDataID,p_table)  + " '" + p_table  + "'";
  CString synonym  = "\n   AND c.synonym_name "  + GetMetaOperator(p_metaDataID,p_table)  + " '" + p_table  + "'";

  if(p_unique == SQL_INDEX_ALL)
  {
    // Get table part
    query = GetMetaQuery(META_STATISTICS,1,false);
    if(!p_schema.IsEmpty())
    {
      query += schemas1;
    }
    if(!p_table.IsEmpty())
    {
      query += tables;
    }
  }
  // Get statistics from table
  query += "\nUNION\n";
  query += GetMetaQuery(META_STATISTICS,2,false);
  if(!p_schema.IsEmpty())
  {
    query += schemas2;
  }
  if(!p_table.IsEmpty())
  {
    query += tables;
  }
  query += unique;

  // Get statistics from synonym
  query += "\nUNION\n";
  query += GetMetaQuery(META_STATISTICS,3,false);
  if(!p_schema.IsEmpty())
  {
    query += schemas2;
  }
  if(!p_table.IsEmpty())
  {
    query += tables;
  }
  query += unique;

  // General replacements
  query.Replace("@CATALOG",p_catalog);
  query = "(" + query + ")\nORDER BY 4,5,6,8";
  return query;
}

CString
GetMetaQuerySpecials(CString& p_server
                    ,CString& p_catalog
                    ,CString& p_schema
                    ,CString& p_table)
{
  CString query;
  CString query1 = GetMetaQuery(META_SPECIAL,1,false);
  CString query2 = GetMetaQuery(META_SPECIAL,2,false);

  if(p_server == p_catalog || p_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_catalog = "";
  }
  if(!p_schema.IsEmpty())
  {
    query1 += "\n   AND o.owner = '" + p_schema + "'";
    query2 += "\n   AND s.owner = '" + p_schema + "'";
  }
  if(!p_table.IsEmpty())
  {
    query1 += "\n   AND o.table_name = '"   + p_table + "'";
    query2 += "\n   AND s.synonym_name = '" + p_table + "'";
  }
  query = query1 + "\nUNION\n" + query2;
  query.Replace("@CATALOG",p_catalog);
  return query;
}

CString
GetMetaQueryTablePrivileges(bool     p_metaDataID
                           ,CString& p_server
                           ,CString& p_catalog
                           ,CString& p_schemas
                           ,CString& p_tables)
{
  CString query;
  CString owner1,owner2;
  CString table1,table2;
  CString synon2;
  CString query1 = GetMetaQuery(META_TABLEPRIVS,1,false);
  CString query2 = GetMetaQuery(META_TABLEPRIVS,2,false);
  CString query3 = GetMetaQuery(META_TABLEPRIVS,3,false);
  CString query4 = GetMetaQuery(META_TABLEPRIVS,4,false);
  CString query5 = GetMetaQuery(META_TABLEPRIVS,5,false);
  CString query6 = GetMetaQuery(META_TABLEPRIVS,6,false);
  CString query7 = GetMetaQuery(META_TABLEPRIVS,7,false);

  if(p_server == p_catalog || p_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_catalog = "";
  }

  if(!p_schemas.IsEmpty())
  {
    owner1 = "\n   AND b.owner "        + GetMetaOperator(p_metaDataID,p_schemas) + " '" + p_schemas + "'";
    owner2 = "\n   AND a.table_schema " + GetMetaOperator(p_metaDataID,p_schemas) + " '" + p_schemas + "'";
  }
  if(!p_tables.IsEmpty())
  {
    table1 = "\n   AND a.table_name "   + GetMetaOperator(p_metaDataID,p_tables) + " '" + p_tables + "'";
    table2 = "\n   AND b.table_name "   + GetMetaOperator(p_metaDataID,p_tables) + " '" + p_tables + "'";
    synon2 = "\n   AND b.synonym_name " + GetMetaOperator(p_metaDataID,p_tables) + " '" + p_tables + "'";
  }
  query = query1 + owner1 + owner2 + table1 + table2 + "\nUNION\n" +
          query2 + owner1 + synon2 + "\nUNION\n" +
          query3 + owner1 + table2 + "\nUNION\n" +
          query4 + owner1 + table2 + "\nUNION\n" +
          query5 + owner1 + table2 + "\nUNION\n" +
          query6 + owner1 + table2 + "\nUNION\n" +
          query7 + owner1 + table2;

  query.Replace("@CATALOG",p_catalog);
  return query;
}

CString
GetMetaQueryColumnPrivileges(bool     p_metaDataID
                            ,CString& p_server
                            ,CString& p_catalog
                            ,CString& p_schemas
                            ,CString& p_tables
                            ,CString& p_columns)
{
  CString query;
  CString owner1,owner2;
  CString table1,table2;
  CString column1,column2;
  CString synon2;
  CString query1 = GetMetaQuery(META_COLUMNPRIVS,1,false);
  CString query2 = GetMetaQuery(META_COLUMNPRIVS,2,false);
  CString query3 = GetMetaQuery(META_COLUMNPRIVS,3,false);
  CString query4 = GetMetaQuery(META_COLUMNPRIVS,4,false);
  CString query5 = GetMetaQuery(META_COLUMNPRIVS,5,false);

  if(p_server == p_catalog || p_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_catalog = "";
  }

  if(!p_schemas.IsEmpty())
  {
    owner1 = "\n   AND b.owner "        + GetMetaOperator(p_metaDataID,p_schemas) + " '" + p_schemas + "'";
    owner2 = "\n   AND a.table_schema " + GetMetaOperator(p_metaDataID,p_schemas) + " '" + p_schemas + "'";
  }
  if(!p_tables.IsEmpty())
  {
    table1 = "\n   AND a.table_name "   + GetMetaOperator(p_metaDataID,p_tables) + " '" + p_tables + "'";
    table2 = "\n   AND b.table_name "   + GetMetaOperator(p_metaDataID,p_tables) + " '" + p_tables + "'";
    synon2 = "\n   AND b.synonym_name " + GetMetaOperator(p_metaDataID,p_tables) + " '" + p_tables + "'";
  }
  if(!p_columns.IsEmpty())
  {
    column1 = "\n   AND a.column_name  " + GetMetaOperator(p_metaDataID,p_columns) + " '" + p_columns + "'";
    column2 = "\n   AND b.column_name  " + GetMetaOperator(p_metaDataID,p_columns) + " '" + p_columns + "'";
  }

  query = query1 + owner1 + owner2 + table1 + table2 + column1 + "\nUNION\n" + 
          query2 + owner1 + synon2 + column1 + "\nUNION\n" + 
          query3 + owner1 + table2 + column2 + "\nUNION\n" + 
          query4 + owner1 + table2 + column2 + "\nUNION\n" + 
          query5 + owner1 + table2 + column2;

  query.Replace("@CATALOG",p_catalog);
  return query;
}

CString 
GetMetaQueryProcedures(bool     p_metaDataID
                      ,CString& p_server
                      ,CString& p_catalog
                      ,CString& p_schemas
                      ,CString& p_procs)
{
  CString owner;
  CString procedure;
  CString query = GetMetaQuery(META_PROCEDURES,0,false);

  if(p_server == p_catalog || p_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_catalog = "";
  }
  if(!p_schemas.IsEmpty())
  {
    owner = "\n   AND b.owner " + GetMetaOperator(p_metaDataID,p_schemas) + " '" + p_schemas + "'";
  }
  if(!p_procs.IsEmpty())
  {
    procedure = "\n   AND b.object_name "   + GetMetaOperator(p_metaDataID,p_procs) + " '" + p_procs + "'";
  }
  query += owner + procedure;
  query.Replace("@CATALOG",p_catalog);
  query += "\nORDER BY 1,2,3";
  return query;
}

CString
GetMetaQueryProcColumns(bool     p_metaDataID
                       ,CString& p_server
                       ,CString& p_catalog
                       ,CString& p_schemas
                       ,CString& p_procs
                       ,CString& p_columns)
{
  CString owner,procs,cols;
  CString query = GetMetaQuery(META_PROCCOLUMNS,0,false);

  if(p_server == p_catalog || p_catalog.IsEmpty())
  {
    // If we are ourselves, don't use it
    p_catalog = "";
  }
  if(!p_schemas.IsEmpty())
  {
    owner = "\n   AND b.owner " + GetMetaOperator(p_metaDataID,p_schemas) + " '" + p_schemas + "'";
  }
  if(!p_procs.IsEmpty())
  {
    procs = "\n   AND b.object_name "   + GetMetaOperator(p_metaDataID,p_procs) + " '" + p_procs + "'";
  }
  if(!p_columns.IsEmpty())
  {
    cols = "\n   AND a.argument_name " + GetMetaOperator(p_metaDataID,p_columns) + " '" + p_columns + "'";
  }
  query += owner + procs + cols + "\nORDER BY 18";
  query.Replace("@CATALOG",p_catalog);
  return query;
}

CString 
GetMetaOperator(bool p_metaDataID,CString& p_tofind)
{
  if(p_metaDataID)
  {
    return CString("=");
  }
  int pos = p_tofind.FindOneOf("%_\\");
  if(pos >= 0)
  {
    return CString("LIKE");
  }
  return CString("=");
}