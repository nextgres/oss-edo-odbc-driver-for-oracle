CREATE TABLE test_num
(
  oid INTEGER
 ,num NUMBER
 )


INSERT INTO test_num VALUES (1,3.123456789012345678);
INSERT INTO test_num VALUES (2,3.1234567890123456789012345);
INSERT INTO test_num VALUES (3,3.123456789012345678901234567890);
INSERT INTO test_num VALUES (3,3.12345678901234567890123456789012);
SELECT * FROM test_num;