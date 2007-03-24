#pragma once

#ifndef __TEMPLATE_PARSER_H__
#define __TEMPLATE_PARSER_H__

#include "TemplateToken.h"
#include "TemplateValue.h"

class TemplateParser {
public:
  enum enParseRes {
    tplEndTokenFound = 1, //end token found {/abc}
    tplBadSyntax = 2, //error in syntax
    tplStopTokenFound = 3, //stop token found
    tplParseOK = 4, //done of parsing
  };

  enum enParseParamRes {
    paramParseOK = 1,
    paramError = 2,
    paramBadSyntax = 3,
    paramInvalidArg = 4, // unknown param
  };


public:
  static enParseRes parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  static enParseParamRes parseParam(TemplateParam* param, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);

  static iTemplateToken* getToken(int type);
  static int getType(string& text);
};


#endif // __TEMPLATE_PARSER_H__