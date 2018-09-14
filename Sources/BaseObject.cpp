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
/////////////////////////////////////////////////////////////////////////////
//
// Contents:  The base object for the ODBC driver
//            Base objects make general handle alloc/free possible
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "BaseObject.h"

// Used for validation of the base object: BASE
// The word "BASE" is hex encoded and checked in each object
__int64 staticBase = 0x66655469;

ODBCBaseObject::ODBCBaseObject(OCIEnv *hEnv)
                :m_ociError(hEnv)
                ,m_signBuff(staticBase)
                ,m_lpPrev(NULL)
                ,m_lpNext(NULL)
{
}

ODBCBaseObject::~ODBCBaseObject()
{
  m_signBuff = 0;
}

int 
ODBCBaseObject::ValidObject()
{
  if(m_signBuff == staticBase)
  {
    return ValidHandle();
  }
  else
  {
    return SQL_INVALID_HANDLE;
  }
}
