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
// Contents:  Interface of List
//
// File       : $Archive: $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once

class ODBCBaseObject;

// class for control pointer list array
class ODBCList
{
public:
  ODBCList();
  ~ODBCList();
  // Add object to list
  // param lpObject ptr to object that must be inserted in list
  // return true for success operation or false for error
  bool AddObject(ODBCBaseObject *lpObject);
  // Remove object from list
  // param lpObject ptr to object that must be inserted in list
  // return true for success operation or false for error
  bool RemoveObject(ODBCBaseObject *lpObject);
  // Remove all object from list
  // delete all objects in list
  // return true for success operation or false for error
  bool RemoveAllObjects();
  // Objects in the list
  bool HasObjects();
  // Get the first object in the list
  ODBCBaseObject* GetFirst();
protected:
  ODBCBaseObject *m_lpFirst;   // First object in ptr list
  ODBCBaseObject *m_lpLast;    // Last  object in ptr list
};

inline ODBCBaseObject* 
ODBCList::GetFirst()
{
  return m_lpFirst;
}
