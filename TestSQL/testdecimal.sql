DROP TABLE test_decimal;

CREATE TABLE test_decimal
(
  veld DECIMAL (30,26)
);

INSERT INTO test_decimal
VALUES (20.1234567890123456789012345);

COMMIT;

SELECT * FROM test_decimal
