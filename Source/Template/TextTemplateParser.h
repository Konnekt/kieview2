#pragma once

#ifndef __TEXT_TEMPLATE_PARSER_H__
#define __TEXT_TEMPLATE_PARSER_H__

#include "iTemplateParser.h"

class TextTemplateParser : public iTemplateParser {
public:
  void parse(iBlockToken* token);
  void parse(oTemplate& tpl);

protected:
  void parseToken(iBlockToken* parent, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos);
  void parseBlock(iBlockToken* parent, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos);
  void parseSection(iSectionToken* token, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos);

  void parseText(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos);
  void parseRegExp(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos);
  void parseBool(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos);
  void parseArgument(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos);
  void parseOperator(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos);
  void parseVar(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos);
  void parseInt(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos);
  void parseBracket(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos);

protected:
  wstring _data;
};

#endif //__TEMPLATE_TEXT_PARSER_H__