-- Test table with all Oracle ODBC types
-- all in one big table
-- Use to test SQLColumns with column comment

DROP TABLE   all_odbc_types;

CREATE TABLE all_odbc_types
(
    field1  CHAR(200)
   ,field2  VARCHAR2(200)
   ,field3  NCHAR(200)
   ,field4  NVARCHAR2(200)
   ,field5  NUMBER
   ,field6  NUMBER(30,18)
   ,field7  NUMBER(8,0)
   ,field8  NUMBER(32,32)
   ,field9  LONG
   ,field10 FLOAT
   ,field11 RAW(200)
   ,field12 DATE
   ,field13 TIMESTAMP
   ,field14 TIMESTAMP(6)
   ,field15 TIMESTAMP(6) WITH TIME ZONE
   ,field16 TIMESTAMP(6) WITH LOCAL TIME ZONE
   ,field17 INTERVAL YEAR TO MONTH
   ,field18 INTERVAL YEAR (2) TO MONTH
   ,field19 INTERVAL DAY TO SECOND(6)
   ,field20 INTERVAL DAY (4) TO SECOND(6)
   ,field21 BLOB
   ,field22 CLOB
   ,field23 UROWID(20)
)
;

--   ,field10  LONG RAW

ALTER TABLE all_odbc_types
ADD CONSTRAINT pk_odbc PRIMARY KEY (field1);

COMMENT ON TABLE  all_odbc_types        IS 'Table with all odbc data types';
COMMENT ON COLUMN all_odbc_types.field1 IS 'Primary key in old char style';
COMMENT ON COLUMN all_odbc_types.field2 IS 'Normal varchar field';
COMMENT ON COLUMN all_odbc_types.field3 IS 'character in your own lingua franqa';
COMMENT ON COLUMN all_odbc_types.field4 IS 'String in your own lingua franqa';
COMMENT ON COLUMN all_odbc_types.field5 IS 'Plain number';
COMMENT ON COLUMN all_odbc_types.field6 IS 'Number with ''some'' precision and scale';
COMMENT ON COLUMN all_odbc_types.field7 IS 'Number with no scale';
COMMENT ON COLUMN all_odbc_types.field8 IS 'ANSI decimal max';
COMMENT ON COLUMN all_odbc_types.field9 IS 'Real long long';
COMMENT ON COLUMN all_odbc_types.field10 IS 'Ansi float';
COMMENT ON COLUMN all_odbc_types.field11 IS 'Some raw data';
COMMENT ON COLUMN all_odbc_types.field12 IS 'Standard oracle date';
COMMENT ON COLUMN all_odbc_types.field13 IS 'New timestamp';
COMMENT ON COLUMN all_odbc_types.field14 IS 'New timestamp with max precision';
COMMENT ON COLUMN all_odbc_types.field15 IS 'New timestamp with time zone';
COMMENT ON COLUMN all_odbc_types.field16 IS 'New timestamp with your own local timezone';
COMMENT ON COLUMN all_odbc_types.field17 IS 'Interval yy-mm';
COMMENT ON COLUMN all_odbc_types.field18 IS 'Interval max 2 yy-mm';
COMMENT ON COLUMN all_odbc_types.field19 IS 'Interval day-second(6)';
COMMENT ON COLUMN all_odbc_types.field20 IS 'Interval max 4 day to second';
COMMENT ON COLUMN all_odbc_types.field21 IS 'It''s not a BLOB';
COMMENT ON COLUMN all_odbc_types.field22 IS 'It''s not a char blob';
COMMENT ON COLUMN all_odbc_types.field23 IS 'Unified ROWID';

