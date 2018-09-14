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
// Contents:  Interface for the attributes of the setup DLL
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#pragma once
#include <map>

#define ATTRIB_DSN          "DSN"
#define ATTRIB_USERID       "UID"
#define ATTRIB_PASSWORD     "PWD"
#define ATTRIB_SERVERNAME   "ServerName"
#define ATTRIB_TRACEFILE    "Tracefile"   
#define ATTRIB_TRACELEVEL   "TraceLevel"  
#define ATTRIB_DESCRIPTION  "Description" 
#define ATTRIB_READONLY     "Readonly"
#define ATTRIB_DRIVER       "Driver"
#define ATTRIB_STATISTICS   "StatisticsEnsure"
#define ATTRIB_PRECOUNT     "PrefetchCount"
#define ATTRIB_PREMEMORY    "PrefetchMemory"
#define ATTRIB_FAILCOUNT    "FailoverCount"
#define ATTRIB_FAILTIME     "FailoverTime"
#define ATTRIB_METADATAID   "MetaDataID"
#define ATTRIB_COLUMNMAXLEN "ColumnMaxLength"
#define ATTRIB_TRANSOPTION  "TranslationOption"
#define ATTRIB_TRANSLIB     "TranslationLibrary"
#define ATTRIB_INTSPEEDUP   "IntegerSpeedUp"

#define ATTRIB_NLSTYPE        "NLSType"
#define ATTRIB_NLSDATE        "NLS_DATE_FORMAT"
#define ATTRIB_NLSTIME        "NLS_TIME_FORMAT"
#define ATTRIB_NLSTIMESTAMP   "NLS_TIMESTAMP_FORMAT"
#define ATTRIB_NLSTIMESTAMPTZ "NLS_TIMESTAMP_TZ_FORMAT"

typedef std::map<CString,CString> AttribMap;

class Attributes
{
public:
   Attributes(HWND p_hwnd,CString p_dataSource);
  ~Attributes();
  void    SetDatasource(CString p_datasource);
  bool    ReadAllAttributes();
  bool    WriteAllAttributes();
  CString MakeConnectionString();
  // Normal attributes
  CString GetAttribute(CString p_name);
  void    SetAttribute(CString p_name,CString p_value);
private:
  HWND      m_hwnd;
  CString   m_dataSource;
  AttribMap m_attribs;
};

inline void
Attributes::SetDatasource(CString p_datasource)
{
  m_dataSource = p_datasource;
}