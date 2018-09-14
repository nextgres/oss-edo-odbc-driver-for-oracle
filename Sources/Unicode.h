////////////////////////////////////////////////////////////////////////
//
// File:      $Archive: $
// Authors:   W.E. Huisman
// Contents:  UNICODE conversion of strings
//            From and back to SQLWCHAR conversions
//            Stacked conversion template
//
// EDO : Edo's Driver for Oracle
// ORACLE ODBC DRIVER for ODBC 3.51
//
// The Original Code was created by Vladimir Tsvigun for IBPhoenix.
// This code is an excerpt of that code, found in the Firbird ODBC driver
//
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "stdafx.h"
#include "MainObject.h"
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>

template <typename TypeRealLen = SQLSMALLINT>
class ConvertingString
{
  enum typestring { NONE, WIDECHARS, BYTESCHARS };

  SQLCHAR*      byteString;
  SQLWCHAR*     unicodeString;
  TypeRealLen*  realLength;
  int			      lengthString;
  typestring	  isWhy;
  bool		      returnCountOfBytes;

public:
  ConvertingString() 
  {
    returnCountOfBytes = true;
    isWhy         = NONE;
    unicodeString = NULL;
    byteString    = NULL;
    realLength    = NULL;
    lengthString  = 0;
  }

  ConvertingString(int length,SQLWCHAR* wcString,TypeRealLen* pLength = NULL,bool retCountOfBytes = true)
  {
    realLength = pLength;
    returnCountOfBytes = retCountOfBytes;

    if(wcString)
    {
      isWhy = BYTESCHARS;
      unicodeString = wcString;
      if(length == SQL_NTS )
      {
        lengthString = 0;
      }
      else if(retCountOfBytes)
      {
        lengthString = length / 2;
      }
      else
      {
        lengthString = length;
      }
    }
    else
    {
      isWhy = NONE;
    }
    Alloc();
  }

  ConvertingString(SQLWCHAR* wcString,int length)
  {
    realLength    = NULL;
    unicodeString = NULL;
    returnCountOfBytes = true;
    isWhy         = BYTESCHARS;

    if(wcString)
    {
      convUnicodeToString(wcString,length);
    }
    else
    {
      byteString = NULL;
      lengthString = 0;
    }
  }

  operator SQLCHAR*()	    { return byteString;   }
  operator int()          { return lengthString; }
  operator SQLSMALLINT()  { return (SQLSMALLINT) lengthString; }
  operator SQLINTEGER()   { return (SQLINTEGER)  lengthString; }

  ~ConvertingString() 
  {
    switch(isWhy)
    {
    case BYTESCHARS:

      if(unicodeString)
      {
        size_t len;
        len = MultiByteToWideChar(codePage,0,(const char*)byteString,-1,unicodeString,lengthString);
        if(len > 0)
        {
          --len;
        }
        if(len > 0)
        {
          *(LPWSTR)(unicodeString + len) = L'\0';
          if(realLength)
          {
            if(returnCountOfBytes)
            {
              *realLength = (TypeRealLen)(len*2);
            }
            else
            {
              *realLength = (TypeRealLen)len;
            }
          }
        }
      }
      delete[] byteString;
      break;

    case NONE:
      if ( realLength && returnCountOfBytes )
      {
        *realLength *= 2;
      }
      break;
    }
  }

  SQLCHAR* convUnicodeToString(SQLWCHAR* wcString,int length)
  {
    size_t   bytesNeeded;
    wchar_t* ptEndWC = NULL;
    wchar_t  saveWC  = 0;

    if(length == SQL_NTS)
    {
      length = (int)wcslen((const wchar_t*)wcString);
    }
    else if(wcString[length] != L'\0')
    {
      ptEndWC = (wchar_t*)&wcString[length];
      saveWC = *ptEndWC;
      *ptEndWC = L'\0';
    }
    bytesNeeded = WideCharToMultiByte(codePage,(DWORD)0,wcString,length,NULL,(int)0,NULL,NULL);
    byteString = new SQLCHAR[bytesNeeded + 2];
    WideCharToMultiByte(codePage,0,wcString,length,(LPSTR)byteString,(int)bytesNeeded,NULL,NULL);

    byteString[bytesNeeded] = '\0';
    lengthString = (int)bytesNeeded;
    if(ptEndWC)
    {
      *ptEndWC = saveWC;
    }
    return byteString;
  }

protected:
  void Alloc()
  {
    switch(isWhy)
    {
    case BYTESCHARS:  if(lengthString)
                      {
                        byteString = new SQLCHAR[lengthString + 2];
                      }
                      else
                      {
                        byteString = NULL;
                      }
                      break;
    case NONE:        unicodeString = NULL;
      byteString    = NULL;
      lengthString  = 0;
      break;
    }
  }
};

