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
/////////////////////////////////////////////////////////////////////////////
//
// Contents:  The interface for the base object for the ODBC driver
//            Base objects make general handle alloc/free possible
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include "ErrorHolder.h"

// Base class for all ODBC objects
class ODBCBaseObject
{
  friend class ODBCList;
public:
  ODBCBaseObject(OCIEnv *hEnv);
  virtual ~ODBCBaseObject();

  // Free object handler
  virtual SQLRETURN FreeObjectHandler() = 0;
  //  Get OCIError object wrapper object
  ODBCErrorHolder* GetOCIErrorObject();
  // Make validation of object, if it is a base ODBCBaseObject
  // Check validation of object by check SignBuff and calling ValidHandle() in the derived class
  // returns SQL_INVALID_HANDLE (-2) if not valid object or type of handle SQL_HANDLE_*
  int ValidObject(); 
  // Each derived class has a valid handle method
  // returns SQL_INVALID_HANDLE (-2) if not valid object or type of handle SQL_HANDLE_*	
  virtual int ValidHandle() = 0;
  // Get next base object in the list
  ODBCBaseObject* GetNext();
protected:
  ODBCErrorHolder m_ociError;  // Error descriptor
  __int64 m_signBuff;          // Signature of object for checking validation of handle
  __int64 m_localSign;         // Signature for object that will inherits from this class

  ODBCBaseObject *m_lpPrev;    // Previous object in ptr list
  ODBCBaseObject *m_lpNext;    // Next object in ptr list
};

inline ODBCErrorHolder*
ODBCBaseObject::GetOCIErrorObject()
{
  return &m_ociError;
}

inline ODBCBaseObject* 
ODBCBaseObject::GetNext()
{
  return m_lpNext;
}
