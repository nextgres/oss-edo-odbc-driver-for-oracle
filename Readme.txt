EDO ORACLE ODBC DRIVER
======================

The Oracle ODBC driver has a number of limitations that are totally
unnecessary and rather a pity. That's where this total rewrite of the
ODBC driver comes in.



Here is a recording of alll bugs that I found in the
Oracle ODBC implementation, shipped from www.oracle.com
to give you a quick impression of the differences.
(for complete documentation, see the accompanying *.CHM
=======================================================

Oracle's binding to NUMBER
--------------------------
This is the main issue why I started with this ODBC driver!!
Whenever Oracle's ODBC driver gets you a number from their database
it converts the number always first to a C++ double type before it
does anything else with it. This knocks of a whole lot of precision
from the number. NUMBER itself is basically "NUMBER(38,38)" in SQL terms.
But it converts it to a "double" with 15 decimals first.
Even, and this is the main issue, if we bind to a NUMBER as a "CHAR" or
"VARCHAR" type.
So if my application has an internal numeric type with a greater precision
than 15 (which I have) and even if I bind a column for a query directly 
as a char or varchar to a NUMBER it doesn't work. Oracle always truncates
the decimals 16 and higher from the number.

Oracle's implementation of SQLGetTypeInfo
-----------------------------------------
Oracle's implmentation of SQLGetTypeInfo does never return the SQL datatype.
The fact is that their own information schema never ever reveals the true
datatype. Only a derived column in varchar format that is not exactly the
true technical datatype.
A way to correct this is with a pretty long 'decode' sequence to program
back to the real odbc data type.

Oracle's implementation of catalog info
---------------------------------------
In the Info routines for the RDBMS catalog, oracle always assumes the 
current database to be your objective (current catalog). This is a bit
akward, because it is pretty simple to get information about other catalogs
by way of the DBLINK functionality.
All catalog functions like 
- SQLTables
- SQLColumns
- SQLPrimaryKeys
- SQLForeignKeys
- SQLStatistics
- SQLSpecialColumns
- SQLTablePrivileges
- SQLColumnPrivileges
- SQLProcedures
- SQLProcedureColumns
support in essence the possibility to ask these things about other catalogs
because all first parameters are always "catalog", "schema". The oracle 
implementation of the ODBC driver always returns an empty string ('') for
the first column of the result set (TABLE_CATALOG).

The way to implement this right is in two steps:
1) store the name of the current database at login time.
2) Alter the information_schema queries on the database to reflect this 
   database (stored) and the names of the links from "all_db_link"

(SQLProcedures contains a bug which has the efect that all procedures are
reported twice to the driver).

Performance of the catalog info
-------------------------------
Better performance in terms of buffer gets
1) Primary keys are not always filled, but are partly on linked tables
2) Old style Oracle 7&8 catalog types are supported in the official driver
   I can drop that to make it perform faster.

SQLColumns              242.000 -> 15.000 (  15 times faster)
SQLProcedures			140.000 ->     34 (4000 times faster)
SQLProcedureColumns		 27.000 ->     50 ( 500 times faster)

Reporting of the ODBC_SQL datatype in the catalog functions
-----------------------------------------------------------
Not one of the catalog functions reports the true ODBC SQL Datatype.
This is a bit of a bear to implement, as the ALL_* views of oracle
only report the character version of the datatype. But it is not
impossible to implement. EDO now reports all SQL datatypes.

SQLGetInfo
----------
Oracle returns strange values for catalog support.
- Name = 'Database link'
  separator = '@'
  supported = 'no' ??????
  So make up your mind. Do you or do you not support catalogs!!
- Oracle does not get the Collation seq (characterset) 
  It's only two round trips to OCI without a server roundtrip
- Oracle reports that it does not support global temporary tables??
- Oracle report different numeric functions that it does support.
  asin, acos, atan, atan2, round
- Capabilities for REVOKE are not reported!
- TRIM (leading, trailing, both) are not reported
- case, cast, coalesce, nullif are not reported
- current_date, current_timestamp, extract are not reported

SQLStatistics
-------------
Oracle does **not** support SQL_ENSURE/SQL_QUICK for the cardinality
and pages information of indexes. So it does always retrieve with
SQL_QUICK and the statistics are not regenerated.
SQL_ENSURE is very easy to build (put an ANALYZE in between).
But should we? The ANALYZE statement is reported to be broken on
Oracle 10 and above. So we make it a driver option.

NOT USED
--------
Shared mode for OCI connections is not used in the ODBC driver
This is a pity, and a missed oppportunity.

SQLNativeSql
------------
The Oracle ODBC driver does not implement the following escapes
- { call procedure_name[(par,...)] }
- { interval 'literal' TYPE (n) TO TYPE }

BLOB/CLOB
---------
Oracle's implementation of this datatype as binding to a binary format
does not work: error "Incorrect LOB locator specified".
The only way to insert a LOB is by binding it to a char field.
But data streaming is not possible in this way. Only static buffers can be inserted!!
(Fixed in Oracle 10g!)

My solution:
Creating a temporary blob with "OCILobCreateTemporary". Data streams to this LOB
with the SQL_NEED_DATA/SQLPutData interface.
This temporary lob gets binded to the INSERT statement and then executed
et voila: the complete LOB goes into the database row!
It works for Oracle 8/9 as well as for 10/11.

Piecewise must use the OCIStmtGetPieceInfo/OCIStmtSetPieceInfo interface
This does work for all datatypes CHAR,VARCHAR2,RAW,LONG,LONG RAW,CLOB,BLOB,NCLOB
