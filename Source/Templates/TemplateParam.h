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

class TemplateHash {
public:
  typedef map<string, TemplateValue> tKeys;

public:
  TemplateHash();

  void set(const string& name, TemplateValue value);
  TemplateValue get(const string& name = "default");

private:
  tKeys _keys;
};

#endif // __TEMPLATE_PARAM_H__