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
// Contents:  Interface for the Error info
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//

#pragma once
#include "Descriptor.h"

SQLRETURN   TranslateOCIReturnCodeToSQLReturnCode(signed int status);
const char* GetHandleTypeDescription(SQLSMALLINT HT);
const char* GetOraTypesEnumerationInString(OraTypesEnumeration Type);
const char* FindODBCErrorText(const char *SQLState,int &InternalCode);
const char* ERROR_GetConnectAttribute(SQLINTEGER p_attribute);
const char* ERROR_GetEnvAttribute(SQLINTEGER p_attribute);
const char* ERROR_GetStmtAttribute(SQLINTEGER p_attribute);

