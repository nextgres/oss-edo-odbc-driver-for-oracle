WELCOME TO EDO (Edo's Driver for Oracle - Open Oracle ODBC)
===========================================================

This driver has it's root in the following open-source products
and is loosly based on parts of other open source software
that does similar things. Those projects beeing:

- Oracle ODBC Driver for Linux (from www.sf.net)
  Written in 2004 by Evgen Bendyak 
- Firebird 2.0 ODBC Driver
  Originally written by Jim Starkey for www.IBPhoenix.com
  The unicode layer by Vladimir Tsvigun was a great example
- mySQL ODBC Driver, by various persons
  From www.mysql.org, a well known open source database
  The SQL_NUMERIC interface was borrowed from this project
- ODBC++, an ODBC project by various persons

And as open source goes: reading these great project has 
certainly helped designing and building this one!!!

The following sources of information have been used very
extensivly:
- The book "Inside ODBC" by Kyle Geiger. The original architect
  of the ODBC standard.
- The current documentation at "msdn.microsoft.com" for ODBC.
- OCI Oracle Call Interface documentation version's 9i upto 11g

The reason to write a whole new ODBC drive for Oracle 9i came
from the notion that there are a number of limitations to the 
original driver by Oracle itself that limits the possibilities
of applications considderably. These limits and bugs are written
down in a separate file "Oracle_odbc_bugs.txt".

Other closed source vendors like 'Easysoft' and 'DataDirect' do
deliver an Oracle ODBC driver, but these are quite costly, sometimes
even to the extent of the price of the database itself!!
