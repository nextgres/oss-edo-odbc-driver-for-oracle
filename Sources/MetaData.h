#pragma once

enum _meta_query_types
{
  META_TYPE         // For SQLGetTypeInfo
 ,META_TABLE        // For SQLGetTables -> Free format
 ,META_CATALOGS     // For SQLGetTables -> SQL_ALL_CATALOGS
 ,META_SCHEMAS      // For SQLGetTables -> SQL_ALL_SCHEMAS
 ,META_TABLETYPES   // For SQLGetTables -> SQL_ALL_TABLE_TYPES
 ,META_COLUMNS      // For SQLColumns
 ,META_PRIMARYS     // For SQLPrimaryKeys
 ,META_FOREIGNS     // For SQLForeignKeys
 ,META_STATISTICS   // For SQLStatistics
 ,META_SPECIAL      // For SQLSpecialColumns
 ,META_TABLEPRIVS   // For SQLTablePrivileges
 ,META_COLUMNPRIVS  // For SQLColumnPrivileges
 ,META_PROCEDURES   // For SQLProcedures
 ,META_PROCCOLUMNS  // For SQLProcedureColumns
};

typedef struct _metaQuery 
{
  int     m_type;
  int     m_subType;
  CString m_query;
}
MetaQuery;

extern MetaQuery metaQueries[];

CString GetMetaOperator    (bool p_metaDataID,CString& p_tofind);
CString GetMetaQuery       (int p_type,int p_subtype,bool p_order = true);
CString GetMetaQueryTables (int      p_type
                           ,bool     p_metaDataID
                           ,CString& p_server
                           ,CString& p_catalog
                           ,CString& p_schemas
                           ,CString& p_names
                           ,CString& p_types);
CString GetMetaQueryColumns(int      p_type
                           ,bool     p_metaDataID
                           ,CString& p_server
                           ,CString& p_catalog
                           ,CString& p_schemas
                           ,CString& p_names
                           ,CString& p_columns);
CString GetMetaQueryForeign(bool     p_metaDataID
                           ,CString& p_server
                           ,CString& p_pk_catalog
                           ,CString& p_pk_schemas
                           ,CString& p_pk_tables
                           ,CString& p_fk_catalog
                           ,CString& p_fk_schemas
                           ,CString& p_fk_tables);
CString GetMetaQueryStatistics 
                           (bool     p_metaDataID
                           ,CString& p_server
                           ,CString& p_catalog
                           ,CString& p_schema
                           ,CString& p_table
                           ,int      p_unique);
CString GetMetaQuerySpecials(CString& p_server
                           ,CString& p_catalog
                           ,CString& p_schema
                           ,CString& p_table);
CString
GetMetaQueryTablePrivileges(bool     p_metaDataID
                           ,CString& p_server
                           ,CString& p_catalog
                           ,CString& p_schemas
                           ,CString& p_tables);
CString
GetMetaQueryColumnPrivileges(bool    p_metaDataID
                           ,CString& p_server
                           ,CString& p_catalogs
                           ,CString& p_schemas
                           ,CString& p_tables
                           ,CString& p_columns);
CString 
GetMetaQueryProcedures     (bool     p_metaDataID
                           ,CString& p_server
                           ,CString& p_catalogs
                           ,CString& p_schemas
                           ,CString& p_procs);
CString
GetMetaQueryProcColumns    (bool     p_metaDataID
                           ,CString& p_server
                           ,CString& p_catalogs
                           ,CString& p_schemas
                           ,CString& p_procs
                           ,CString& p_columns);
