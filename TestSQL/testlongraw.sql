DROP TABLE test_longraw;

CREATE TABLE test_longraw
(
    oid  INTEGER
   ,veld LONG RAW
);


INSERT INTO test_longraw
VALUES (1,'aabbcc010203');



SELECT * FROM test_longraw;
COMMIT;