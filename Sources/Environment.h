//////////////////////////////////////////////////////////////////////////
//
// EDO = Edo's Driver for Oracle
// ORACLE ODBC DRIVER for ODBC 3.51
//
// Copyright (C) 2008 ir. Wicher Edo Huisman
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
// Contents:  The ODBC environment
//            Connecting it one-to-one to an OCI (Oracle Client Interface) Environment
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "Locking.h"
#include "BaseObject.h"
#include "ODBCList.h"

class ODBCConnection;
class ODBCStatement;

// Base class for SQL_HANDLE_ENV type (SQLHENV)
class ODBCEnvironment:public ODBCBaseObject
{	
public:
  // Constructor
  // hEnv pointer to OCIEnv object to witch ODBCEnv will be connected
  ODBCEnvironment(OCIEnv* hEnv);
  // Destructor
  virtual ~ODBCEnvironment();
  // Destructor from generic handler
  SQLRETURN FreeObjectHandler();
  // Remove connection from this environment
  void RemoveConnection(ODBCConnection* p_conn);
  // Validation of environment object.
  // return SQL_INVALID_HANDLE (-2) if not valid object or SQL_HANDLE_ENV
  int ValidHandle();
  // Get OCIEnv object
  OCIEnv* GetOwnerEnv();
  // Alloc DB Connection object
  SQLRETURN AllocDBConnection(ODBCConnection **lpObject);
  // Set object internal attributes
  // For more details looks for SQLSetEnvAttr in ODBC documentation	
  SQLRETURN SetEnvAttr(SQLINTEGER Attribute
                      ,SQLPOINTER Value
                      ,SQLINTEGER StringLength);
  // Get object internal attributes
  // For more details looks for SQLGetEnvAttr in ODBC documentation	
  SQLRETURN GetEnvAttr(SQLINTEGER Attribute
                      ,SQLPOINTER Value
                      ,SQLINTEGER BufferLength
                      ,SQLINTEGER* StringLength);
  // Return the ODBC Version
  SQLINTEGER GetODBCVersion();
  // Sets the ODBC Version
  SQLRETURN  SetODBCVersion(SQLINTEGER p_version);
  // SQLEndTran for all connections in the environment
  SQLRETURN SQLEndTran(SQLUSMALLINT p_type);

protected:
  // OCI Environment object
  OCIEnv* m_hEnv;
  // All connections list array
  ODBCList m_connections;
  // Synchronization of the Environment object
  Mutex m_lock_mutex;
  // ODBC Version must be set before allocating an environment
  SQLINTEGER m_ODBC_Version;
  // Non-reentrant destructor
  int m_inDestructor;
};

inline OCIEnv* 
ODBCEnvironment::GetOwnerEnv()
{
  return m_hEnv;
}
