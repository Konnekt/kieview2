  /**
  *  kIEview2 RTF to HTML parser
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 12 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-11-21 10:39:27 +0100 (Wt, 21 lis 2006) $
  */

#include "stdafx.h"
#include "RtfHtml.h"

RtfHtmlSpan::RtfHtmlSpan() {
}

RtfHtmlSpan::~RtfHtmlSpan() {
}

char* RtfHtmlSpan::skipGroup(char *str) {
  int cnt = 1, length;
  char curchar, nextchar;

  while (cnt) {
    curchar = *str;
    if (curchar == '{') {
      cnt++;
      str++;
    } else if (curchar == '}') {
      cnt--;
      str++;
    } else if (curchar == '\\') {
      nextchar = str[1];
      if (nextchar == '{' || nextchar == '}') {
        str += 2;
      } else if (!memcmp(str, "bin", 3) && isdigit(str[4])) {
        str += 4;
        length = strtol(str, NULL, 10);
        str += length;
      } else {
        str++;
      }
    } else {
      str++;
    }
  }
  return str;
}

char * RtfHtmlSpan::colorTable(char * str) {
  int cnt = 1, pos;
  char curchar;
  BYTE red = 0, green = 0, blue = 0;
  char keyword[100], param_str[100];
  int param;

  if (!m_aColors.empty()) {
    m_aColors.erase(m_aColors.begin(), m_aColors.end());
  }

  while (cnt) {
    curchar = *str;
    if (curchar == '{') {
      cnt++;
      str++;
    } else if (curchar == '}') {
      cnt--;
      str++;
    } else if (curchar == ';') {
      m_aColors.push_back(RGB(red, green, blue));
      red = green = blue = 0;
      str++;
    } else if (curchar == '\\') {
      for (pos = 1; isalpha(str[pos]); pos++);
      memcpy(keyword, str+1, pos-1);
      keyword[pos-1] = 0;
      str += pos;

      for (pos = 0; isdigit(str[pos]) || str[pos] == '-'; pos++);
      if (pos) {
        memcpy(param_str, str, pos);
        param_str[pos] = 0;
        param = atoi(param_str);
        str += pos;
      } else {
        param = -1;
      }
      if (*str == ' ') {
        str++;
      }

      if (!strcmp(keyword, "red")) {
        red = param;
      } else if (!strcmp(keyword, "green")) {
        green = param;
      } else if (!strcmp(keyword, "blue")) {
        blue = param;
      }
    } else {
      str++;
    }
  }
  return str;
}

string RtfHtmlSpan::rtfParse(char *crtf, int rtflen) {
  char *rtf = crtf;
  string html("");
  int pos = 0;
  bool bAsterisk = false;
  string stack(""), prevstack("");
  UINT color = 0;
  int idx;
  char keyword[100], param_str[100];
  int param;
  char curchar, nextchar;

  while (*rtf) {
    curchar = *rtf;
    if (curchar == '\\') {
      nextchar = rtf[1];
      if (nextchar == '\\' || nextchar == '{' || nextchar == '}') {
        html += nextchar;
        rtf += 2;
      } else if (isalpha(nextchar)) {
        for (pos = 1; isalpha(rtf[pos]); pos++);
        memcpy(keyword, rtf+1, pos-1);
        keyword[pos-1] = 0;
        rtf += pos;

        for (pos = 0; isdigit(rtf[pos]) || rtf[pos] == '-'; pos++);
        if (pos) {
          memcpy(param_str, rtf, pos);
          param_str[pos] = 0;
          param = atoi(param_str);
          rtf += pos;
        } else {
          param =- 1;
        }
        if (*rtf == ' ') {
          rtf++;
        }

        // wykonujemy polecenia z keyword i param
        if (bAsterisk) {
          bAsterisk = false;
          rtf = skipGroup(rtf-1);
        } else {
          if (!strcmp(keyword, "colortbl")) {
            rtf = colorTable(rtf);
          } else if (!strcmp(keyword, "filetbl") || !strcmp(keyword, "stylesheet") || !strcmp(keyword, "info") || !strcmp(keyword, "fonttbl")) {
            // te grupy nas nie interesuja
            rtf = skipGroup(rtf);
          } else if (!strcmp(keyword, "line") || !strcmp(keyword, "softline")) {
            html += "<br />";
          } else if (!strcmp(keyword, "tab")) {
            html += "&nbsp;&nbsp;";
          } else if (!strcmp(keyword, "pard")) {
            // skip
          } else if (!strcmp(keyword, "par")) {
            html += "<br />";
          } else if (!strcmp(keyword, "b")) {
            if (param != 0) {
              if (stack.find('b') == stack.npos) {
                stack += "b";
              }
            } else {
              if ((idx = stack.find_last_of('b')) != stack.npos) {
                stack.erase(idx, 1);
              }
            }
          } else if (!strcmp(keyword, "i")) {
            if (param != 0) {
              if (stack.find('i') == stack.npos) {
                stack += "i";
              }
            } else {
              if ((idx = stack.find_last_of('i')) != stack.npos) {
                stack.erase(idx, 1);
              }
            }
          } else if (!strcmp(keyword, "ul")) {
            if (param != 0) {
              if (stack.find('u') == stack.npos) {
                stack += "u";
              }
            } else {
              if ((idx = stack.find_last_of('u')) != stack.npos) {
                stack.erase(idx, 1);
              }
            }
          } else if (!strcmp(keyword, "ulnone")) {
            if ((idx = stack.find_last_of('u')) != stack.npos) {
              stack.erase(idx, 1);
            }
          } else if (!strcmp(keyword, "cf")) {
            if (param != color) {
              if ((idx = stack.find_last_of('c')) != stack.npos) {
                stack.erase(idx, 1);
              }
              color = param;
              if (color != 0) {
                stack += "c";
              }
            }
          } else if (!strcmp(keyword, "plain")) {
            if ((idx = stack.find_last_of('b')) != stack.npos) {
              stack.erase(idx, 1);
            }
            if ((idx = stack.find_last_of('i')) != stack.npos) {
              stack.erase(idx, 1);
            }
            if ((idx = stack.find_last_of('u')) != stack.npos) {
              stack.erase(idx, 1);
            }
            if ((idx = stack.find_last_of('c')) != stack.npos) {
              stack.erase(idx, 1);
            }
          } else if (!strcmp(keyword, "bullet")) {
            html += "&bull;";
          }
        }
      } else {
        // wykonujemy akcje symbolu kontrolnego z nextchar
        if (nextchar == '\'') {
          rtf += 2;
          char stmp[8];
          stmp[0] = rtf[0];
          stmp[1] = rtf[1];
          stmp[2] = 0;

          int code = strtol(stmp, NULL, 16);
          switch (code) {
            case 167:
              html += "&bull;";
              break;
            default:
              html += code;
          }
        } else if(nextchar == '*') {
          bAsterisk = true;
        }
        rtf += 2;
      }
    } else if (curchar == '{') {
      // otwieranie grupy
      rtf++;
    } else if (curchar == '}') {
      // zamykanie grupy
      rtf++;
    } else if (curchar == 13 || curchar == 10) {
      rtf++;
    } else {
      if (prevstack != stack) {
        if (prevstack.length() > 0) {
          html += "</span>";
        }
        if (stack.length() > 0) {
          html += "<span style=\"";
          for (UINT j = 0; j < stack.length(); j++) {
            char ch = stack[j];
            switch (ch) {
              case 'b': html += "font-weight: bold;"; break;
              case 'i': html += "font-style: italic;"; break;
              case 'u': html += "text-decoration: underline;"; break;
              case 'c': {
                char buf[32];
                COLORREF rgb = color < m_aColors.size() ? m_aColors[color] : RGB(0,0,0);
                sprintf(buf, "%.2x%.2x%.2x", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
                html += "color: #"; html += buf; html += ";";
                break;
              }
            }
          }
          html += "\">";
        }
      }
      prevstack = stack;
      html += curchar;
      rtf++;
    }
  }
  return html;
}

RtfHtmlTag::RtfHtmlTag() {
}

RtfHtmlTag::~RtfHtmlTag() {
}

char * RtfHtmlTag::skipGroup(char *str) {
  int cnt = 1, length;
  char curchar, nextchar;

  while (cnt) {
    curchar = *str;
    if (curchar == '{') {
      cnt++;
      str++;
    } else if (curchar == '}') {
      cnt--;
      str++;
    } else if (curchar == '\\') {
      nextchar = str[1];

      if (nextchar == '{' || nextchar == '}') {
        str += 2;
      } else if (!memcmp(str, "bin", 3) && isdigit(str[4])) {
        str += 4;
        length = strtol(str, NULL, 10);
        str += length;
      } else {
        str++;
      }
    } else {
      str++;
    }
  }
  return str;
}

char * RtfHtmlTag::colorTable(char * str) {
  int cnt = 1, pos;
  char curchar;
  BYTE red = 0, green = 0, blue = 0;
  char keyword[100], param_str[100];
  int param;

  if (!m_aColors.empty()) {
    m_aColors.erase(m_aColors.begin(), m_aColors.end());
  }

  while (cnt) {
    curchar = *str;
    if (curchar == '{') {
      cnt++;
      str++;
    } else if (curchar == '}') {
      cnt--;
      str++;
    } else if (curchar == ';') {
      m_aColors.push_back(RGB(red, green, blue));
      red = green = blue = 0;
      str++;
    } else if (curchar == '\\') {
      for (pos = 1; isalpha(str[pos]); pos++);
      memcpy(keyword, str+1, pos-1);
      keyword[pos-1] = 0;
      str += pos;

      for (pos = 0; isdigit(str[pos]) || str[pos] == '-'; pos++);
      if (pos) {
        memcpy(param_str, str, pos);
        param_str[pos] = 0;
        param = atoi(param_str);
        str += pos;
      } else {
        param = -1;
      }
      if (*str == ' ') {
        str++;
      }

      if (!strcmp(keyword, "red")) {
        red = param;
      } else if (!strcmp(keyword, "green")) {
        green = param;
      } else if (!strcmp(keyword, "blue")) {
        blue = param;
      }
    } else {
      str++;
    }
  }
  return str;
}

// schodzi po stosie znacznikow do idx usuwa go i wchodzi na gore
string RtfHtmlTag::convStepDown(string &stack, int idx, UINT color) {
  string msg;

  for (int pos = (int)stack.size()-1; pos >= idx; pos--) {
    char ch = stack.at(pos);
    if (ch == 'b') msg += "</b>";
    else if (ch == 'i') msg += "</i>";
    else if (ch == 'u') msg += "</u>";
    else if (ch == 'c') msg += "</span>";
  }

  stack.erase(idx, 1);
  for (int pos = idx; pos < (int)stack.size(); pos++) {
    char ch = stack.at(pos);
    if (ch == 'b') msg += "<b>";
    else if (ch == 'i') msg += "<i>";
    else if (ch == 'u') msg += "<u>";
    else if (ch == 'c') {
      char buf[32];
      COLORREF rgb = color < m_aColors.size() ? m_aColors[color] : RGB(0,0,0);
      sprintf(buf, "%.2x%.2x%.2x", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
      msg += "<span style=\"color: #"; msg += buf; msg += "\">";
    }
  }
  return msg;
}

string RtfHtmlTag::rtfParse(char *crtf, int rtflen) {
  char *rtf = crtf;
  string html;
  int pos = 0;
  bool bAsterisk = false;
  string stack;
  UINT color = 0;
  int idx;
  char keyword[100], param_str[100];
  int param;
  char curchar, nextchar;

  while (*rtf) {
    curchar = *rtf;
    if (curchar == '\\') {
      nextchar = rtf[1];
      if (nextchar == '\\' || nextchar == '{' || nextchar == '}') {
        html += nextchar;
        rtf += 2;
      } else if (isalpha(nextchar)) {
        for (pos = 1; isalpha(rtf[pos]); pos++);
        memcpy(keyword, rtf+1, pos-1);
        keyword[pos-1] = 0;
        rtf += pos;

        for (pos = 0; isdigit(rtf[pos]) || rtf[pos] == '-'; pos++);
        if (pos)  {
          memcpy(param_str, rtf, pos);
          param_str[pos] = 0;
          param = atoi(param_str);
          rtf += pos;
        } else {
          param =- 1;
        }
        if (*rtf == ' ') {
          rtf++;
        }

        // wykonujemy polecenia z keyword i param
        if (bAsterisk) {
          bAsterisk = false;
          rtf = skipGroup(rtf-1);
        } else {
          if (!strcmp(keyword, "colortbl")) {
            rtf = colorTable(rtf);
          } else if (!strcmp(keyword, "filetbl") || !strcmp(keyword, "stylesheet") || !strcmp(keyword, "info") || !strcmp(keyword, "fonttbl")) {
            // te grupy nas nie interesuja
            rtf = skipGroup(rtf);
          } else if (!strcmp(keyword, "line") || !strcmp(keyword, "softline")) {
            html += "<br />";
          } else if (!strcmp(keyword, "tab")) {
            html += "&nbsp;&nbsp;";
          } else if (!strcmp(keyword, "pard")) {
            // skip
          } else if (!strcmp(keyword, "par")) {
            html += "<br />";
          } else if (!strcmp(keyword, "b")) {
            if (param != 0) {
              if (stack.find('b') == stack.npos) {
                html += "<b>";
                stack += "b";
              }
            } else {
              if ((idx = stack.find_last_of('b')) != stack.npos) {
                html += convStepDown(stack, (UINT)idx, color);
              }
            }
          } else if (!strcmp(keyword, "i")) {
            if (param != 0) {
              if (stack.find('i') == stack.npos) {
                html += "<i>";
                stack += "i";
              }
            } else {
              if ((idx = stack.find_last_of('i')) != stack.npos) {
                html += convStepDown(stack, (UINT)idx, color);
              }
            }
          } else if (!strcmp(keyword, "ul")) {
            if (param != 0) {
              if (stack.find('u') == stack.npos) {
                html += "<u>";
                stack += "u";
              }
            } else {
              if ((idx = stack.find_last_of('u')) != stack.npos) {
                html += convStepDown(stack, (UINT)idx, color);
              }
            }
          } else if (!strcmp(keyword, "ulnone")) {
            if ((idx = stack.find_last_of('u')) != stack.npos) {
              html += convStepDown(stack, (UINT)idx, color);
            }
          } else if (!strcmp(keyword, "cf")) {
            if (param != color) {
              if ((idx = stack.find_last_of('c')) != stack.npos) {
                html += convStepDown(stack, (UINT)idx, color);
              }
              color = param;
              if (color != 0) {
                stack += "c";
                char buf[32];
                COLORREF rgb = color < m_aColors.size() ? m_aColors[color] : RGB(0,0,0);
                sprintf(buf, "%.2x%.2x%.2x", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
                html += "<span style=\"color: #"; html += buf; html += "\">";
              }
            }
          } else if (!strcmp(keyword, "plain")) {
            if ((idx = stack.find_last_of('b')) != stack.npos)
              html += convStepDown(stack, (UINT)idx, color);
            if ((idx = stack.find_last_of('i')) != stack.npos)
              html += convStepDown(stack, (UINT)idx, color);
            if ((idx = stack.find_last_of('u')) != stack.npos)
              html += convStepDown(stack, (UINT)idx, color);
            if ((idx = stack.find_last_of('c')) != stack.npos)
              html += convStepDown(stack, (UINT)idx, color);
          } else if (!strcmp(keyword, "bullet")) {
            html += "&bull;";
          }
        }
      } else {
        // wykonujemy akcje symbolu kontrolnego z nextchar
        if (nextchar == '\'') {
          rtf += 2;
          char stmp[8];
          stmp[0] = rtf[0];
          stmp[1] = rtf[1];
          stmp[2] = 0;

          int code = strtol(stmp, NULL, 16);
          switch (code) {
            case 167:
              html += "&bull;";
              break;
            default:
              html += code;
          }
        } else if(nextchar == '*') {
          bAsterisk = true;
        }
        rtf += 2;
      }
    } else if (curchar == '{') {
      // otwieranie grupy
      rtf++;
    } else if (curchar == '}') {
      // zamykanie grupy
      rtf++;
    } else if (curchar == 13 || curchar == 10) {
      rtf++;
    } else {
      html += curchar;
      rtf++;
    }
  }

  // koncowe zamykanie znacznikow html 
  for (pos = stack.size()-1; pos >= 0; pos--) {
    char ch = stack.at(pos);
    if (ch == 'b') html += "</b>";
    else if (ch == 'i') html += "</i>";
    else if (ch == 'u') html += "</u>";
    else if (ch == 'c') html += "</span>";
  }

  // usuwamy znaczniki html ktore zaczynaja sie i odrazu koncza
  bool puste = true;
  while (puste) {
    puste = false;
    pos = 0;
    while ((pos = html.find("<b></b>", pos)) != html.npos) {
      html.erase(pos, 7);
      puste = true;
    }
    pos = 0;
    while ((pos = html.find("<i></i>", pos)) != html.npos) {
      html.erase(pos, 7);
      puste = true;
    }
    pos = 0;
    while ((pos = html.find("<u></u>", pos)) != html.npos) {
      html.erase(pos, 7);
      puste = true;
    }
    pos = 0;
    int pos2 = 0;
    while ((pos = html.find("<span", pos2)) != html.npos) {
      if ((pos2 = html.find('>', pos)) != html.npos) {
        if (html.substr(pos2 + 1, 7) == "</span>") {
          html.erase(pos, pos2 + 8 - pos);
          puste = true;
        }
      }
    }
  }
  return html;
} 