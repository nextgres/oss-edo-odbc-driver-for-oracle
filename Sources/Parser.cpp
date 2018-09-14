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
// Contents:  ODBC to NATIVE SQL parser
//
// File       : $Archive:  $
// Last change: $JustDate: $
// Changed by : $Author:   $
// Revision   : $Revision: $
//
#include "StdAfx.h" 
#include "Parser.h" 

// Basic tests for all escape sequences
// SELECT * FROM dual WHERE dummy LIKE 'X' --(* VENDOR(Microsoft), PRODUCT(odbc) escape '@' *)-- 
// SELECT * FROM dual WHERE dummy LIKE 'X' --(* escape '@' *)-- 
// SELECT * FROM dual WHERE dummy LIKE 'X'   {  escape '@' }
// SELECT { * } from dual;
// SELECT * FROM { oj kbs_lan lan LEFT OUTER JOIN kbs_ew ew ON ew.oid = lan.ew_oid } WHERE oid < 200
// SELECT { d'2008-30-07'} FROM dual
// SELECT { t'12:34:47'}   FROM dual
// SELECT { ts'1991-08-16 22:55:00'} AS remi FROM dual
// SELECT { ts'1997-07-30 22:34:47'} AS tara FROM dual
// { call procedure()}
// SELECT { fn CURRATE()} FROM dual

ParserWord
ODBCParser::m_parserWords[] =
{
  { "{"       ,&ODBCParser::QckEscapeInitiate  }     // Quick escape initiator
 ,{ "}"       ,&ODBCParser::QckEscapeTerminate }     // Quick escape terminator
 ,{ "oj"      ,&ODBCParser::LeftOuterJoin      }     // Outer join syntax
 ,{ "fn"      ,&ODBCParser::Funct              }     // Function call syntax
 ,{ "call"    ,&ODBCParser::CallProc           }     // Procedure call syntax
 ,{ "?"       ,&ODBCParser::CallFunc           }     // Function  call syntax
 ,{ "d"       ,&ODBCParser::DateLiteral        }     // Date literal
 ,{ "t"       ,&ODBCParser::TimeLiteral        }     // Time literal
 ,{ "ts"      ,&ODBCParser::TimeStampLiteral   }     // Timestamp literal
 ,{ "interval",&ODBCParser::IntervalLiteral    }     // Interval literal
 ,{ "guid"    ,&ODBCParser::Guid               }     // GUID literal
 ,{ "escape"  ,&ODBCParser::Escape             }     // Escape clause
 ,{ "vendor"  ,&ODBCParser::Vendor             }     // Vendor clause
 ,{ "--(*"    ,&ODBCParser::StdEscapeInitiate  }     // Standard escape initiator
 ,{ "*)"      ,&ODBCParser::StdEscapeTerminate }     // Standard escape terminator
 ,{ "in"      ,&ODBCParser::FuncWordIn         }     // Keyword POSITION(term IN term)
};

ParserWord
ODBCParser::m_functionWords[] = 
{
  // GENERIC Conversion function
  { "convert"           ,&ODBCParser::FuncConvert        }
 ,{ "cast"              ,&ODBCParser::FuncCast            }
  // STRING FUNCTIONS
 ,{ "ascii"             ,&ODBCParser::FuncAscii           }
 ,{ "bit_length"        ,&ODBCParser::FuncBitLength       }
 ,{ "char"              ,&ODBCParser::FuncChar            }
 ,{ "char_length"       ,&ODBCParser::FuncCharLength      }
 ,{ "character_length"  ,&ODBCParser::FuncCharLength      }
 ,{ "concat"            ,&ODBCParser::FuncConcat          }
 ,{ "difference"        ,&ODBCParser::FuncDifference      }
 ,{ "insert"            ,&ODBCParser::FuncInsert          }
 ,{ "lcase"             ,&ODBCParser::FuncLcase           }
 ,{ "left"              ,&ODBCParser::FuncLeft            }
 ,{ "length"            ,&ODBCParser::FuncLength          }
 ,{ "locate"            ,&ODBCParser::FuncLocate          }
 ,{ "ltrim"             ,&ODBCParser::FuncLtrim           }
 ,{ "octet_length"      ,&ODBCParser::FuncOctetLength     }
 ,{ "position"          ,&ODBCParser::FuncPosition        }
 ,{ "repeat"            ,&ODBCParser::FuncRepeat          }
 ,{ "replace"           ,&ODBCParser::FuncReplace         }
 ,{ "right"             ,&ODBCParser::FuncRight           }
 ,{ "rtrim"             ,&ODBCParser::FuncRtrim           }
 ,{ "soundex"           ,&ODBCParser::FuncSoundex         }
 ,{ "space"             ,&ODBCParser::FuncSpace           }
 ,{ "substring"         ,&ODBCParser::FuncSubstring       }
 ,{ "ucase"             ,&ODBCParser::FuncUcase           }
 // SYSTEM FUNCTIONS
 ,{ "database"          ,&ODBCParser::FuncDatabase        }
 ,{ "ifnull"            ,&ODBCParser::FuncIfnull          }
 ,{ "user"              ,&ODBCParser::FuncUser            }
 // NUMERIC FUNCTIONS
 ,{ "abs"               ,&ODBCParser::FuncAbs             }
 ,{ "acos"              ,&ODBCParser::FuncAcos            }
 ,{ "asin"              ,&ODBCParser::FuncAsin            }
 ,{ "atan"              ,&ODBCParser::FuncAtan            }
 ,{ "atan2"             ,&ODBCParser::FuncAtan2           }
 ,{ "ceiling"           ,&ODBCParser::FuncCeiling         }
 ,{ "cos"               ,&ODBCParser::FuncCos             }
 ,{ "cot"               ,&ODBCParser::FuncCot             }
 ,{ "degrees"           ,&ODBCParser::FuncDegrees         }
 ,{ "exp"               ,&ODBCParser::FuncExp             }
 ,{ "floor"             ,&ODBCParser::FuncFloor           }
 ,{ "log"               ,&ODBCParser::FuncLog             }
 ,{ "log10"             ,&ODBCParser::FuncLog10           }
 ,{ "mod"               ,&ODBCParser::FuncMod             }
 ,{ "pi"                ,&ODBCParser::FuncPi              }
 ,{ "power"             ,&ODBCParser::FuncPower           }
 ,{ "radians"           ,&ODBCParser::FuncRadians         }
 ,{ "rand"              ,&ODBCParser::FuncRand            }
 ,{ "round"             ,&ODBCParser::FuncRound           }
 ,{ "sign"              ,&ODBCParser::FuncSign            }
 ,{ "sin"               ,&ODBCParser::FuncSin             }
 ,{ "sqrt"              ,&ODBCParser::FuncSqrt            }
 ,{ "tan"               ,&ODBCParser::FuncTan             }
 ,{ "truncate"          ,&ODBCParser::FuncTruncate        }
 // DATE/TIME FUNCTIONS
 ,{ "current_date"      ,&ODBCParser::FuncCurrentDate     }
 ,{ "current_time"      ,&ODBCParser::FuncCurrentTime     }
 ,{ "current_timestamp" ,&ODBCParser::FuncCurrentTimestamp}
 ,{ "curdate"           ,&ODBCParser::FuncCurdate         }
 ,{ "curtime"           ,&ODBCParser::FuncCurtime         }
 ,{ "dayname"           ,&ODBCParser::FuncDayname         }
 ,{ "dayofmonth"        ,&ODBCParser::FuncDayofmonth      }
 ,{ "dayofweek"         ,&ODBCParser::FuncDayofweek       }
 ,{ "dayofyear"         ,&ODBCParser::FuncDayofyear       }
 ,{ "extract"           ,&ODBCParser::FuncExtract         }
 ,{ "hour"              ,&ODBCParser::FuncHour            }
 ,{ "minute"            ,&ODBCParser::FuncMinute          }
 ,{ "month"             ,&ODBCParser::FuncMonth           }
 ,{ "monthname"         ,&ODBCParser::FuncMonthname       }
 ,{ "now"               ,&ODBCParser::FuncNow             }
 ,{ "pi"                ,&ODBCParser::FuncPi              }
 ,{ "quarter"           ,&ODBCParser::FuncQuarter         }
 ,{ "second"            ,&ODBCParser::FuncSecond          }
 ,{ "timestampadd"      ,&ODBCParser::FuncTimestampadd    }
 ,{ "timestampdiff"     ,&ODBCParser::FuncTimestampdiff   }
 ,{ "week"              ,&ODBCParser::FuncWeek            }
 ,{ "year"              ,&ODBCParser::FuncYear            }
};

#define PARSER_WORDS   (sizeof(m_parserWords)  /sizeof(ParserWord))
#define FUNCTION_WORDS (sizeof(m_functionWords)/sizeof(ParserWord))

ODBCParser::ODBCParser()
           :m_state(0)
           ,m_levelStd(0)
           ,m_levelQck(0)
           ,m_input(NULL)
           ,m_res(0)
           ,m_pos(0)
           ,m_termLength(0)
{
  m_SQLSTATE[0] = 0;
}

ODBCParser::~ODBCParser()
{
}

void 
ODBCParser::SetError(char* p_error)
{
  // Record only the FIRST error. 
  // Following errors are not registered
  if(m_SQLSTATE[0] == 0)
  {
    strncpy_s(m_SQLSTATE,6,p_error,5);
    m_SQLSTATE[5] = 0;
  }
}

void
ODBCParser::AppendOutput(CString& convert)
{
  for(int i = 0; i < convert.GetLength(); ++i)
  {
    m_term.SetAt(m_res++,convert.GetAt(i));
  }
}

void
ODBCParser::AppendOutput(const char *string)
{
  while(*string)
  {
    m_term.SetAt(m_res++,*string++);
  }
}

// See if we must parse one of the following:
// - An escape sequence "{ <code> something }" with generic ODBC syntax
// - A parameter binding sequence
void
ODBCParser::MustParse(CString& p_sql,int& count_esc,int& count_par)
{
  const char*  sql  = p_sql.GetString();
  const char*  last = sql;
	
  count_esc = 0;
  count_par = 0;
  while(*sql)
	{
		if(*sql == '\'')
		{
      // Parse string
      while(*(++sql))
      {
        // we detect end of text '' section
        if(*sql == '\'') 
        {
          break;
        }
      }
		}
    if(*sql == '{')
		{
			++count_esc;
		}
    else if(*sql == '?')
    {
      ++count_par;
    }
    else if(*last == '(' && *sql == '*')
    {
      ++count_esc;
    }
    last = sql++;
	}
}

void 
ODBCParser::Parse(CString& p_input,CString& p_output)
{
  int length = p_input.GetLength();
  m_termLength = 2 * length + 100;
  m_pos    = 0;
  m_res    = 0;
  m_input  = &p_input;

  while(GetTerm())
  {
    p_output += m_term + m_input->GetAt(m_pos-1);
    m_term    = "";
    m_res     = 0;
  }
  p_output += m_term;

  // Perform tests
  if(m_levelStd || m_levelQck)
  {
    // Level of escape sequence left hanging
    SetError("HYD00");
  }
}

int
ODBCParser::GetTerm()
{
  int c = 0;
  bool found;
  CString word;
  m_keywordBreak = false;

  m_term.GetBufferSetLength(m_termLength);
  while((c = m_input->GetAt(m_pos)) != 0)
  {
    // Empty space and red-space
    if(isspace(c) || c < 32)
    {
      m_term.SetAt(m_res++,(char) c);
      c = m_input->GetAt(++m_pos);
      continue;
    }
    // Semantic word
    found = false;
    GetWord(word);
    if(word == "," || word == ")")
    {
      // END OF THE TERM
      m_term.SetAt(m_res,0);
      m_term.ReleaseBuffer();
      return 1;
    }
    for(int i = 0; i < PARSER_WORDS; ++i)
    {
      if(word.CompareNoCase(m_parserWords[i].m_word) == 0)
      {
        Function func = m_parserWords[i].m_funct;
        if(func)
        {
          if((this->*(func))())
          {
            found = true;
          }
          break;
        }
      }
    }
    if(m_keywordBreak)
    {
      // END OF THE TERM
      m_term.SetAt(m_res,0);
      m_term.ReleaseBuffer();
      return 1;
    }
    if(found)
    {
      continue;
    }
    // Append none parser words to output
    AppendOutput(word);
  }
  m_term.SetAt(m_res,0);
  m_term.ReleaseBuffer();
  return 0;
}

// BASIC TEST: SELECT { fn CONVERT( { fn CURDATE()}, SQL_VARCHAR )} FROM dual
CString
ODBCParser::RecurseTerm()
{
  m_term.SetAt(m_res,0);
  m_term.ReleaseBuffer();
  CString onStack = m_term;
  int res = m_res;
  m_term = "";
  m_res  = 0;
  GetTerm();
  --m_pos; // Reparse at next get with GetWord
  CString term = m_term;
  m_term = onStack;
  m_term.GetBufferSetLength(m_termLength);
  m_res = res;
  
  return term;
}

// Gets a word or a string
void
ODBCParser::GetWord(CString& p_word)
{
  p_word = "";
  int ch = m_input->GetAt(m_pos);
  while(ch && ch <= ' ')
  {
    ch = m_input->GetAt(++m_pos);
  }
  if(ch == '\'')
  {
    p_word = (char)ch;
    do
    {
      do 
      {
        ch = m_input->GetAt(++m_pos);
        p_word += (unsigned char)ch;
      } 
      while(ch && ch != '\'');
      if(ch) 
      {
        ch = m_input->GetAt(++m_pos);
        if(ch == '\'')
        {
          p_word += (unsigned char)ch;
        }
      }
    }
    while(ch == '\'');
    return;
  }
  int alpha = isalnum(ch) || (ch == '_');
  do 
  {
    p_word += (unsigned char)ch;
    if(ch=='}' || ch=='{' || ch == ',' || ch == ')')
    {
      ++m_pos;
      break;
    }
    ch = m_input->GetAt(++m_pos);
    if(ch==0 || ch==' ' || ch=='\r' || ch=='\n' || ch=='\'' || ch=='}' || ch=='{')
    {
      break;
    }
  } 
  while((isalnum(ch) || (ch=='_')) == alpha);
}

//////////////////////////////////////////////////////////////////////////
//
// ESCAPES
//
//////////////////////////////////////////////////////////////////////////

int
ODBCParser::StdEscapeInitiate()
{
  ++m_levelStd;
  return 1;
}

int
ODBCParser::StdEscapeTerminate()
{
  // *)-- cannot be parsed at once
  // because the way GetWord() works.
  int oldpos = m_pos;
  int c = m_input->GetAt(m_pos);
  if(c != '-')
  {
    return 0;
  }
  c = m_input->GetAt(++m_pos);
  if(c != '-')
  {
    m_pos = oldpos;
    return 0;
  }
  ++m_pos;
  // Level ok.
  --m_levelStd;
  return 1;
}

int  
ODBCParser::QckEscapeInitiate()
{
  ++m_levelQck;
  return 1;
}

int  
ODBCParser::QckEscapeTerminate()
{
  --m_levelQck;
  return 1;
}

// I've never seen anyone using this ridiculous escape sequence
// Apparently it's in the ODBC standard to remind us of the fact
// that it is a Microsoft standard.
int
ODBCParser::Vendor()
{
  CString word;

  if(m_levelStd == 0 && m_levelQck == 0)
  {
    // General syntax error
    SetError("42000");
    return 0;
  }
  GetWord(word);
  if(word == "(")
  {
    GetWord(word);
    if(word.CompareNoCase("Microsoft") == 0)
    {
      GetWord(word);
      if(word == ")")
      {
        GetWord(word);
        if(word == ",")
        {
          while(isspace(m_input->GetAt(m_pos))) ++m_pos;
          GetWord(word);
          if(word.Compare("PRODUCT") == 0)
          {
            GetWord(word);
            if(word == "(")
            {
              GetWord(word);
              if(word.CompareNoCase("odbc") == 0)
              {
                GetWord(word);
                if(word == ")")
                {
                  return 1;
                }
              }
            }
          }
        }
      }
    }
  }
  SetError("22007");
  return 0;
}

int
ODBCParser::LeftOuterJoin()
{
  if(m_levelStd == 0 && m_levelQck == 0)
  {
    // General syntax error
    SetError("42000");
    return 0;
  }
  return 1;
}

int
ODBCParser::DateLiteral()
{
  // Check for literal
  int oldpos = m_pos;
  while(isspace(m_input->GetAt(m_pos))) ++m_pos;
  if(m_input->GetAt(m_pos) != '\'')
  {
    m_pos = oldpos;
    return 0;
  }
  CString word;
  GetWord(word); // Must be 'yyyy-mm-dd'
  CString convert = "TO_DATE(" + word + ",'YYYY-MM-DD')";
  AppendOutput(convert);
  return 1;
}

int
ODBCParser::TimeLiteral()
{
  // Check for literal
  int oldpos = m_pos;
  while(isspace(m_input->GetAt(m_pos))) ++m_pos;
  if(m_input->GetAt(m_pos) != '\'')
  {
    m_pos = oldpos;
    return 0;
  }
  CString word;
  GetWord(word); // Must be 'HH:MM:SS'
  CString convert = "TO_TIMESTAMP(" + word + ",'HH24:MI:SS')";
  AppendOutput(convert);
  return 1;
}

int
ODBCParser::TimeStampLiteral()
{
  // Check for literal
  int oldpos = m_pos;
  while(isspace(m_input->GetAt(m_pos))) ++m_pos;
  if(m_input->GetAt(m_pos) != '\'')
  {
    m_pos = oldpos;
    return 0;
  }
  // Must be 'YYYY-MM-DD HH:MM:SS'
  CString word;
  GetWord(word);
  CString convert = "TO_TIMESTAMP(" + word + ",'YYYY-MM-DD HH24:MI:SS')";
  AppendOutput(convert);
  return 1;
}

int
ODBCParser::IntervalLiteral()
{
  // { interval 'nnn nnn:nnn:nnn' LEAD[(n)] TO LOWEST }
  return 0;
}

int
ODBCParser::Guid()
{
  // { guid 'nnnnnnnn-nnnn-nnnn-nnnn-nnnnnnnnnnnn'} => 'nnnnnnnn-nnnn-nnnn-nnnn-nnnnnnnnnnnn'
  return 0;
}

int 
ODBCParser::Escape()
{
  // { escape '@' } => escape '@'
  return 0;
}

// { call procedure[(par,...)] } => call procedure[(par,...)]
int
ODBCParser::CallProc()
{
  if(m_levelStd == 0 && m_levelQck == 0)
  {
    // Do nothing
    return 0;
  }
  CString total;
  CString word;
  GetWord(total); // Procedure or function name
  total = "call " + total;
  GetWord(word);
  if(word == "(")
  {
    while(!word.IsEmpty() && word != ")")
    {
      total += word;
      CString term = RecurseTerm();
      total += term;
      GetWord(word);
    }
  }
  if(word == ")" || word == "()")
  {
    total += word;
    AppendOutput(total);
    return 1;
  }
  SetError("42000");
  return 0;
}

// { ? = function([par,...]) } =>   BEGIN ? = function([par,...]); END;
int
ODBCParser::CallFunc()
{
  if(m_levelStd == 0 && m_levelQck == 0)
  {
    // Do nothing
    return 0;
  }
  CString total;
  CString word;
  GetWord(word);  // =
  if(word == "=")
  {
    GetWord(total); // Procedure or function name
    GetWord(word);
    if(word == "(")
    {
      while(!word.IsEmpty() && word != ")")
      {
        total += word;
        CString term = RecurseTerm();
        total += term;
        GetWord(word);
      }
    }
    if(word == ")" || word == "()")
    {
      total += word;
      total = "BEGIN ? = " + total + "; END;";
      AppendOutput(total);
      return 1;
    }
  }
  SetError("42000");
  return 0;
}

int
ODBCParser::Funct()
{
  CString word;
  GetWord(word);  // Name of the function
  int res = 0;
  for(int i = 0; i < FUNCTION_WORDS; ++i)
  {
    if(word.CompareNoCase(m_functionWords[i].m_word) == 0)
    {
      Function func = m_functionWords[i].m_funct;
      if(func)
      {
        res = (this->*(func))();
        if(res == 0)
        {
          AppendOutput(word);
        }
        return 1;
      }
    }
  }
  // Unknown {fn function()} code
  SetError("HYD01");
  return 0;
}

int
ODBCParser::DoubleEllipsisLeft()
{
  CString word;
  GetWord(word);
  if(word == "()")
  {
    return 1;
  }
  if(word == "(")
  {
    GetWord(word);
    if(word == ")")
    {
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

//////////////////////////////////////////////////////////////////////////
//
//  FUNCTION CODE
//
//////////////////////////////////////////////////////////////////////////
//
// BASIC FUNCTION TESTS
//
//////////////////////////////////////////////////////////////////////////

// SELECT { fn Ascii('a') } FROM dual;
int
ODBCParser::FuncAscii()
{
  // ASCII(asc) -> ASCII(asc)
  return 0;
}

// SELECT { fn bit_length(dummy) } FROM dual;
int
ODBCParser::FuncBitLength()
{
  // BIT_LENGTH(term) -> LENGTHB * 8
  CString word("(8*LENGTHB");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    AppendOutput(word);
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(")");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn CHAR(97)} FROM dual;
int
ODBCParser::FuncChar()
{
  // CHAR -> CHR
  CString word("CHR");
  AppendOutput(word);
  return 1;
}

// SELECT { fn CHAR_LENGTH('monkey')} FROM dual;
// SELECT { fn CHARACTER_LENGTH('monkey')} FROM dual;
int
ODBCParser::FuncCharLength()
{
  // CHAR_LENGTH      -> LENGTHC
  // CHARACTER_LENGTH -> LENGTHC
  CString word("LENGTHC");
  AppendOutput(word);
  return 1;
}

// SELECT { fn CONCAT('Gorilla ','business')} from dual;
// The next returns 'Gorilla ', and not null
// SQLGetInfo with SQL_CONCAT_NULL_BEHAVIOR returns SQL_CB_NON_NULL
// SELECT { fn CONCAT('Gorilla ',NULL)} from dual;
int
ODBCParser::FuncConcat()
{
  // CONCAT(a,b) -> CONCAT(a,b)
  return 0;
}

int
ODBCParser::FuncDifference()
{
  // Don't know how to implement this
  // Difference between two SOUNDEX "ABCD" values
  // General syntax error.
  SetError("42000");
  return 0;
}

// SELECT { fn INSERT('abcdefghijk',4,2,'xy')} from dual;
// returns 'abcxyfghijk'
int
ODBCParser::FuncInsert()
{
  // INSERT(string,start,len,exp) -> SUBSTR(string,1,start-1) || exp || SUBSTR(string,start+len)
  CString word("SUBSTR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString string = RecurseTerm();
    GetWord(word);
    if(word == ",")
    {
      CString start  = RecurseTerm();
      GetWord(word);
      if(word == ",")
      {
        CString length = RecurseTerm();
        GetWord(word);
        if(word == ",")
        {
          CString expression = RecurseTerm();

          // Now build string
          AppendOutput(string);
          AppendOutput(",1,");
          AppendOutput(start);
          AppendOutput("-1)||");
          AppendOutput(expression);
          AppendOutput("||SUBSTR(");
          AppendOutput(string);
          AppendOutput(",");
          AppendOutput(start);
          AppendOutput("+");
          AppendOutput(length);
          GetWord(word);
          if(word == ")")
          {
            AppendOutput(word);
            return 1;
          }
        }
      }
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn LCASE('ABC')} FROM dual;
int
ODBCParser::FuncLcase()
{
  // LCASE -> LOWER
  CString word("LOWER");
  AppendOutput(word);
  return 1;
}

// SELECT { fn LEFT('ABCDEF',3)} FROM dual;
int
ODBCParser::FuncLeft()
{
  // LEFT(string,count) -> SUBSTR(string,1,count)
  CString word("SUBSTR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString string = RecurseTerm();
    AppendOutput(string);
    GetWord(word);
    if(word == ",")
    {
      CString count = RecurseTerm();
      AppendOutput(",1,");
      AppendOutput(count);
      GetWord(word);
      if(word == ")")
      {
        AppendOutput(word);
        return 1;
      }
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn Length('ABCDEF')} FROM dual;
int
ODBCParser::FuncLength()
{
  // LENGTH -> LENGTH
  return 0;
}

// SELECT { fn locate('klm','abcdefghijklm',4)} FROM dual;
// SELECT { fn locate('klm','abcdefghijklm')} FROM dual;
int
ODBCParser::FuncLocate()
{
  // LOCATE(string1,string2 [,start]) -> INSTR(string2,string1[,start])
  CString word("INSTR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    
    CString string1 = RecurseTerm();
    GetWord(word);
    if(word == ",")
    {
      CString start;
      CString string2 = RecurseTerm();
      GetWord(word);
      if(word == ",")
      {
        start = RecurseTerm();
        GetWord(word);
      }
      if(word == ")")
      {
        AppendOutput(string2);
        AppendOutput(",");
        AppendOutput(string1);
        if(!start.IsEmpty())
        {
          AppendOutput(",");
          AppendOutput(start);
        }
        AppendOutput(word);
        return 1;
      }
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn LTRIM('   abc')} FROM dual;
int
ODBCParser::FuncLtrim()
{
  // LTRIM -> LTRIM
  return 0;
}

// SELECT { fn OCTET_LENGTH('ABCDEFGHIJ')} FROM dual;
int
ODBCParser::FuncOctetLength()
{
  // OCTET_LENGTH -> LENGTHB
  CString word("LENGTHB");
  AppendOutput(word);
  return 1;
}

int 
ODBCParser::FuncWordIn()
{
  if(m_levelStd >= 0 || m_levelQck >= 0)
  {
    m_keywordBreak = true;
    return 1;
  }
  return 0;
}

// SELECT {fn POSITION('klm' IN 'abcdefghijklmnop')} from dual;
int
ODBCParser::FuncPosition()
{
  // POSITION(substr IN string) => INSTR(string,substr)
  CString word("INSTR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString substr = RecurseTerm();
    if(m_keywordBreak)
    {
      m_keywordBreak = false;
      ++m_pos; // Yuck
      CString string = RecurseTerm();
      AppendOutput(string);
      AppendOutput(",");
      AppendOutput(substr);
      GetWord(word);
      if(word == ")")
      {
        AppendOutput(word);
        return 1;
      }
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn repeat('klm',3)} FROM dual;
int
ODBCParser::FuncRepeat()
{
  // REPEAT(string,n) => RPAD(string,n*LENGTH(string),string)
  CString word("RPAD(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString string = RecurseTerm();
    GetWord(word);
    if(word == ",")
    {
      CString count = RecurseTerm();
      AppendOutput(string);
      AppendOutput(",");
      AppendOutput(count);
      AppendOutput("*LENGTH(");
      AppendOutput(string);
      AppendOutput("),");
      AppendOutput(string);
      GetWord(word);
      if(word == ")")
      {
        AppendOutput(word);
        return 1;
      }
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn REPLACE('this is a surprise','is','xxx')} FROM dual;
int 
ODBCParser::FuncReplace()
{
  // REPLACE -> REPLACE
  return 0;
}

// SELECT { fn right('abcdefg',4)} FROM dual;
int
ODBCParser::FuncRight()
{
  // RIGHT(string,count) => SUBSTR(string,-(count))
  CString word("SUBSTR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString string = RecurseTerm();
    GetWord(word);
    if(word == ",")
    {
      CString count = RecurseTerm();
      AppendOutput(string);
      AppendOutput(",-(");
      AppendOutput(count);
      AppendOutput(")");
      GetWord(word);
      if(word == ")")
      {
        AppendOutput(word);
        return 1;
      }
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn RTRIM('abcdefgklmklm','klm')} FROM dual;
int
ODBCParser::FuncRtrim()
{
  // RTRIM -> RTRIM
  return 0;
}

// SELECT { fn SOUNDEX('Huisman')} FROM dual;
int
ODBCParser::FuncSoundex()
{
  // SOUNDEX -> SOUNDEX
  return 0;
}

// SELECT { fn SPACE(15)}||'xx' FROM dual;
int
ODBCParser::FuncSpace()
{
  // SPACE(count) => LPAD(' ',count)
  CString word("LPAD(' ',");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString count = RecurseTerm();
    AppendOutput(count);
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn SUBSTRING('abcdefghijklm',4,3)} FROM dual;
int
ODBCParser::FuncSubstring()
{
  // SUBSTRING => SUBSTR
  CString word("SUBSTR");
  AppendOutput(word);
  return 1;
}

// SELECT { fn UCASE('abc')} FROM dual;
int
ODBCParser::FuncUcase()
{
  // UCASE -> UPPER
  CString word("UPPER");
  AppendOutput(word);
  return 1;
}

//////////////////////////////////////////////////////////////////////////
//
// SYSTEM FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

// SELECT { fn DATABASE()} FROM dual;
// SELECT { fn DATABASE ( )} FROM dual;
int  
ODBCParser::FuncDatabase()
{
  // DATABASE() => (sys_context('userenv','db_name'))
  CString word("(sys_context('userenv','db_name'))");
  AppendOutput(word);
  return DoubleEllipsisLeft();
}

// SELECT { fn IFNULL('abc','x')} FROM dual;
// SELECT { fn IFNULL(NULL,'x')}  FROM dual;
int  
ODBCParser::FuncIfnull()
{
  // IFNULL(exp,value) => NVL(exp,value)
  CString word("NVL");
  AppendOutput(word);
  return 1;
}

// SELECT {fn USER()} FROM dual;
// SELECT { fn USER ( ) } FROM dual;
int  
ODBCParser::FuncUser()
{
  // USER() => USER
  CString word("USER");
  AppendOutput(word);
  return DoubleEllipsisLeft();
}

//////////////////////////////////////////////////////////////////////////
//
// NUMERIC FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

// SELECT { fn ABS( 42)} from dual;
// SELECT { fn ABS(-42)} from dual;
int
ODBCParser::FuncAbs()
{
  // ABS -> ABS
  return 0;
}

// SELECT { fn ACOS(0.5)} FROM dual;
int  
ODBCParser::FuncAcos()
{
  // ACOS -> ACOS
  return 0;
}

// SELECT { fn ASIN(0.5)} FROM dual;
int  
ODBCParser::FuncAsin()
{
  // ASIN -> ASIN
  return 0;
}

// SELECT { fn ATAN(0.5)} FROM dual;
int  
ODBCParser::FuncAtan()
{
  // ATAN -> ATAN
  return 0;
}

// SELECT { fn ATAN2(0.5,1)} FROM dual;
int  
ODBCParser::FuncAtan2()
{
  // ATAN2 -> ATAN2
  return 0;
}

// SELECT {fn CEILING(3.14)} FROM dual;
int  
ODBCParser::FuncCeiling()
{
  // CEILING => CEIL
  CString word("CEIL");
  AppendOutput(word);
  return 1;
}

// SELECT { fn COS(1.5)} FROM dual;
int  
ODBCParser::FuncCos()
{
  // COS -> COS
  return 0;
}

// SELECT { fn COT(1.5)} FROM dual;
int  
ODBCParser::FuncCot()
{
  // COT(a) => (1/TAN(a))
  CString word("(1/TAN(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn DEGREES(3.1)} FROM dual;
int  
ODBCParser::FuncDegrees()
{
  // DEGREES(a) => (a*360/PI)
  CString word;
  GetWord(word);
  if(word == "(")
  {
    AppendOutput(word);
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput("*360/3.14159265358979323846264338327950288412");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn EXP(3)} FROM dual;
int  
ODBCParser::FuncExp()
{
  // EXP->EXP
  return 0;
}

// SELECT { fn FLOOR(3.14)} FROM dual;
int  
ODBCParser::FuncFloor()
{
  // FLOOR -> FLOOR
  return 0;
}

// SELECT { fn LOG(20)} FROM dual;
int  
ODBCParser::FuncLog()
{
  // LOG(num) => LOG(e,num)
  CString word("LOG(2.7182818284590452353602874713526624978,");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  return 0;
}

// SELECT { fn LOG10(100)} FROM dual;
int  
ODBCParser::FuncLog10()
{
  // LOG10(num) => LOG(10,num)
  CString word("LOG(10,");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  return 0;
}

// SELECT { fn MOD(9.2,4)} FROM dual;
int  
ODBCParser::FuncMod()
{
  // MOD -> MOD
  return 0;
}

// SELECT { fn PI()} FROM dual;
int  
ODBCParser::FuncPi()
{
  // PI in 38 decimals for a Oracle NUMBER(38,38)
  CString word("TO_NUMBER(3.14159265358979323846264338327950288412)");
  AppendOutput(word);
  return DoubleEllipsisLeft();
}

// SELECT { fn POWER(10,4)} FROM dual;
int  
ODBCParser::FuncPower()
{
  // POWER -> POWER
  return 0;
}

// SELECT { fn RADIANS(180)} FROM dual;
int  
ODBCParser::FuncRadians()
{
  // RADIANS(a) => (a/360*PI)
  CString word;
  GetWord(word);
  if(word == "(")
  {
    AppendOutput(word);
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput("/360*3.14159265358979323846264338327950288412");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0;
}

// SELECT { fn RAND()} FROM dual;
int  
ODBCParser::FuncRand()
{
  // It is not possible to support RAND random numbers
  SetError("HYC00");
  return 0;
}

// SELECT { fn ROUND(3.1456,2)} from dual;
int  
ODBCParser::FuncRound()
{
  // ROUND(a,b) => ROUND(a,b)
  return 0;
}

// SELECT { fn SIGN(-3)} FROM dual;
// SELECT { fn SIGN(+3)} FROM dual;
// SELECT { fn SIGN( 0)} FROM dual;
int  
ODBCParser::FuncSign()
{
  // SIGN(n)
  return 0;
}

// SELECT { fn SIN(1.1)} FROM dual;
int  
ODBCParser::FuncSin()
{
  // SIN -> SIN
  return 0;
}

// SELECT { fn SQRT(10000)} FROM dual;
int  
ODBCParser::FuncSqrt()
{
  // SQRT -> SQRT
  return 0;
}

// SELECT { fn TAN(1)} FROM dual;
int  
ODBCParser::FuncTan()
{
  // TAN -> TAN
  return 0;
}

// SELECT { fn TRUNCATE(3.1456,2)} FROM dual;
int
ODBCParser::FuncTruncate()
{
  // TRUNCATE -> TRUNC
  CString word("TRUNC");
  AppendOutput(word);
  return 1 ;
}

//////////////////////////////////////////////////////////////////////////
//
// DATETIME FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////


// SELECT { fn CURRENT_DATE()} from dual;
int  
ODBCParser::FuncCurrentDate() 
{ 
  // CURRENT_DATE => TO_DATE(TO_CHAR(SYSDATE,'YYYY-MM-DD'),'YYYY-MM-DD')
  CString word("TO_DATE(TO_CHAR(SYSDATE,'YYYY-MM-DD'),'YYYY-MM-DD')");
  AppendOutput(word);
  return DoubleEllipsisLeft();
}

// SELECT { fn CURRENT_TIME(3)} FROM dual;
// SELECT { fn CURRENT_TIME()} FROM dual;
int  
ODBCParser::FuncCurrentTime() 
{ 
  // CURRENT_TIME(prec) => TO_CHAR(CURRENT_TIMESTAMP(prec),'HH24:MI:SS.FF')
  CString word("TO_CHAR(CURRENT_TIMESTAMP(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString precision = RecurseTerm();
    AppendOutput(precision);
    AppendOutput("),'HH24:MI:SS");
    int prec = atoi(precision);
    if(prec > 0)
    {
      AppendOutput(".FF");
    }
    AppendOutput("'");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  else if(word == "()")
  {
    AppendOutput("0),'HH24:MI:SS')");
    return 1;
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn CURRENT_TIMESTAMP(3)} from dual;
// SELECT { fn CURRENT_TIMESTAMP()} from dual;
int  
ODBCParser::FuncCurrentTimestamp() 
{ 
  // CURRENT_TIMESTAMP -> CURRENT_TIMESTAMP
  CString word("CURRENT_TIMESTAMP(");
  AppendOutput(word);
  GetWord(word);
  if(word == "()")
  {
    AppendOutput("0)");
    return 1;
  }
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(")");
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn CURDATE()} FROM dual;
int  
ODBCParser::FuncCurdate() 
{ 
  // CURDATE => SYSDATE
  CString word("TO_DATE(TO_CHAR(SYSDATE,'YYYY-MM-DD'),'YYYY-MM-DD')");
  AppendOutput(word);
  return DoubleEllipsisLeft();
}

// SELECT { fn CURTIME()} from dual;
int  
ODBCParser::FuncCurtime() 
{ 
  // CURTIME => SYSDATE
  CString word("TO_CHAR(SYSDATE,'HH24:MI:SS')");
  AppendOutput(word);
  return DoubleEllipsisLeft();
}

// SELECT { fn DAYNAME({fn CURDATE()})} FROM dual;
int  
ODBCParser::FuncDayname() 
{ 
  // DAYNAME(date) => TRIM(TO_CHAR(date,'DAY'))
  CString word("TRIM(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'DAY')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT {fn DAYOFMONTH( {fn CURDATE()} )} FROM dual;
int  
ODBCParser::FuncDayofmonth() 
{ 
  // DAYOFMONTH(date) => TRIM(TO_CHAR(date,'DD'))
  CString word("TRIM(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'DD')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn DAYOFWEEK( {fn CURDATE()} )} FROM dual;
int  
ODBCParser::FuncDayofweek() 
{ 
  // DAYOFWEEK(date) -> TO_NUMBER(TO_CHAR(date,'D'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'D')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn DAYOFYEAR( {fn CURDATE()} )} FROM dual;
int  
ODBCParser::FuncDayofyear() 
{ 
  // DAYOFYEAR(date) -> TO_NUMBER(TO_CHAR(date,'DDD'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'DDD')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn EXTRACT( MONTH FROM {fn CURDATE()} )} FROM dual;
int  
ODBCParser::FuncExtract() 
{ 
  // EXTRACT -> EXTRACT
  return 0; 
}

// SELECT { fn HOUR( {fn CURRENT_TIMESTAMP(2)} )} FROM dual;
int  
ODBCParser::FuncHour() 
{ 
  // HOUR(timestamp) => TO_NUMBER(TO_CHAR(timestamp,'HH24'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'HH24')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn MINUTE( {fn CURRENT_TIMESTAMP(2)} )} FROM dual;
int  
ODBCParser::FuncMinute() 
{ 
  // MINUTE(timestamp) => TO_NUMBER(TO_CHAR(timestamp,'MI'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'MI')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn MONTH( {fn CURRENT_TIMESTAMP(2)} )} FROM dual;
int  
ODBCParser::FuncMonth() 
{ 
  // MONTH(timestamp) => TO_NUMBER(TO_CHAR(timestamp,'MM'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'MM')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn MONTHNAME( {fn CURRENT_TIMESTAMP(2)} )} FROM dual;
int  
ODBCParser::FuncMonthname() 
{ 
  // MONTHNAME(date) => TRIM(TO_CHAR(date,'MONTH'))
  CString word("TRIM(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'MONTH')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn NOW()} FROM dual;
int  
ODBCParser::FuncNow() 
{ 
  CString word("SYSDATE");
  AppendOutput(word);
  return DoubleEllipsisLeft();
}

// SELECT { fn QUARTER( {fn CURRENT_TIMESTAMP(2)} )} FROM dual;
int  
ODBCParser::FuncQuarter() 
{ 
  // QUARTER(date) -> TO_NUMBER(TO_CHAR(date,'Q'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'Q')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn SECOND( {fn CURRENT_TIMESTAMP(2)} )} FROM dual;
int  
ODBCParser::FuncSecond() 
{ 
  // SECOND(timestamp) => TO_NUMBER(TO_CHAR(timestamp,'SS'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'SS')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

int
ODBCParser::FuncTimestampadd()
{
  // TIMESTAMPADD(interval,exp1,exp2) is not supported
  // EXTRACT(DAY FROM (exp1 + exp2)) works only if exp2 is a ds_interval
  // and there is no {i'interval'} escape code in ODBC
  SetError("HYC00");
  return 0;
}

// SELECT {fn TIMESTAMPDIFF(SQL_TSI_HOUR, {fn CURDATE()} , TO_DATE('01-01-2008') )} FROM dual;
int  
ODBCParser::FuncTimestampdiff() 
{ 
  // TIMESTAMPDIF(interval,exp1,exp2) => (exp1 - exp2)
  // SQL_TSI_DAY    -> FLOOR(diff....)
  // SQL_TSI_HOUR   -> FLOOR(24*(diff....))
  // SQL_TSI_MINUTE -> FLOOR(1440*(diff....))  // 60*24
  // SQL_TSI_SECOND -> FLOOR(86400*(diff...))  // 60*60*24
  // SQL_TSI_WEEK   -> FLOOR((diff....)/7)
  // SQL_TSI_QUARTER-> FLOOR((diff....)/91)
  // SQL_TIS_YEAR   -> FLOOR((diff....)/365.25)
  CString diff("((");
  CString front;
  CString rear;
  CString word;
  GetWord(word);
  if(word == "(")
  {
    CString interval = RecurseTerm();
    interval.Trim();
    interval.MakeLower();
    interval.TrimLeft("sql_tsi_");
         if(interval == "day"    ) { front = "FLOOR(";        rear = ")";         }
    else if(interval == "hour"   ) { front = "FLOOR(24*(";    rear = "))";        }
    else if(interval == "minute" ) { front = "FLOOR(1440*(";  rear = "))";        }
    else if(interval == "second" ) { front = "FLOOR(86400*("; rear = "))";        }
    else if(interval == "week"   ) { front = "FLOOR((";       rear = ")/7)";      }
    else if(interval == "quarter") { front = "FLOOR((";       rear = ")/91)";     }
    else if(interval == "year"   ) { front = "FLOOR((";       rear = ")/365.25)"; }
    else 
    {
      // frac_second / month generate a "optional feature not implemented"
      SetError("HYC00");
      return 0;
    }
    GetWord(word);
    if(word==",")
    {
      CString expression1 = RecurseTerm();
      diff += expression1;
      GetWord(word);
      if(word == ",")
      {
        CString expression2 = RecurseTerm();
        diff += ")-(";
        diff += expression2;
        diff += "))";
        GetWord(word);
        if(word == ")")
        {
          diff = front + diff + rear;
          AppendOutput(diff);
          return 1;
        }
      }
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn WEEK( {fn CURRENT_TIMESTAMP(2)} )} FROM dual;
int  
ODBCParser::FuncWeek() 
{ 
  // WEEK(date) => TO_NUMBER(TO_CHAR(date,'IW'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'IW')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

// SELECT { fn YEAR( {fn CURRENT_TIMESTAMP(2)} )} FROM dual;
int  
ODBCParser::FuncYear() 
{
  // YEAR(timestamp) => TO_NUMBER(TO_CHAR(timestamp,'IYYY'))
  CString word("TO_NUMBER(TO_CHAR(");
  AppendOutput(word);
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    AppendOutput(term);
    AppendOutput(",'IYYY')");
    GetWord(word);
    if(word == ")")
    {
      AppendOutput(word);
      return 1;
    }
  }
  SetError("42000");
  return 0; 
}

//////////////////////////////////////////////////////////////////////////
//
// Generic CAST / CONVERT
//
//////////////////////////////////////////////////////////////////////////

int
ODBCParser::FuncCast()
{
  // CAST -> CAST
  return 0;
}

// SELECT { fn CONVERT(1234,SQL_VARCHAR)} FROM dual;
int
ODBCParser::FuncConvert()
{
  // CONVERT( something, SQL_TYPE) => 
  CString word;
  GetWord(word);
  if(word == "(")
  {
    CString term = RecurseTerm();
    GetWord(word);
    if(word == ",")
    {
      GetWord(word);
      word.Trim();
      word.MakeUpper();
      if(word.Left(4) == "SQL_")
      {
        word = word.Mid(4);
      }
      if(word == "INT"  || word == "INTEGER" || word == "FLOAT"   || 
         word == "REAL" || word == "DOUBLE"  || word == "TINYINT" ||
         word == "BIT"  || word == "BIGINT"  || word == "DECIMAL" ||
         word == "NUMERIC" || 
         word == "SMALLINT" )
      {
         AppendOutput("TO_NUMBER(");
         AppendOutput(term);
      }
      else if(word == "CHAR"  || word == "VARCHAR"  || word == "LONGVARCHAR" || 
              word == "WCHAR" || word == "WVARCHAR" || word == "WLONGVARCHAR")
      {
        AppendOutput("TO_CHAR(");
        AppendOutput(term);
      }
      else if(word == "DATE" || word == "TYPE_DATE")
      {
        AppendOutput("TO_DATE(");
        AppendOutput(term);
        AppendOutput(",'YYYY-MM-DD'");
      }
      else if(word == "TIME" || word == "TYPE_TIME")
      {
        AppendOutput("TO_DATE(");
        AppendOutput(term);
        AppendOutput(",'HH24:MI:SS'");
      }
      else if(word == "TIMESTAMP" || word == "TYPE_TIMESTAMP")
      {
        AppendOutput("TO_DATE(");
        AppendOutput(term);
        AppendOutput(",'YYYY-MM-DD HH24:MI:SS'");
      }
      else if(word == "BINARY" || word == "VARBINARY" || word == "LONGVARBINARY")
      {
        AppendOutput("HEXTORAW(");
        AppendOutput(term);
      }
      else if(word == "INTERVAL_YEAR_TO_MONTH")
      {
        AppendOutput("TO_YMINTERVAL(");
        AppendOutput(term);
      }
      else if(word == "INTERVAL_DAY_TO_SECOND")
      {
        AppendOutput("TO_DSINTERVAL(");
        AppendOutput(term);
      }
      else
      {
        // Optional feature not implemented
        SetError("HYC00");
        return 0;
      }
      GetWord(word);
      if(word == ")")
      {
        AppendOutput(word);
        return 1;
      }
    }
  }
  SetError("42000");
  return 0;
}
