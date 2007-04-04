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
  };

public:
  void parse(oTemplate& tpl);
  enParseRes parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);

  enParseParamRes parseParam(TemplateParam* param, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  TemplateParam::enOperators parseOperator(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  void parseText(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  void parseConst(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  bool parseArgument(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  void parseVar(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  void parseInt(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);

  iTemplateToken* getToken(int type, oTemplate& tpl);
  int getType(string& text);
};

#endif // __TEMPLATE_PARSER_H__