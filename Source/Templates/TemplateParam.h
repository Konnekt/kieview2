#pragma once

#ifndef __TEMPLATE_PARAM_H__
#define __TEMPLATE_PARAM_H__

#include "TemplateParser.h"
#include "iBlockToken.h"

class TemplateParam {
public:
  struct sArgument {
    TemplateValue value;
    enOperators nextOperator;
    bool not;

    sArgument(TemplateValue& value, enOperators nextOperator, bool not): 
      value(value), nextOperator(nextOperator), not(not) { }
  };

  typedef vector<sArgument*> tArguments;

public:
  TemplateParam(TemplateParser* parser, iBlockToken* parent): _parser(parser), _block(parent) { }
  TemplateParam(const TemplateParam& param);
  ~TemplateParam();

public:
  void add(TemplateValue value, enOperators nextOperator, bool not);
  void clear();
  UINT count();

  TemplateParser* getParser() {
    return _parser;
  }
  iBlockToken* getBlock() {
    return _block;
  }
  TemplateValue output();

protected:
  tArguments _arguments;
  TemplateParser* _parser;
  iBlockToken* _block;
};

class TemplateHash {
public:
  typedef map<string, TemplateValue> tValues;

public:
  TemplateValue operator [] (const string& key) {
    return get(key);
  }

public:
  TemplateHash(TemplateValue defValue) {
    set("default", defValue);
  }
  TemplateHash() { }

public:
  void set(const string& name, TemplateValue value);
  TemplateValue get(const string& name);

protected:
  tValues _values;
};

#endif // __TEMPLATE_PARAM_H__