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
// Contents:  Implementation of the attributes of the setup DLL
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h" 
#include "Attributes.h"
#include <odbcinst.h>

char* all_attribs[] =
{
  ATTRIB_SERVERNAME
 ,ATTRIB_DSN
 ,ATTRIB_PASSWORD
 ,ATTRIB_USERID
 ,ATTRIB_TRACEFILE
 ,ATTRIB_TRACELEVEL
 ,ATTRIB_DESCRIPTION
 ,ATTRIB_READONLY
 ,ATTRIB_DRIVER
 ,ATTRIB_STATISTICS
 ,ATTRIB_PRECOUNT
 ,ATTRIB_PREMEMORY
 ,ATTRIB_FAILCOUNT
 ,ATTRIB_FAILTIME
 ,ATTRIB_METADATAID
 ,ATTRIB_COLUMNMAXLEN
 ,ATTRIB_TRANSLIB
 ,ATTRIB_TRANSOPTION
 ,ATTRIB_INTSPEEDUP
 ,ATTRIB_NLSTYPE
 ,ATTRIB_NLSDATE
 ,ATTRIB_NLSTIME
 ,ATTRIB_NLSTIMESTAMP
 ,ATTRIB_NLSTIMESTAMPTZ
 ,NULL
};

Attributes::Attributes(HWND p_hwnd,CString p_dataSource)
           :m_dataSource(p_dataSource)
           ,m_hwnd(p_hwnd)
{
}

Attributes::~Attributes()
{
}

CString
Attributes::GetAttribute(CString p_name)
{
  CString value;
  AttribMap::iterator it =  m_attribs.find(p_name);
  if(it != m_attribs.end())
  {
    value = it->second;
  }
  return value;
}

void
Attributes::SetAttribute(CString p_name, CString p_value)
{
  AttribMap::iterator it = m_attribs.find(p_name);
  if(it != m_attribs.end())
  {
    it->second = p_value;
  }
  else
  {
    m_attribs.insert(std::make_pair(p_name,p_value));
  }
}


bool    
Attributes::ReadAllAttributes()
{
  char attribValue[_MAX_PATH];
  int  i = 0;
  while(all_attribs[i])
  {
    if(SQLGetPrivateProfileString(m_dataSource,all_attribs[i],"",attribValue,_MAX_PATH,"ODBC.INI"))
    {
      if(attribValue[0])
      {
        m_attribs.insert(std::make_pair(all_attribs[i],attribValue));
      }
    }
    ++i;
  }
  return true;
}

bool    
Attributes::WriteAllAttributes()
{
  CString error;
  AttribMap::iterator it = m_attribs.begin();
  while(it != m_attribs.end())
  {
    CString key   = it->first;
    CString value = it->second;
    if(SQLWritePrivateProfileString(m_dataSource,key,value,"ODBC.INI") == FALSE)
    {
      error += "\nCannot write [" + key + "," + value + "] to the registry.";
    }
    ++it;
  }
  if(!error.IsEmpty())
  {
    error = "ERROR While saving ODBC attributes:\n" + error;
    ::MessageBox(m_hwnd,error,"EDO ODBC",MB_OK|MB_ICONERROR|MB_TASKMODAL);
    return false;
  }
  return true;
}

CString
Attributes::MakeConnectionString()
{
  CString val;
  CString connection;

  val = GetAttribute(ATTRIB_DSN);
  if(!val.IsEmpty())
  {
    connection += "DSN=" + val + ";";
  }
  val = GetAttribute(ATTRIB_USERID);
  if(!val.IsEmpty())
  {
    connection += "UID=" + val + ";";
  }
  val = GetAttribute(ATTRIB_PASSWORD);
  if(!val.IsEmpty())
  {
    connection += "PWD=" + val + ";";
  }
  return connection;
}