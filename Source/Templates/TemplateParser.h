#pragma once

#ifndef __TEMPLATE_PARSER_H__
#define __TEMPLATE_PARSER_H__

#include <Stamina/Object.h>
#include <Stamina/ObjectImpl.h>
#include <Stamina/Exception.h>
#include <Stamina/CriticalSection.h>
#include <Stamina/String.h>
#include <Stamina/RegEx.h>

class iTemplateToken;
class iBlockToken;
class TemplateParam;

typedef SharedPtr<class iTemplate> oTemplate;

enum enOperators {
  opNone,
  opPlus,
  opMinus,
  opNot,
  opComp,
  opDiff,
  opRegExDiff,
  opRegExComp,
  opAnd,
  opOr
};

class TemplateParser {
public:
  enum enParseRes {
    tplEndTokenFound = 1, // end token found {/abc}
    tplBadSyntax = 2, // syntax error
    tplStopTokenFound = 3, // stop token found
    tplParseOK = 4 // done parsing
  };

  enum enParseParamRes {
    paramParseOK = 1,
    paramError = 2
  };

public:
  void parse(oTemplate& tpl);
  enParseRes parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);

  enParseParamRes parseParam(TemplateParam* param, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  enOperators parseOperator(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  void parseText(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  void parseConst(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  bool parseArgument(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  void parseVar(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);
  void parseInt(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);

  iTemplateToken* getToken(int type, iBlockToken* token);
  int getType(string& text);
};

#endif // __TEMPLATE_PARSER_H__
