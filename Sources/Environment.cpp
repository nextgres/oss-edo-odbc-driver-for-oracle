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
// Contents:  The ODBC environment
//            Connecting it one-to-one to an OCI (Oracle Client Interface) Environment
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "Environment.h"
#include "MainObject.h"
#include "Connection.h"
#include "ErrorInfo.h"

#pragma warning (disable: 4311)
#pragma warning (disable: 4996)

// Validation sign for an environment handle: ENVI
const __int64 staticEnvironment = 0x4549574D;

ODBCEnvironment::ODBCEnvironment(OCIEnv *hEnvParam)
                :ODBCBaseObject(hEnvParam)
                ,m_ODBC_Version(0)
                ,m_inDestructor(0)
{	
  m_localSign = staticEnvironment;
	m_hEnv = hEnvParam;
}

ODBCEnvironment::~ODBCEnvironment()
{
  // If OCIHandleFree here below calls "doexit" then we will become
  // re-entrant. Stop that here.
  if(m_inDestructor)
  {
    _exit(0);
    return;
  }
  m_inDestructor = 1;

  WRITELOG("ODBCEnvironment::~ODBCEnvironment(%p)",this);
	m_ociError.Done();

  // Remove pending connections on this environment
	m_connections.RemoveAllObjects();
	if(m_hEnv)
	{
    // Crashes, but why?
    // OCIHandleFree((dvoid*) m_hEnv, OCI_HTYPE_ENV);
		m_hEnv = NULL;
	}
  ODBCDriver.RemoveEnv(this);
  // Erase our signature
  m_localSign = 0;
}

SQLRETURN
ODBCEnvironment::FreeObjectHandler()
{
  // Call from generic list handler
  if(m_connections.HasObjects())
  {
    return SQL_ERROR;
  }
  delete this;
  return SQL_SUCCESS;
}

void
ODBCEnvironment::RemoveConnection(ODBCConnection* p_conn)
{
  m_connections.RemoveObject((ODBCBaseObject*)p_conn);
}

int 
ODBCEnvironment::ValidHandle()
{
  // compare internal signification with global sign for this class
  if(m_localSign == staticEnvironment)
	{
		return SQL_HANDLE_ENV;
	}
  else
  {
    return SQL_INVALID_HANDLE;
  }
}

// Return the ODBC Version
SQLINTEGER 
ODBCEnvironment::GetODBCVersion()
{
  WRITELOG("ODBC Version reported as: %d",m_ODBC_Version);
  return m_ODBC_Version;
}

// Sets the ODBC Version
SQLRETURN  
ODBCEnvironment::SetODBCVersion(SQLINTEGER p_version)
{
  if(p_version == SQL_OV_ODBC3 ||
     p_version == SQL_OV_ODBC2 )
  {
    m_ODBC_Version = p_version;
    WRITELOG("ODBC Version set to: %d",p_version);
    return SQL_SUCCESS;
  }
  return SQL_ERROR;
}

SQLRETURN 
ODBCEnvironment::SetEnvAttr(SQLINTEGER Attribute
                           ,SQLPOINTER Value
                           ,SQLINTEGER StringLength)
{
  WRITELOG("SQLSetEnvAttr(%p,%s,%d,%d)",this,ERROR_GetEnvAttribute(Attribute),Value,StringLength);
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  SQLINTEGER Val=(SQLINTEGER)((DWORD_PTR)Value);
  switch(Attribute)
	{
  	case SQL_ATTR_ODBC_VERSION:       switch(Val)
			                                {
			                                  case SQL_OV_ODBC2:          // Fall through
			                                  case SQL_OV_ODBC3:          if(SetODBCVersion(Val) != SQL_SUCCESS)
                                                                    {
                                                                      m_ociError.AddError("HY000",10345,"Driver does not work properly with ODBC Driver Manager or applications with a ODBC version older than 2.x");
                                                                      return SQL_ERROR;
                                                                    }
                                                                    return SQL_SUCCESS;
			                                  default:                    m_ociError.AddError("HY024");
				                                                            return SQL_ERROR;
  		                                }
	  	                                break;
	  case SQL_ATTR_CONNECTION_POOLING:	switch(Val)
		                                  {
		                                    case SQL_CP_OFF:				    return SQL_SUCCESS;
		                                    case SQL_CP_ONE_PER_DRIVER:	m_ociError.AddError("HYC00");				
			                                                              return SQL_ERROR;
		                                    case SQL_CP_ONE_PER_HENV:		m_ociError.AddError("HYC00");				
			                                                              return SQL_ERROR;
		                                    default:				            m_ociError.AddError("HY024");
			                                                              return SQL_ERROR;
		                                  }
	                                    break;
  	case SQL_ATTR_CP_MATCH:			      switch(Val)
			                                {
			                                  case SQL_CP_STRICT_MATCH:		return SQL_SUCCESS;
                                        case SQL_CP_RELAXED_MATCH:	m_ociError.AddError("HYC00");				
				                                                            return SQL_ERROR;
			                                  default:				            m_ociError.AddError("HY024");
				                                                            return SQL_ERROR;
			                                }
  		                                break;
    case SQL_ATTR_OUTPUT_NTS:         switch(Val)
                                      {
                                        case SQL_TRUE:              return SQL_SUCCESS;
                                        case SQL_FALSE:             m_ociError.AddError("HYC00");				
                                                                    return SQL_ERROR;
                                      }
	  default:		                      m_ociError.AddError("HY092");
		                                  break;
	}
  return SQL_ERROR;
}

SQLRETURN 
ODBCEnvironment::GetEnvAttr(SQLINTEGER  Attribute
                           ,SQLPOINTER  Value
                           ,SQLINTEGER  /*BufferLength*/
                           ,SQLINTEGER* StringLength)
{
  WRITELOG("SQLGetEnvAttr(%p,%s,%d)",this,ERROR_GetEnvAttribute(Attribute),*StringLength);
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  SQLUINTEGER* Val= (SQLUINTEGER*)Value;
  switch(Attribute)
	{
  	case SQL_ATTR_ODBC_VERSION: 	    *Val = GetODBCVersion();
                                      break;
	  case SQL_ATTR_CONNECTION_POOLING:	*Val = SQL_CP_OFF;
                                		  break;
	  case SQL_ATTR_CP_MATCH:     			*Val = SQL_CP_STRICT_MATCH;			
		                                  break;
    case SQL_ATTR_OUTPUT_NTS:         *Val = SQL_TRUE;
                                      break;
	  default:                      		m_ociError.AddError("HY092");
		                                  return SQL_ERROR;
	}	
  return SQL_SUCCESS;
}

SQLRETURN 
ODBCEnvironment::AllocDBConnection(ODBCConnection **lpObject)
{
  Guard guard(m_lock_mutex);
	m_ociError.Clear();

  if(m_ODBC_Version != SQL_OV_ODBC3 &&
     m_ODBC_Version != SQL_OV_ODBC2 )
  {
    // Driver cannot work with other applications than true ODBC 2.x or newer
    // Otherwise it must return a function sequence error!!
    m_ociError.AddError("HY010");
    return SQL_ERROR;
  }
  *lpObject = new ODBCConnection(this);
  if(*lpObject)
  {
	  m_connections.AddObject(*lpObject);
    return SQL_SUCCESS;
  }
  else
  {
    // ERROR OUT OF MEMORY
    m_ociError.AddError("HY001",0,NULL);
    return SQL_ERROR;
  }
}

SQLRETURN
ODBCEnvironment::SQLEndTran(SQLUSMALLINT p_type)
{
  // Call SQLEndTran for all connections in the environment
  SQLRETURN ret = SQL_SUCCESS;
  ODBCBaseObject* obj = m_connections.GetFirst();
  while(obj)
  {
    ODBCConnection* connection = (ODBCConnection*)obj;
    SQLRETURN res = connection->SQLEndTran(p_type);
    if(res != SQL_SUCCESS)
    {
      ret = res;
    }
    obj = obj->GetNext();
  }
  return ret;
}
