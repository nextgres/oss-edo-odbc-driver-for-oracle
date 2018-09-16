// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				  // Allow use of features specific to Windows Vista or later.
#define WINVER 0x0601		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		    // Allow use of features specific to Windows Vista or later.                   
#define _WIN32_WINNT 0x0601	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		    // Allow use of features specific to Windows Vista or later.
#define _WIN32_WINDOWS 0x0601 // Change this to the appropriate value to target other versions
#endif

#ifndef _WIN32_IE			    // Allow use of features specific to IE 8.0 or later.
#define _WIN32_IE 0x0800	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


// EDO
// We are now building a ODBC 3.51 version driver !!!
// This will get the correct header information from <sql.h> and <sqlext.h> 
#define ODBCVER 0x0351

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// EDO
//
// SPECIAL MACRO'S GO HERE
//
#include "..\res\resource.h"

// comment out this line to build without a logging mechanism!
#define EDO_LOGGING
// comment out this line to built without logging of columns and params
// #define EDO_COLUMN_LOGGING

#ifdef EDO_LOGGING
#define WRITELOG(p_format, ...) if(ODBCDriver.LogSystemOn())\
{\
  ODBCDriver.PrintLog(p_format,__VA_ARGS__);\
}
#else
#define WRITELOG(p_format,...)
#endif

#pragma warning (disable: 4121)   // Alignment sensitive to packing
#pragma warning (disable: 4239)   // Nonstandard Unicode to string conversion