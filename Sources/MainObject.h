//////////////////////////////////////////////////////////////////////////
//
// EDO = Edo's Driver for Oracle
// ORACLE ODBC DRIVER for ODBC 3.51
//
// Copyright (C) 2008-2015 ir. Wicher Edo Huisman
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
//
////////////////////////////////////////////////////////////////////////
//
// Contents:  Interface of the ODBC main object
//            Static object for the driver (all environments/connections)
//            Implements logging and the alloc/free of environments
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "ODBCList.h"
#include "ErrorHolder.h"
#include "Locking.h"
#include <locale.h>

// Codepage
extern UINT      codePage;
extern _locale_t ODBC_Locale;

// Supported ODBC functions in this driver
extern SQLUSMALLINT ODBCFunctions_2[100];                              // ODBC 1.x and 2.x standard is 100 int's
extern SQLUSMALLINT ODBCFunctions_3[SQL_API_ODBC3_ALL_FUNCTIONS_SIZE]; // ODBC 3.x standard = 4000 bits

// Main module class
// This class auto start, when this module is loaded. And auto stop, when this module is begin unloading
class ODBCEnvironment;

class ODBCMainObject
{
public:
  // Mutex for Env object access		
  Mutex m_lock_mutex;
  // Read if present /etc/OraODBC.ini and open Log system (if compiled with LOG_SYSTEM_ON)
  ODBCMainObject();
  // Delete Env object. Stop Log system (if compiled with LOG_SYSTEM_ON)
  ~ODBCMainObject();
  // Initialise the ODBC 2.x and 3.x arrays
  void Init();
  // Print to logfile (if it exists)
  int PrintLog(const char* FormatMsg,...);
  // Open the logfile
  bool OpenLog(const char* p_logname,bool p_append = true);
  // Close the log
  void CloseDriverLog();
  // Return the fact that logging is on
  bool LogSystemOn();
  // Allocate a new environment
  SQLRETURN AllocEnv(ODBCEnvironment **lpObject);	
  // Remove an environment
  void RemoveEnv(ODBCEnvironment* p_env);
  // Return the error object for diag purposes
  ODBCErrorHolder* GetOCIErrorObject();
protected:
  // Open the driver log (if any)
  void OpenDriverLog();
  // All Env list array
  ODBCList m_environments;
  // Driver error
  ODBCErrorHolder m_ociError;
  // Logfile
  FILE *m_logfile;
};

inline bool
ODBCMainObject::LogSystemOn()
{
  return (m_logfile != NULL);
}

inline ODBCErrorHolder* 
ODBCMainObject::GetOCIErrorObject()
{
  return &m_ociError;
}

// THE MAIN OBJECT
// This object auto start, when this module is loaded. 
// And auto stop, when this module is begin unloading
extern ODBCMainObject ODBCDriver;