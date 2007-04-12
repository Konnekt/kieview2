#include "stdafx.h"
#include "TextTemplateParser.h"

void TextTemplateParser::parse(iBlockToken* token) {
  wstring::iterator itCurrPos = _data.begin();
  wstring::iterator itEnd = _data.end();

  while (itCurrPos != itEnd) {
    itCurrPos++;
  }
}

void TextTemplateParser::parse(oTemplate& tpl) {
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