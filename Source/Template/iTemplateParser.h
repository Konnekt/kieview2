#pragma once

#ifndef __ITEMPLATE_PARSER_H__
#define __ITEMPLATE_PARSER_H__

class TemplateParam;
class iBlockToken;

typedef SharedPtr<class iTemplate> oTemplate;

class iTemplateParser : public iObject {
public:
  void parse(const String& data) = 0;
  void parseTemplate(oTemplate& tpl) = 0;
  void parseToken(iBlockToken* parent, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos) = 0;
  void parseBlock(iBlockToken* parent, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos) = 0;
  void parseSection(iSectionToken* token, wstring::iterator itCurrPos, wstring::iterator itEnd, wstring::iterator& itPos) = 0;

  void parseText(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos) = 0; // "text" or 'text'
  void parseRegExp(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos) = 0; // /expr/flags
  void parseBool(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos) = 0; // true false
  void parseArgument(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos) = 0; // (123 + "abC") =~ /abc/i
  void parseOperator(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos) = 0; // + - ! AND ||
  void parseVar(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos) = 0; // $func() $array[index] $var $abc[1].chain
  void parseInt(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos) = 0; // -123 234
  void parseBracket(TemplateParam* param, enOperators operator_, bool not, wstring::iterator itCurrPos,
    wstring::iterator itEnd, wstring::iterator& itPos) = 0; // ( )
};

#endif //__ITEMPLATE_PARSER_H__