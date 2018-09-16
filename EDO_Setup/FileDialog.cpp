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
// Contents:  General wrapper for the MFC CFileDialog 
//            (To help me from repeating my previous mistakes)
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "StdAfx.h"
#include "EDO_Setup.h"
#include "FileDialog.h"
#include <dlgs.h>

#pragma warning (disable:4312)

DocFileDialog::DocFileDialog(bool    p_open        // true = open, false = SaveAs
                            ,CString p_title       // Title of the dialog
                            ,CString p_defext      // Default extension
                            ,CString p_filename    // Default first file
                            ,int     p_flags       // Default flags
                            ,CString p_filter      // Filter for extensions
                            ,CString p_direct)     
              :m_open(p_open)
{
  if(p_filter.IsEmpty())
  {
    p_filter = "Text files (*.txt)|*.txt|";
  }
  // Register original CWD (Current Working Directory)
  GetCurrentDirectory(MAX_PATH, m_original);
  if(!p_direct.IsEmpty())
  {
    // Change to starting directory
    SetCurrentDirectory(p_direct.GetString());
  }
  strncpy_s(m_filter,  1024,   p_filter,  1024);
  strncpy_s(m_filename,MAX_PATH,p_filename,MAX_PATH);
  strncpy_s(m_defext,  100,    p_defext,  100);
  strncpy_s(m_title,   100,    p_title,   100);
  FilterString(m_filter);

  // Fill in the filename structure
  p_flags |= OFN_ENABLESIZING | OFN_LONGNAMES | OFN_NOCHANGEDIR |  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  p_flags &= ~(OFN_NODEREFERENCELINKS | OFN_NOLONGNAMES | OFN_NOTESTFILECREATE);

  m_ofn.lStructSize       = sizeof(OPENFILENAME);
  m_ofn.hwndOwner         = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
  m_ofn.hInstance         = (HINSTANCE) GetWindowLongPtr(m_ofn.hwndOwner,GWLP_HINSTANCE);
  m_ofn.lpstrFile         = (LPSTR) m_filename;
  m_ofn.lpstrDefExt       = (LPSTR) m_defext;
  m_ofn.lpstrTitle        = (LPSTR) m_title;
  m_ofn.lpstrFilter       = (LPSTR) m_filter;
  m_ofn.Flags             = p_flags;
  m_ofn.nFilterIndex      = 1;    // Use lpstrFilter
  m_ofn.nMaxFile          = MAX_PATH;
  m_ofn.lpstrCustomFilter = NULL; //(LPSTR) buf_filter;
  m_ofn.nMaxCustFilter    = 0;
  m_ofn.lpstrFileTitle    = NULL;
  m_ofn.nMaxFileTitle     = 0;
  m_ofn.lpstrInitialDir   = NULL;
  m_ofn.nFileOffset       = 0;
  m_ofn.lCustData         = NULL;
  m_ofn.lpfnHook          = NULL;
  m_ofn.lpTemplateName    = NULL;
}

DocFileDialog::~DocFileDialog()
{
  // Go back to the original directory
  SetCurrentDirectory(m_original);
}

#pragma warning(disable:4702)
int 
DocFileDialog::DoModal()
{
  int res = IDCANCEL;
  try
  {
    if(m_open)
    {
      res = GetOpenFileName(&m_ofn);
    }
    else
    {
      res = GetSaveFileName(&m_ofn);
    }
  }
  catch(...)
  {
    ::MessageBox(NULL,"Cannot create a file dialog","ERROR",MB_OK|MB_ICONHAND);
  }
  return res;
}

CString 
DocFileDialog::GetChosenFile()
{
  return (CString) m_ofn.lpstrFile;
}

void
DocFileDialog::FilterString(char *filter)
{
  char *pnt = filter;
  while(*pnt)
  {
    if(*pnt == '|')
    {
      *pnt = 0;
    }
    ++pnt;
  }
  *pnt++ = 0;
  *pnt++ = 0;
}
