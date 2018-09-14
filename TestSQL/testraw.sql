-- Test for a 'normal' RAW datatype

:FILE append 'c:\tmp\odbctest.txt';

CREATE TABLE test_raw
(
  field RAW(100)
)
/

DELETE FROM test_raw;
:variable1 INPUT BINARY;
:variable1 = 'AB121314DE';

INSERT INTO test_raw VALUES(?);
COMMIT;

:variable1 = '0';
:SELECT ON;

SELECT field FROM test_raw;

:IF variable1 <> 'AB121314DE';
:print 'ERROR raw not loaded';

DELETE FROM test_raw;
DROP TABLE test_raw;

:FILE CLOSE;