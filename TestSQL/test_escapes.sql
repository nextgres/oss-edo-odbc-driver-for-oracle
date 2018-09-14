-- All ODBC escape tests

CREATE OR REPLACE PROCEDURE mytest
IS
monkey VARCHAR2(100);
begin
  SELECT dummy INTO monkey FROM dual;
END;
/

CREATE OR REPLACE FUNCTION myfunc
RETURN VARCHAR2 IS
begin
  RETURN 'ABC';
END;
/


SELECT { fn USER() } FROM dual;
SELECT --(* fn USER() *)-- FROM dual;
SELECT --(* VENDOR(Microsoft), PRODUCT(odbc) fn USER() *)-- FROM dual;
SELECT { * } from dual;
{ call mytest()};
SELECT { myfunc()} FROM dual;

SELECT { d'2008-07-30'} FROM dual;
SELECT { t'12:34:47'}   FROM dual;
SELECT { ts'1991-08-16 22:55:00'} AS remi FROM dual;
SELECT { ts'1997-07-30 22:34:47'} AS tara FROM dual;
SELECT { INTERVAL '5-3' YEAR(2) TO MONTH } FROM dual;
SELECT { INTERVAL '4 12:22:18' DAY TO SECOND } FROM dual;
SELECT dummy FROM dual WHERE dummy LIKE 'X' { ESCAPE '\'} AND dummy >'A';
SELECT { 'one''two''three'} FROM dual;



SELECT { fn ABS( 42)} from dual;
SELECT { fn ABS(-42)} from dual;
SELECT { fn ACOS(0.5)} FROM dual;
SELECT { fn Ascii('a') } FROM dual;
SELECT { fn ASIN(0.5)} FROM dual;
SELECT { fn ATAN(0.5)} FROM dual;
SELECT { fn ATAN2(0.5,1)} FROM dual;
SELECT { fn bit_length(dummy) } FROM dual;
SELECT { fn CEILING(3.14)} FROM dual;
SELECT { fn CHAR(97)} FROM dual;
SELECT { fn CHAR_LENGTH('monkey')} FROM dual;
SELECT { fn CHARACTER_LENGTH('monkey')} FROM dual;
SELECT { fn CONCAT('Gorilla ','business')} from dual;
SELECT { fn CONCAT('Gorilla ',NULL)} from dual;
SELECT { fn CONVERT(1234,SQL_VARCHAR)} FROM dual;
SELECT { fn COS(1.5)} FROM dual;
SELECT { fn COT(0.234)} FROM dual;
SELECT { fn CURDATE()} FROM dual;
SELECT { fn CURTIME()} from dual;
SELECT { fn CURRENT_DATE()} from dual;
SELECT { fn CURRENT_TIME(3)} FROM dual;
SELECT { fn CURRENT_TIME()} FROM dual;
SELECT { fn CURRENT_TIMESTAMP(3)} from dual;
SELECT { fn CURRENT_TIMESTAMP()} from dual;
SELECT { fn DATABASE()} FROM dual;
SELECT { fn DATABASE ( )} FROM dual;
SELECT { fn DAYNAME({fn CURDATE()})} FROM dual;
SELECT { fn DAYOFMONTH( {fn CURDATE()} )} FROM dual;
SELECT { fn DAYOFWEEK( {fn CURDATE()} )} FROM dual;
SELECT { fn DAYOFYEAR( {fn CURDATE()} )} FROM dual;
SELECT { fn DEGREES(3.1)} FROM dual;
SELECT { fn EXP(3)} FROM dual;
SELECT { fn EXTRACT( MONTH FROM {fn CURDATE()} )} FROM dual;
SELECT { fn FLOOR(3.14)} FROM dual;
SELECT { fn HOUR( {fn CURTIME()} )} FROM dual;
SELECT { fn IFNULL('abc','x')} FROM dual;
SELECT { fn IFNULL(NULL,'x')}  FROM dual;
SELECT { fn INSERT('abcdefghijk',4,2,'xy')} from dual;
SELECT { fn LCASE('ABC')} FROM dual;
SELECT { fn LEFT('ABCDEF',3)} FROM dual;
SELECT { fn Length('ABCDEF')} FROM dual;
SELECT { fn locate('klm','abcdefghijklm',4)} FROM dual;
SELECT { fn locate('klm','abcdefghijklm')} FROM dual;
SELECT { fn LOG(20)} FROM dual;
SELECT { fn LOG10(100)} FROM dual;
SELECT { fn LTRIM('   abc')} FROM dual;
SELECT { fn MINUTE( {fn CURTIME()} )} FROM dual;
SELECT { fn MOD(9.2,4)} FROM dual;
SELECT { fn MONTH( {fn CURDATE()} )} FROM dual;
SELECT { fn MONTHNAME( {fn CURDATE()} )} FROM dual;
SELECT { fn MONTHNAME( {fn CURDATE()} )} FROM dual;
SELECT { fn NOW()} FROM dual;
SELECT { fn OCTET_LENGTH('ABCDEFGHIJ')} FROM dual;
SELECT { fn PI()} FROM dual;
SELECT { fn POSITION('klm' IN 'abcdefghijklmnop')} from dual;
SELECT { fn POWER(10,4)} FROM dual;
SELECT { fn QUARTER( {fn CURDATE()} FROM dual;
SELECT { fn RADIANS(180)} FROM dual;
SELECT { fn repeat('klm',3)} FROM dual;
SELECT { fn REPLACE('this is a surprise','is','xxx')} FROM dual;
SELECT { fn right('abcdefg',4)} FROM dual;
SELECT { fn ROUND(3.1456,2)} from dual;
SELECT { fn RTRIM('abcdefgklmklm','klm')} FROM dual;
SELECT { fn SECOND( {fn CURTIME()} )} FROM dual;
SELECT { fn SIGN(-3)} FROM dual;
SELECT { fn SIGN(+3)} FROM dual;
SELECT { fn SIGN( 0)} FROM dual;
SELECT { fn SIN(1.1)} FROM dual;
SELECT { fn SOUNDEX('Huisman')} FROM dual;
SELECT { fn SPACE(15)}||'xx' FROM dual;
SELECT { fn SQRT(10000)} FROM dual;
SELECT { fn SUBSTRING('abcdefghijklm',4,3)} FROM dual;
SELECT { fn TAN(1)} FROM dual;
SELECT { fn TIMESTAMPDIFF(SQL_TSI_HOUR, {fn CURDATE()} , TO_DATE('01-01-2008') )} FROM dual;
SELECT { fn TRUNCATE(3.1456,2)} FROM dual;
SELECT { fn UCASE('abc')} FROM dual;
SELECT { fn USER()} FROM dual;
SELECT { fn USER ( ) } FROM dual;
SELECT { fn WEEK( {fn CURDATE()} )} FROM dual;
SELECT { fn YEAR( {fn CURDATE()} )} FROM dual;
