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
// Contents:  Implementation of List
//
// File       : $Archive: $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "StdAfx.h" 
#include "ODBCList.h"
#include "BaseObject.h"

ODBCList::ODBCList()
{
  m_lpFirst =
  m_lpLast  = NULL;
}

ODBCList::~ODBCList()
{
  m_lpFirst =
  m_lpLast  = NULL;
}

bool
ODBCList::HasObjects()
{
  return (m_lpFirst != NULL);
}

bool 
ODBCList::AddObject(ODBCBaseObject *lpObject)
{
  // if not NULL, then object already in some list
  if(lpObject->m_lpPrev || lpObject->m_lpNext)
  {
    TRACE("object already in some list\n");
    return false;
  }
  if(m_lpLast)
  {
    // TRACE("list have Last %p\n",m_lpLast);
    lpObject->m_lpPrev = m_lpLast;
    m_lpLast->m_lpNext = lpObject;
    lpObject->m_lpNext = NULL;
    m_lpLast = lpObject;
  }
  else
  {
    // TRACE("list have no Last\n");
    m_lpFirst = 
    m_lpLast  = lpObject;
    lpObject->m_lpPrev = NULL;
    lpObject->m_lpNext = NULL;
  }
  return true;
}

bool 
ODBCList::RemoveObject(ODBCBaseObject *lpObject)
{
  //printf("delete object from list\n");
  if(!lpObject)
  {
    return false;
  }
  if(lpObject->m_lpPrev)
  {
    //printf("object have prev %p\n",lpObject->lpPrev);
    lpObject->m_lpPrev->m_lpNext = lpObject->m_lpNext;
  }
  if(lpObject->m_lpNext)
  {
    //printf("object have next %p\n",lpObject->lpNext);
    lpObject->m_lpNext->m_lpPrev = lpObject->m_lpPrev;
  }
  if(m_lpFirst==lpObject)
  {
    //printf("object is first\n");
    m_lpFirst = lpObject->m_lpNext;
    if(m_lpLast == lpObject)
    {
      //printf("object is last\n");
      m_lpLast  =
      m_lpFirst = NULL;
    }
    lpObject->m_lpPrev = 
    lpObject->m_lpNext = NULL;
    return true;
  }
  if(m_lpLast == lpObject)
  {
    //printf("object is last but not first\n");
    m_lpLast = lpObject->m_lpPrev;
    lpObject->m_lpPrev = 
    lpObject->m_lpNext = NULL;
    return true;
  }
  //printf("set object prev next to null\n");
  lpObject->m_lpPrev =
  lpObject->m_lpNext = NULL;
  return true;
}

bool 
ODBCList::RemoveAllObjects()
{
  ODBCBaseObject *lpObject = m_lpFirst,*lpTempObj;
  while(lpObject)
  {
    lpTempObj = lpObject;
    lpObject  = lpObject->m_lpNext;
    lpTempObj->m_lpNext = NULL;
    lpTempObj->m_lpPrev = NULL;
    delete lpTempObj;
  }
  m_lpLast  =
  m_lpFirst = NULL;
  return true;
}
