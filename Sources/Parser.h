
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
// Contents:  Interface for the ODBC to NATIVE SQL parser
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//

class ODBCParser;
typedef int (ODBCParser::*Function)(void);

typedef struct _parserWord
{
  char*    m_word;
  Function m_funct;
}
ParserWord;

class ODBCParser
{
public:
   ODBCParser();
  ~ODBCParser();
   void   MustParse(CString& p_sql,int& count_esc,int& count_par);
   void   Parse(CString& p_input,CString& p_output);
   char*  GetSQLState();

private:
  void SetError(char* p_error);
  void AppendOutput(CString& convert);
  void AppendOutput(const char *string);
  int  GetTerm();
  CString RecurseTerm();
  void GetWord(CString& p_word);
  int  DoubleEllipsisLeft();
  int  StdEscapeInitiate();
  int  StdEscapeTerminate();
  int  QckEscapeInitiate();
  int  QckEscapeTerminate();
  int  LeftOuterJoin();
  int  DateLiteral();
  int  TimeLiteral();
  int  TimeStampLiteral();
  int  IntervalLiteral();
  int  Guid();
  int  Escape();
  int  Vendor();
  int  CallFunc();
  int  CallProc();
  int  Funct();
  // GENERIC Convert
  int  FuncConvert();
  int  FuncCast();
  // STRING Functions
  int  FuncAscii();
  int  FuncBitLength();
  int  FuncChar();
  int  FuncCharLength();
  int  FuncConcat();
  int  FuncDifference();
  int  FuncInsert();
  int  FuncLcase();
  int  FuncLeft();
  int  FuncLength();
  int  FuncLocate();
  int  FuncLtrim();
  int  FuncOctetLength();
  int  FuncPosition();
  int  FuncWordIn();
  int  FuncRepeat();
  int  FuncReplace();
  int  FuncRight();
  int  FuncRtrim();
  int  FuncSoundex();
  int  FuncSpace();
  int  FuncSubstring();
  int  FuncUcase();
  // SYSTEM functions
  int  FuncDatabase();
  int  FuncIfnull();
  int  FuncUser();
  // NUMERIC Functions
  int  FuncAbs();
  int  FuncAcos();
  int  FuncAsin();
  int  FuncAtan();
  int  FuncAtan2();
  int  FuncCeiling();
  int  FuncCos();
  int  FuncCot();
  int  FuncDegrees();
  int  FuncExp();
  int  FuncFloor();
  int  FuncLog();
  int  FuncLog10();
  int  FuncMod();
  int  FuncPi();
  int  FuncPower();
  int  FuncRadians();
  int  FuncRand();
  int  FuncRound();
  int  FuncSign();
  int  FuncSin();
  int  FuncSqrt();
  int  FuncTan();
  int  FuncTruncate();
  // DATETIME FUNCTIONS
  int  FuncCurrentDate();
  int  FuncCurrentTime();
  int  FuncCurrentTimestamp();
  int  FuncCurdate();
  int  FuncCurtime();
  int  FuncDayname();
  int  FuncDayofmonth();
  int  FuncDayofweek();
  int  FuncDayofyear();
  int  FuncExtract();
  int  FuncHour();
  int  FuncMinute();
  int  FuncMonth();
  int  FuncMonthname();
  int  FuncNow();
  int  FuncQuarter();
  int  FuncSecond();
  int  FuncTimestampadd();
  int  FuncTimestampdiff();
  int  FuncWeek();
  int  FuncYear();

  static ParserWord m_parserWords  [];
  static ParserWord m_functionWords[];
  CString*  m_input;
  CString   m_term;
  int       m_termLength;
  int       m_pos;
  int       m_res;
  int       m_state;
  int       m_levelStd;
  int       m_levelQck;
  bool      m_keywordBreak;
  char      m_SQLSTATE[6];
};

inline char*
ODBCParser::GetSQLState()
{
  return &m_SQLSTATE[0];
}