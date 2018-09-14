SELECT view_name FROM all_views
WHERE view_name like '%COMMENT%'
;



select u.name AS owner
      ,o.name AS table_name
      ,c.name AS column_name
      ,decode(c.type#, 1, decode(c.charsetform, 2, 'NVARCHAR2', 'VARCHAR2'),
                       2, decode(c.scale, null,decode(c.precision#, null, 'NUMBER', 'FLOAT'),'NUMBER'),
                       8, 'LONG',
                       9,  decode(c.charsetform, 2, 'NCHAR VARYING', 'VARCHAR'),
                       12, 'DATE',
                       23, 'RAW',
                       24, 'LONG RAW',
                       58, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       69, 'ROWID',
                       96, decode(c.charsetform, 2, 'NCHAR', 'CHAR'),
                       105, 'MLSLABEL',
                       106, 'MLSLABEL',
                       111, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       112, decode(c.charsetform, 2, 'NCLOB', 'CLOB'),
                       113, 'BLOB',
                       114, 'BFILE',
                       115, 'CFILE',
                       121, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       122, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       123, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       178, 'TIME(' ||c.scale|| ')',
                       179, 'TIME(' ||c.scale|| ')' || ' WITH TIME ZONE',
                       180, 'TIMESTAMP(' ||c.scale|| ')',
                       181, 'TIMESTAMP(' ||c.scale|| ')' || ' WITH TIME ZONE',
                       231, 'TIMESTAMP(' ||c.scale|| ')' || ' WITH LOCAL TIME ZONE',
                       182, 'INTERVAL YEAR(' ||c.precision#||') TO MONTH',
                       183, 'INTERVAL DAY(' ||c.precision#||') TO SECOND(' || c.scale || ')',
                       208, 'UROWID', 'UNDEFINED') AS data_type
      ,decode(c.type#, 111, 'REF') AS data_type_mod
      ,nvl2(ac.synobj#, (select u.name
                           from sys.user$ u
                               ,sys.obj$ o
                          where o.owner# = u.user#
                            and o.obj#=ac.synobj#), ut.name) AS data_type_owner
      ,c.Length     AS data_type_length
      ,c.precision# AS data_precision
      ,c.scale      AS data_scale
      ,decode(sign(c.null$),-1,'D', 0, 'Y', 'N') AS nullable
      ,decode(c.col#, 0, to_number(null), c.col#) AS column_id
      ,c.deflength  AS default_length
      ,c.default$   AS data_default
      ,h.distcnt    AS num_distinct
      ,h.lowval     AS low_value
      ,h.hival      AS high_value
      ,h.density    AS density
      ,h.null_cnt   AS num_nulls
      ,decode(h.row_cnt, 0, 1, 1, 1, h.row_cnt-1) AS num_buckets
      ,h.timestamp# AS last_analyzed
      ,h.sample_size  AS sample_size
      ,decode(c.charsetform, 1, 'CHAR_CS',
                             2, 'NCHAR_CS',
                             3, NLS_CHARSET_NAME(c.charsetid),
                             4, 'ARG:'||c.charsetid) AS character_set_name
      ,decode(c.charsetid, 0, to_number(NULL),nls_charset_decl_len(c.length, c.charsetid)) AS char_col_decl_len
      ,decode(bitand(h.spare2, 2), 2, 'YES', 'NO') AS global_stats
      ,decode(bitand(h.spare2, 1), 1, 'YES', 'NO') AS user_stats
      ,h.avgcln   AS avg_col_len
      ,c.spare3   AS char_length
      ,decode(c.type#, 1, decode(bitand(c.property, 8388608), 0, 'B', 'C'),
                      96, decode(bitand(c.property, 8388608), 0, 'B', 'C'),
                      null) AS char_used
      ,decode(bitand(ac.flags, 128), 128, 'YES', 'NO') AS v80_fmt_image
      ,decode(o.status, 1, decode(bitand(ac.flags, 256), 256, 'NO', 'YES'),
              decode(bitand(ac.flags, 2), 2, 'NO',
              decode(bitand(ac.flags, 4), 4, 'NO',
              decode(bitand(ac.flags, 8), 8, 'NO','N/A')))) AS data_upgraded
      ,decode(c.property, 0, 'NO', decode(bitand(c.property, 32), 32, 'YES','NO'))
      ,decode(c.property, 0, 'NO', decode(bitand(c.property,  8),  8, 'YES','NO'))
      ,decode(c.segcol#, 0, to_number(null), c.segcol#)
      ,c.intcol#
  from sys.col$ c
      ,sys.obj$ o
      ,sys.hist_head$ h
      ,sys.user$ u
      ,sys.coltype$ ac
      ,sys.obj$ ot
      ,sys.user$ ut
 where o.obj#   = c.obj#
   and o.owner# = u.user#
   and c.obj#   = h.obj#(+)  and c.intcol# = h.intcol#(+)
   and c.obj#   = ac.obj#(+) and c.intcol# = ac.intcol#(+)
   and ac.toid  = ot.oid$(+)
   and ot.type#(+) = 13
   and ot.owner# = ut.user#(+)
   and o.type# in (2, 3, 4)  -- tables, clusters, views

   AND u.name = 'MDBA'
   AND o.name = 'KBS_CT'
;

-- FOR ODBC

select u.name AS owner
      ,o.name AS table_name
      ,c.name AS column_name
      ,decode(c.type#, 1, decode(c.charsetform, 2, 'NVARCHAR2', 'VARCHAR2'),
                       2, decode(c.scale, null,decode(c.precision#, null, 'NUMBER', 'FLOAT'),'NUMBER'),
                       8, 'LONG',
                       9,  decode(c.charsetform, 2, 'NCHAR VARYING', 'VARCHAR'),
                       12, 'DATE',
                       23, 'RAW',
                       24, 'LONG RAW',
                       58, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       69, 'ROWID',
                       96, decode(c.charsetform, 2, 'NCHAR', 'CHAR'),
                       105, 'MLSLABEL',
                       106, 'MLSLABEL',
                       111, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       112, decode(c.charsetform, 2, 'NCLOB', 'CLOB'),
                       113, 'BLOB',
                       114, 'BFILE',
                       115, 'CFILE',
                       121, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       122, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       123, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
                       178, 'TIME(' ||c.scale|| ')',
                       179, 'TIME(' ||c.scale|| ')' || ' WITH TIME ZONE',
                       180, 'TIMESTAMP(' ||c.scale|| ')',
                       181, 'TIMESTAMP(' ||c.scale|| ')' || ' WITH TIME ZONE',
                       231, 'TIMESTAMP(' ||c.scale|| ')' || ' WITH LOCAL TIME ZONE',
                       182, 'INTERVAL YEAR(' ||c.precision#||') TO MONTH',
                       183, 'INTERVAL DAY(' ||c.precision#||') TO SECOND(' || c.scale || ')',
                       208, 'UROWID', 'UNDEFINED') AS data_type
      ,decode(c.type#, 111, 'REF') AS data_type_mod
      ,c.Length     AS data_type_length
      ,c.precision# AS data_precision
      ,c.scale      AS data_scale
      ,decode(sign(c.null$),-1,'D', 0, 'Y', 'N') AS nullable
      ,decode(c.col#, 0, to_number(null), c.col#) AS column_id
      ,c.deflength  AS default_length
      ,c.default$   AS data_default
      ,decode(c.charsetform, 1, 'CHAR_CS',
                             2, 'NCHAR_CS',
                             3, NLS_CHARSET_NAME(c.charsetid),
                             4, 'ARG:'||c.charsetid) AS character_set_name
      ,decode(c.charsetid, 0, to_number(NULL),nls_charset_decl_len(c.length, c.charsetid)) AS char_col_decl_len
      ,c.spare3   AS char_length
      ,decode(c.type#, 1, decode(bitand(c.property, 8388608), 0, 'B', 'C'),
                      96, decode(bitand(c.property, 8388608), 0, 'B', 'C'),
                      null) AS char_used
      ,decode(bitand(ac.flags, 128), 128, 'YES', 'NO') AS v80_fmt_image
      ,decode(o.status, 1, decode(bitand(ac.flags, 256), 256, 'NO', 'YES'),
              decode(bitand(ac.flags, 2), 2, 'NO',
              decode(bitand(ac.flags, 4), 4, 'NO',
              decode(bitand(ac.flags, 8), 8, 'NO','N/A')))) AS data_upgraded
      ,decode(c.property, 0, 'NO', decode(bitand(c.property, 32), 32, 'YES','NO'))
      ,decode(c.property, 0, 'NO', decode(bitand(c.property,  8),  8, 'YES','NO'))
      ,decode(c.segcol#, 0, to_number(null), c.segcol#)
      ,c.intcol#
  from sys.col$ c
      ,sys.obj$ o
      ,sys.user$ u
      ,sys.coltype$ ac
      ,sys.obj$ ot
 where o.obj#    = c.obj#
   and o.owner#  = u.user#
   and c.obj#    = ac.obj#(+)
   and c.intcol# = ac.intcol#(+)
   and ac.toid   = ot.oid$(+)
   and ot.type#(+) = 13
   and o.type# in (2, 3, 4)  -- tables, clusters, views
   AND u.name = 'MDBA'
   AND o.name = 'KBS_CT'
;

SELECT CASE '@@CATALOG'
          WHEN '@' THEN (SELECT UPPER(value)
                            FROM v$parameter
                          WHERE name = 'db_name')
          ELSE '@CATALOG' END as table_cat
      ,username    as table_schem
      ,''            as table_name
      ,''            as table_type
      ,''            as remarks
  FROM all_users@dbs
;


-- ODBC SQLColumns

SELECT CASE '@'
          WHEN '@' THEN (SELECT UPPER(value)
                            FROM v$parameter
                          WHERE name = 'db_name')
          ELSE '' END as table_cat
      ,u.name                 as table_schem
      ,o.name                 AS table_name
      ,c.name                 AS column_name
      ,c.type#                AS data_type
      ,decode(c.type#,
            1, decode(c.charsetform, 2, 'NVARCHAR2', 'VARCHAR2'),
            2, decode(c.scale, null,decode(c.precision#, null, 'NUMBER', 'FLOAT'),'NUMBER'),
            8, 'LONG',
            9, decode(c.charsetform, 2, 'NCHAR VARYING', 'VARCHAR'),
           12, 'DATE',
           23, 'RAW',
           24, 'LONG RAW',
           58, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
           69, 'ROWID',
           96, decode(c.charsetform, 2, 'NCHAR', 'CHAR'),
          105, 'MLSLABEL',
          106, 'MLSLABEL',
          111, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
          112, decode(c.charsetform, 2, 'NCLOB', 'CLOB'),
          113, 'BLOB',
          114, 'BFILE',
          115, 'CFILE',
          121, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
          122, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
          123, nvl2(ac.synobj#, (select o.name from sys.obj$ o where o.obj#=ac.synobj#), ot.name),
          178, 'TIME(' ||c.scale|| ')',
          179, 'TIME(' ||c.scale|| ')' || ' WITH TIME ZONE',
          180, 'TIMESTAMP(' ||c.scale|| ')',
          181, 'TIMESTAMP(' ||c.scale|| ')' || ' WITH TIME ZONE',
          231, 'TIMESTAMP(' ||c.scale|| ')' || ' WITH LOCAL TIME ZONE',
          182, 'INTERVAL YEAR(' ||c.precision#||') TO MONTH',
          183, 'INTERVAL DAY(' ||c.precision#||') TO SECOND(' || c.scale || ')',
          208, 'UROWID', 'UNDEFINED') AS type_name
      ,Decode(c.type#,
            1, c.Length,
            2, 45,
            8, c.Length,
            9, c.Length,
           12, 25,
           23, c.Length,
           24, c.Length,
           58, c.Length,
           69, 25,
          105, c.Length,
          106, c.Length,
          111, c.Length,
          112, c.Length,
          113, c.Length,
          114, c.Length,
          115, c.Length,
          121, c.Length,
          122, c.Length,
          123, c.Length,
          178, 12,
          179, 18,
          180, 25,
          181, 30,
          231, 30,
          182, 25,
          183, 25,
          208, 64,c.Length)  AS column_size
      ,c.Length              AS buffer_length
      ,c.scale               AS decimal_digits
      ,Decode(c.type#,2,2,0) AS num_prec_radix
      ,decode(sign(c.null$),-1,'D', 0, 'Y', 'N') AS nullable
      ,'' AS remarks
      ,'' AS column_def
      ,decode(c.type#,
            1, decode(c.charsetform, 2, -9, 12),
            2, decode(c.scale, null,decode(c.precision#, null, 3, 6),3),
            8, -1,
            9, decode(c.charsetform, 2, -9, 12),
           12, 11,
           23, -3,
           24, -4,
           58, 0,
           69, 0,
           96, decode(c.charsetform, 2, -8, 1),
          105, 0,
          106, 0,
          111, 0,
          112, decode(c.charsetform, 2, -10, -1),
          113, -4,
          114, 0,
          115, 0,
          121, 0,
          122, 0,
          123, 0,
          178, 10,
          179, 92,
          180, 11,
          181, 93,
          231, 93,
          182, 107,
          183, 110,
          208, 0, 0) AS sql_data_type
      ,decode(c.type#,
          178, 12, -- hour to second
          179, 12,
          182, 7,  -- year to month
          183, 10, -- day to second
          0) AS sql_datetime_sub
      ,decode(c.charsetid, 0, to_number(NULL),nls_charset_decl_len(c.length, c.charsetid)) AS char_octet_length
      ,decode(c.col#, 0, to_number(null), c.col#) AS ordinal_position
      ,decode(sign(c.null$),-1,'D', 0, 'Y', 'N') AS nullable
  FROM sys.user$ u
      ,sys.obj$ o
      ,sys.col$ c
      ,sys.coltype$ ac
      ,sys.obj$ ot
 where o.obj#    = c.obj#
   and o.owner#  = u.user#
   and c.obj#    = ac.obj#(+)
   and c.intcol# = ac.intcol#(+)
   and ac.toid   = ot.oid$(+)
   and ot.type#(+) = 13
   and o.type# in (2, 3, 4)  -- tables, clusters, views
   AND u.name = 'MDBA'
   AND o.name = 'KBS_CT'



-- Hoe Oracle SQLCOlumns doet

 SELECT /*+ RULE */
        ''
       ,owner
       ,table_name
       ,column_name
       ,0
       ,data_type
       ,data_precision
       ,decode(data_type
              ,'DATE',    16
              ,'FLOAT',   8
              ,'LONG RAW',2147483647
              ,'LONG',    2147483647
              ,'CLOB',    2147483647
              ,'BLOB',    2147483647
              ,'BFILE',   2147483647
              ,'NUMBER',NVL(data_precision+2,40),data_length)
       ,data_scale
       ,0
       ,decode(nullable, 'Y', 1, 'N', 0)
       ,''
       ,''
       ,0
       ,0
       ,0
       ,column_id
       ,decode(nullable, 'Y', 'YES', 'N', 'NO')
  FROM  all_tab_columns
 WHERE TABLE_NAME = 'KBS_CT'
   AND OWNER ='MDBA'
UNION
SELECT /*+ RULE */
        ''
       ,b.owner
       ,b.synonym_name
       ,a.column_name
       ,0
       ,a.data_type
       ,a.data_precision
       ,decode(data_type
              ,'DATE',    16
              ,'FLOAT',   8
              ,'LONG RAW',2147483647
              ,'LONG',    2147483647
              ,'CLOB',    2147483647
              ,'BLOB',    2147483647
              ,'BFILE',   2147483647
              ,'NUMBER',NVL(data_precision+2,40),data_length)
       ,a.data_scale
       ,0
       ,decode(a.nullable, 'Y', 1, 'N', 0)
       ,''
       ,''
       ,0
       ,0
       ,0
       ,a.column_id
       ,decode(a.nullable, 'Y', 'YES', 'N', 'NO')
   FROM all_tab_columns a
       ,all_synonyms b
  WHERE a.table_name = b.table_name
    and a.owner      = b.table_owner
    AND b.synonym_name = 'KBS_CT'
    AND b.owner = 'PUBLIC'

  ORDER BY 2,3,17
;

SELECT /*+RULE*/ * FROM all_synonyms WHERE synonym_name = 'KBS_CT' AND owner ='PUBLIC';

-- EDO SQLColumns

SELECT ''            as table_cat
      ,c.owner       as table_schem
      ,c.table_name  as table_name
      ,c.column_name as column_name
      ,decode(SubStr(c.data_type,1,5)
            ,'VARCH',     12  -- VARCHAR2
            ,'NVARC',     -9  -- NVARCHAR2
            ,'NUMBE',      3  -- NUMBER
            ,'FLOAT',      8
            ,'LONG',      -1
            ,'NCHAR',decode(c.data_type,'NCHAR',-8,'NCHAR VARYING',-9)
            ,'DATE',      11
            ,'RAW',       -3
            ,'LONG ',     -4  -- LONG RAW
            ,'CHAR',       1
            ,'NCLOB',    -10
            ,'CLOB',      -1
            ,'BLOB',      -4
            ,'TIME(',     92
            ,'TIMES',     93
            ,'INTER',Decode(SubStr(c.data_type,1,12)
                          ,'INTERVAL YEA',107,'INTERVAL DAY',110)
            ,0)         AS data_type
      ,c.data_type   as type_name
      ,decode(data_type
             ,'DATE',16
             ,'FLOAT',8
             ,'LONG RAW',2147483647
             ,'LONG',2147483647
             ,'CLOB',2147483647
             ,'BLOB',2147483647
             ,'BFILE',2147483647
             ,'NUMBER',NVL(data_precision+2,40)
             ,data_length) as column_size
      ,c.data_length   as buffer_length
      ,c.data_scale    as decimal_digits
      ,CASE c.data_type
            WHEN 'NUMBER' THEN 2
            ELSE  0  END as num_prec_radix
      ,CASE c.nullable
            WHEN 'Y' THEN 1
            ELSE  0  END as nullable
      ,(SELECT m.comments
          FROM all_col_comments m
         WHERE c.owner       = m.owner
           AND c.table_name  = m.table_name
           AND c.column_name = m.column_name ) as remarks
      ,''            as column_def
      ,0             as sql_data_type
      ,CASE SubStr(c.data_type,1,12)
            WHEN 'INTERVAL YEA' THEN 7
            WHEN 'INTERVAL DAY' THEN 10
            ELSE 0 END as sql_datatime_sub
      ,c.char_col_decl_length as char_octet_length
      ,c.column_id       as ordinal_position
      ,CASE c.nullable
            WHEN 'Y' THEN 1
            ELSE  0  END as is_nullable
  FROM all_tab_columns c
 WHERE c.table_name = 'DBS_CT'
   AND c.owner = 'IDBA'
;


-- FOREIGN KEYS
SELECT * FROM all_cons_columns;


SELECT /*+ RULE */
       ''
      ,pk.owner
      ,pk.table_name
      ,pc.column_name
      ,NULL
      ,fk.owner
      ,fk.table_name
      ,fc.column_name
      ,pc.position
      ,NULL AS update_rule
      ,decode(fk.delete_rule, 'CASCADE', 0, 'NO ACTION', 3) AS delete_rule
      ,fk.constraint_name
      ,pk.constraint_name
      ,Decode(fk.deferred,'IMMEDIATE',1,0)
  FROM all_constraints  pk
      ,all_cons_columns pc
      ,all_constraints  fk
      ,all_cons_columns fc
 WHERE pk.owner           = pc.owner
   and pk.constraint_name = pc.constraint_name
   and fk.owner           = fc.owner
   and fk.constraint_name = fc.constraint_name
   and pk.constraint_type ='P'
   and fk.constraint_type ='R'
   and pk.constraint_name = fk.r_constraint_name
   and pk.owner           = fk.r_owner
   and pc.position        = fc.position
   AND fk.table_name ='KBS_CT'
   and fk.owner      ='MDBA'
   ORDER BY 2, 3, 9



-- SQLStatistics

(
SELECT /*+ RULE */
       ''
      ,T.owner
      ,T.table_name
      ,TO_NUMBER(NULL)
      ,''
      ,''
      ,0
      ,TO_NUMBER(NULL)
      ,''
      ,''
      ,num_rows
      ,blocks
      ,NULL
  FROM ALL_TABLES T
 WHERE table_name='KBS_CT'
   AND owner = 'MDBA'
 UNION
SELECT /*+ RULE*/
       ''
      ,a.table_owner
      ,a.table_name
      ,decode(a.uniqueness, 'UNIQUE', 0, 1)
      ,a.owner
      ,a.index_name
      ,3
      ,b.column_position
      ,b.column_name
      ,'A'
      ,a.distinct_keys
      ,a.leaf_blocks
      ,NULL
  FROM ALL_INDEXES a
      ,ALL_IND_COLUMNS b
 WHERE a.owner = b.index_owner
   AND a.index_name = b.index_name
   AND a.table_owner='MDBA'
   AND a.table_name='KBS_CT'
UNION
SELECT /*+ RULE */
       ''
      ,c.table_owner
      ,c.table_name
      ,decode(a.uniqueness, 'UNIQUE', 0, 1)
      ,a.owner
      ,a.index_name
      ,3
      ,b.column_position
      ,b.column_name
      ,'A'
      ,a.distinct_keys
      ,a.leaf_blocks
      ,NULL
  FROM ALL_INDEXES a
      ,ALL_IND_COLUMNS b
      ,ALL_SYNONYMS c
 WHERE c.synonym_name='KBS_CT'
   AND (c.owner='MDBA' OR c.owner = 'PUBLIC')
   AND c.table_name  = a.table_name
   AND c.table_name  = b.table_name
   AND c.table_owner = a.table_owner
   AND c.table_owner= b.table_owner
   AND a.owner = b.index_owner
   AND a.index_name = b.index_name )
 ORDER BY 4, 5, 6, 8
;

-- SQLSpecialColumns

SELECT /*+ RULE */
       2        AS scope
      ,'ROWID'  AS column_name
      ,1        AS data_type
      ,'ROWID'  AS type_name
      ,NULL     AS column_size
      ,18       AS buffer_length
      ,NULL     AS decimal_digits
      ,2        AS pseudo_column
  FROM ALL_TABLES a
 WHERE a.table_name='ALL_ODBC_TYPES'
   AND a.owner='IDBA'
 UNION SELECT /*+ RULE */
       2
      ,'ROWID'
      ,1
      ,'ROWID'
      ,NULL
      ,18
      ,NULL
      ,2
  FROM ALL_TABLES a
      ,ALL_SYNONYMS b
 WHERE b.table_name = a.table_name
   AND b.synonym_name='ALL_ODBC_TYPES'
   AND b.owner='IDBA'
;

-- SQLTablePrivileges

SELECT ''           AS table_cat
      ,b.owner      AS table_schem
      ,a.table_name
      ,a.grantor
      ,a.grantee
      ,a.privilege
      ,a.grantable AS is_grantable
  FROM ALL_TAB_PRIVS a
     , ALL_TABLES b
 WHERE a.table_schema = b.owner
   AND a.table_name   = b.table_name
   AND b.owner  = ('IDBA')
   AND b.table_name  = ('ALL_ODBC_TYPES')
   AND a.table_schema  = ('IDBA')
   AND a.table_name  = ('ALL_ODBC_TYPES')
UNION
SELECT ''
      ,c.owner
      ,c.synonym_name
      ,a.grantor
      ,a.grantee
      ,a.privilege
      ,a.grantable
  FROM ALL_TAB_PRIVS a
      ,ALL_TABLES b
      ,ALL_SYNONYMS c
 WHERE c.table_name = b.table_name
   AND c.table_owner= b.owner
   AND a.table_schema=c.table_owner
   AND a.table_name = c.table_name
   AND c.owner  = ('IDBA')
   AND c.synonym_name  = ('ALL_ODBC_TYPES')
UNION
SELECT ''
      ,o.owner
      ,o.table_name
      ,o.owner
      ,o.owner
      ,'SELECT'
      ,'YES'
  FROM all_tables o
 WHERE o.owner = 'IDBA'
   AND o.table_name = 'ALL_ODBC_TYPES'
UNION
SELECT ''
      ,o.owner
      ,o.table_name
      ,o.owner
      ,o.owner
      ,'INSERT'
      ,'YES'
  FROM all_tables o
 WHERE o.owner = 'IDBA'
   AND o.table_name = 'ALL_ODBC_TYPES'
UNION
SELECT ''
      ,o.owner
      ,o.table_name
      ,o.owner
      ,o.owner
      ,'UPDATE'
      ,'YES'
  FROM all_tables o
 WHERE o.owner = 'IDBA'
   AND o.table_name = 'ALL_ODBC_TYPES'
UNION
SELECT ''
      ,o.owner
      ,o.table_name
      ,o.owner
      ,o.owner
      ,'DELETE'
      ,'YES'
  FROM all_tables o
 WHERE o.owner = 'IDBA'
   AND o.table_name = 'ALL_ODBC_TYPES'
UNION
SELECT ''
      ,o.owner
      ,o.table_name
      ,o.owner
      ,o.owner
      ,'REFERENCES'
      ,'YES'
  FROM all_tables o
 WHERE o.owner = 'IDBA'
   AND o.table_name = 'ALL_ODBC_TYPES'


-- SQLColumnPrivileges

GRANT update (field1,field2) ON idba.all_odbc_types TO mdba;

SELECT * FROM all_synonyms
WHERE SYNONYM_name = 'ALL_ODBC_TYPES';

SELECT ''           AS table_cat
      ,b.owner      AS table_schem
      ,a.table_name
      ,a.column_name
      ,a.grantor
      ,a.grantee
      ,a.privilege
      ,a.grantable AS is_grantable
  FROM all_col_privs a
     , all_tab_columns b
 WHERE a.table_schema = b.owner
   AND a.table_name   = b.table_name
   AND a.column_name  = b.column_name
   AND b.owner        = 'IDBA'
   AND b.table_name   = 'ALL_ODBC_TYPES'
   AND a.table_schema = 'IDBA'
   AND a.table_name   = 'ALL_ODBC_TYPES'
UNION
SELECT ''
      ,c.owner
      ,c.synonym_name
      ,a.column_name
      ,a.grantor
      ,a.grantee
      ,a.privilege
      ,a.grantable
  FROM all_col_privs a
      ,all_tab_columns b
      ,all_synonyms c
 WHERE c.table_name   = b.table_name
   AND c.table_owner  = b.owner
   AND a.table_schema = c.table_owner
   AND a.table_name   = c.table_name
   AND a.column_name  = b.column_name
   AND c.owner        = 'PUBLIC'
   AND c.synonym_name = 'ALL_ODBC_TYPES'
UNION
SELECT ''
      ,o.owner
      ,o.table_name
      ,o.column_name
      ,o.owner
      ,o.owner
      ,'SELECT'
      ,'YES'
  FROM all_tab_columns o
 WHERE o.owner = 'IDBA'
   AND o.table_name = 'ALL_ODBC_TYPES'
   AND o.column_name = 'FIELD1'
UNION
SELECT ''
      ,o.owner
      ,o.table_name
      ,o.column_name
      ,o.owner
      ,o.owner
      ,'UPDATE'
      ,'YES'
  FROM all_tab_columns o
 WHERE o.owner       = 'IDBA'
   AND o.table_name  = 'ALL_ODBC_TYPES'
   AND o.column_name = 'FIELD1'
UNION
SELECT ''
      ,o.owner
      ,o.table_name
      ,o.column_name
      ,o.owner
      ,o.owner
      ,'REFERENCES'
      ,'YES'
  FROM all_tab_columns o
 WHERE o.owner       = 'IDBA'
   AND o.table_name  = 'ALL_ODBC_TYPES'
   AND o.column_name = 'FIELD1'

-- SQLProcedures

SELECT /*+ RULE */
       ''
      ,b.owner
      ,decode (b.object_type,'PACKAGE',
                 CONCAT( CONCAT (b.object_name, '.'), a.object_name),b.object_name)
      ,NULL
      ,NULL
      ,NULL
      ,NULL
      ,decode (b.object_type,'PACKAGE',decode(a.position, 0, 2, 1, 1, 0),
               decode(b.object_type, 'PROCEDURE', 1, 'FUNCTION', 2, 0))
  FROM  ALL_ARGUMENTS a
      , ALL_OBJECTS b
 WHERE (b.object_type = 'PROCEDURE' OR
        b.object_type = 'FUNCTION' )
   AND  b.object_id = a.object_id
   AND a.sequence = 1
   AND a.OBJECT_NAME LIKE 'DBS_CT_INS_PROC'
UNION
SELECT /*+ RULE */
       ''
      ,b.owner
      ,b.object_name
      ,NULL
      ,NULL
      ,NULL
      ,NULL
      ,decode(b.object_type, 'PROCEDURE', 1, 'FUNCTION', 2, 0)
 FROM ALL_OBJECTS b
 WHERE  (b.object_type = 'PROCEDURE' OR
         b.object_type = 'FUNCTION')
  AND b.OBJECT_NAME LIKE 'DBS_CT_INS_PROC'
UNION
SELECT /*+ RULE */
       distinct ''
      ,a.owner
      ,a.package_name || '.' || a.object_name
      ,NULL
      ,NULL
      ,NULL
      ,NULL
      ,decode(a.position, 0, 2, 1, 1, 0)
  FROM ALL_ARGUMENTS a
      ,all_objects b
 WHERE a.object_id = b.object_id
   AND a.sequence = 1
   AND a.OBJECT_NAME LIKE 'DBS_CT_INS_PROC'
   AND b.object_type = 'PACKAGE'
 ORDER BY 2,3
;

-- Maar deze is beter: SQLProcedures
SELECT ''       as procedure_cat
      ,b.owner  as procedure_schem
      ,decode (b.object_type,'PACKAGE',CONCAT(CONCAT(b.object_name,'.'), a.object_name),b.object_name) as procedure_name
      ,(SELECT Count(*)
          FROM all_arguments a
         WHERE a.object_id = b.object_id
           AND a.position  > 0
           AND a.in_out IN ('IN','IN/OUT'))    as num_input_params
      ,(SELECT Count(*)
          FROM all_arguments a
         WHERE a.object_id = b.object_id
           AND a.position  > 0
           AND a.in_out IN ('OUT','IN/OUT'))   as num_output_params
      ,(SELECT Count(*)
          FROM all_arguments a
         WHERE a.object_id = b.object_id
           AND a.position  > 0
           AND a.data_type = ('PL/SQL TABLE')) as num_result_sets
      ,NULL    as remarks
      ,decode (b.object_type,'PACKAGE',decode(a.position,0,2,1,1,0),decode(b.object_type,'PROCEDURE',1,'FUNCTION',2,0)) as procedure_type
  FROM all_arguments a
      ,all_objects b
 WHERE ( b.object_type = 'PROCEDURE' OR
         b.object_type = 'FUNCTION'  OR
         b.object_type = 'PACKAGE'   )
   AND b.object_id = a.object_id
   AND a.sequence  = 1
   AND b.owner = 'IDBA'
   AND b.object_name = 'DBS_CT_DEL_PROC'
ORDER BY 1,2,3
;


-- SQLProcedureColumns

SELECT ''        AS procedure_cat
      ,a.owner   AS procedure_schem
      ,decode (b.object_type,'PACKAGE',CONCAT(CONCAT(b.object_name,'.'),a.object_name),b.object_name) AS procedure_name
      ,decode(a.position,0,'RETURN_VALUE',a.argument_name) AS colunn_name
      ,decode(a.position,0,5,decode(a.in_out,'IN',1,'IN/OUT',2,'OUT',4)) AS column_type
      ,decode(SubStr(a.data_type,1,5)
            ,'VARCH',     12  -- VARCHAR2
            ,'NVARC',     -9  -- NVARCHAR2
            ,'NUMBE',      3  -- NUMBER
            ,'FLOAT',      8
            ,'LONG',      -1
            ,'NCHAR',decode(a.data_type,'NCHAR',-8,'NCHAR VARYING',-9)
            ,'DATE',      11
            ,'RAW',       -3
            ,'LONG ',     -4  -- LONG RAW
            ,'CHAR',       1
            ,'NCLOB',    -10
            ,'CLOB',      -1
            ,'BLOB',      -4
            ,'TIME(',     92
            ,'TIMES',     93
            ,'INTER',Decode(SubStr(a.data_type,1,12)
                          ,'INTERVAL YEA',107,'INTERVAL DAY',110)
            ,0)         AS data_type
      ,a.data_type      AS type_name
      ,a.data_precision AS column_size
      ,a.data_length    AS buffer_length
      ,a.data_scale     AS decimal_digits
      ,a.radix          AS num_prec_radix
      ,2                AS nullable
      ,''               AS remarks
      ,''               AS column_def
      ,0                AS sql_data_type
      ,0                AS sql_datetime_sub
      ,''               AS char_octet_length
      ,a.position       AS ordinal_position
      ,''               AS is_nullable
  FROM ALL_ARGUMENTS a
      ,ALL_OBJECTS b
 WHERE (b.object_type = 'PROCEDURE' OR
        b.object_type = 'FUNCTION')
   AND b.object_id = a.object_id
   AND a.data_level = 0
   AND b.OBJECT_NAME LIKE 'DBS_CT_INS_PROC'
   AND b.OWNER = 'IDBA'
 ORDER BY 18

 ;

