#pragma once

#ifndef __TEMPLATE_PARSER_H__
#define __TEMPLATE_PARSER_H__

#include "TemplateToken.h"

class TemplateParser {
public:
  enum enParseRes {
  PF_ETF = 1, //end token found {/abc}
  PF_ERR = 2, //error in syntax
  PF_STF = 3, //stop token found
  PF_OK = 4, //done of parsing
};

public:
  static enParseRes parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  static void parseParam();

  static iTemplateToken* getToken(int type);
  static int getType(string& text);
};

#endif // __TEMPLATE_PARSER_H__