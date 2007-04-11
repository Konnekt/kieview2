#pragma once

#ifndef __TEXT_TEMPLATE_PARSER_H__
#define __TEXT_TEMPLATE_PARSER_H__

#include "iTemplateParser.h"

class TextTemplateParser : public iTemplateParser {
public:
  void parse(iBlockToken* token);
  void parseTemplate(oTemplate& tpl);
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

private:
  wstring _data;
};

void TextTemplateParser::parse(iBlockToken* token) {
  wstring::iterator itCurrPos = _data.begin();
  wstring::iterator itEnd = _data.end();

  while (itCurrPos != itEnd) {
    itCurrPos++;
  }

}

void TextTemplateParser::parseTemplate(oTemplate& tpl) {
  _data = tpl->getData().w_str();
  parse(tpl->getBlock());
}

void TextTemplateParser::parseToken(iBlockToken* parent, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos) {
}

void TextTemplateParser::parseBlock(iBlockToken* parent, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos) {
}

void TextTemplateParser::parseSection(iSectionToken* token, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos) {
}

void TextTemplateParser::parseText(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos)
{
}

void TextTemplateParser::parseRegExp(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos)
{
}

void TextTemplateParser::parseBool(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos)
{
}

void TextTemplateParser::parseArgument(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos)
{
}

void TextTemplateParser::parseOperator(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos)
{
}

void TextTemplateParser::parseVar(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos)
{
}

void TextTemplateParser::parseInt(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos)
{
}

void TextTemplateParser::parseBracket(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos)
{
}

#endif //__TEMPLATE_TEXT_PARSER_H__