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
// Contents:  Implementation of the Internal Descriptor
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "Descriptor.h"
#include "Statement.h"
#include "Environment.h"
#include "ODBCTypes.h"
#include "ErrorInfo.h"
#include "MainObject.h"

// For validation of the descriptor object: DESC
__int64 staticDescriptor = 0x68695447;

ODBCDescriptor::ODBCDescriptor(ODBCConnection* p_connection
                              ,ODBCStatement*  p_statement
                              ,SQLSMALLINT     p_alloc_type
                              ,bool            p_row)
               :ODBCBaseObject(p_connection->GetOwnerODBCEnv()->GetOwnerEnv())
               ,m_statement(p_statement)
               ,m_connection(p_connection)
               ,m_desc_count(0)
               ,m_desc_count_alloc(DESCRIPTOR_BLOCK_SIZE)
               ,m_desc_array_status_ptr(0)
               ,m_desc_alloc_type(p_alloc_type)
               ,m_desc_row_type(p_row)
{
  m_localSign = staticDescriptor;
  m_ociError.SetDSNName(p_connection->GetOCIErrorObject()->GetDSNName());
}

ODBCDescriptor::~ODBCDescriptor()
{
  m_statement              = NULL;
  m_connection             = NULL;
  m_desc_count             = 0;
  m_desc_count_alloc       = 0;
  m_desc_array_status_ptr  = NULL;
  m_localSign              = 0;
}

int 
ODBCDescriptor::ValidHandle()
{
  // compare internal sign with global sign for this class
  if(m_localSign == staticDescriptor)
  {
    return SQL_HANDLE_DESC;
  }
  else
  {
    return SQL_INVALID_HANDLE;
  }
}
