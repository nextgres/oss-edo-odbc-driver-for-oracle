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
// Contents:  EDO Specific attributes for a connection
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once

// Driver specific connection attributes for SQLSetConnectAttr/SQLGetConnectAttr
#define SQL_ATTR_EDO_ENSURE_STATISTICS  8000
#define SQL_ATTR_EDO_PREFECTH_COUNT     8001
#define SQL_ATTR_EDO_PREFETCH_MEMORY    8002
#define SQL_ATTR_EDO_FAILOVER_COUNT     8003
#define SQL_ATTR_EDO_FAILOVER_TIME      8004
#define SQL_ATTR_EDO_TRACE              8005
#define SQL_ATTR_EDO_TRACEFILE          8006
#define SQL_ATTR_EDO_INTEGERSPEEDUP     8007
