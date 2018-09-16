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
// Contents:  Implementation of the Mutex locking mechanism
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "Locking.h"

Mutex::Mutex()
{
  //TRACE("CTOR Mutex %p\n",this);
  InitializeCriticalSection(&m_section);
}

Mutex::~Mutex()
{
  //TRACE("DTOR Mutex %p\n",this);
  if(m_section.LockCount >= 0)
  {
    release();
    DeleteCriticalSection(&m_section);
  }
}

void 
Mutex::lock()
{
  EnterCriticalSection(&m_section);
}

void 
Mutex::release()
{
  LeaveCriticalSection(&m_section);
}

bool
Mutex::islocked()
{
  return (m_section.LockCount >= 0);
}

Guard::Guard(Mutex& p_mutex)
      :m_mutex(p_mutex)
{
  p_mutex.lock();
}

Guard::~Guard()
{
  m_mutex.release();
}


