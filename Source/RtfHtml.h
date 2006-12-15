/**
  *  kIEview2 RTF to HTML parser
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#pragma once

#include <ctype.h>
#include <stack>

class RtfHtml {
public: 
  RtfHtml() { };
  virtual ~RtfHtml() { };

  virtual string rtfParse(char* crtf, int rtflen) = 0;
};

class RtfHtmlSpan : public RtfHtml {
public:
  RtfHtmlSpan();
  ~RtfHtmlSpan();

  string rtfParse(char* crtf, int rtflen);

protected:
  char* skipGroup(char* str);
  char* colorTable(char* str);

  deque<COLORREF> m_aColors;
};

class RtfHtmlTag : public RtfHtml {
public:
  RtfHtmlTag();
  ~RtfHtmlTag();

  string rtfParse(char* crtf, int rtflen);

protected:
  char* skipGroup(char* str);
  char* colorTable(char* str);
  string convStepDown(string &stack, int idx, UINT color);

  deque<COLORREF> m_aColors;
};