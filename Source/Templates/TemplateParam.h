#pragma once

#ifndef __TEMPLATE_PARAM_H__
#define __TEMPLATE_PARAM_H__

#include "TemplateParser.h"
#include "iBlockToken.h"

class TemplateParam {
public:
  struct sArgument {
    bool not;
    TemplateValue value;
    enOperators nextOperator;

    sArgument(TemplateValue& value, enOperators nextOperator, bool not): value(value), nextOperator(nextOperator), not(not) { }
  };

  typedef vector<sArgument*> tArguments;

public:
  TemplateParam(TemplateParser* parser, iBlockToken* parent);
  TemplateParam(const TemplateParam& param);
  ~TemplateParam();

public:
  void add(TemplateValue value, enOperators nextOperator, bool not);
  void clear();
  UINT count();
  TemplateValue output();

  TemplateParser* getParser() {
    return _parser;
  }
  iBlockToken* getBlock() {
    return _block;
  }

protected:
  tArguments _arguments;
  TemplateParser* _parser;
  iBlockToken* _block;
};

#endif // __TEMPLATE_PARAM_H__