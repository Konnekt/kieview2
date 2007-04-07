#pragma once

#ifndef __TEMPLATE_VALUE_H__
#define __TEMPLATE_VALUE_H__

#include <string>
#include <Stamina/Time64.h>
#include <Stamina/Helpers.h>

using namespace std;
using namespace Stamina;

class TemplateVariable;
class TemplateFunction;
class TemplateParam;

class TemplateValue {
public:
  enum enTypes {
    tString = 1,
    tInteger = 2,
    tInteger64 = 3,
    tBoolean = 4,
    tDate64 = 5,
    tVar = 6,
    tFunction = 7,
    tParam = 8,
  };

  union {
    string* vString;
    int vInt;
    __int64 vInt64;
    bool vBool;
    Date64* vDate64;
    TemplateVariable* vVar;
    TemplateFunction* vFunction;
    TemplateParam* vParam;
  };

public:
  enTypes getType();
  void clear();
  void copy(TemplateValue& value);

  TemplateValue operator + (TemplateValue& value) {
    return plus(value);
  }
  TemplateValue operator - (TemplateValue& value) {
    return minus(value);
  }
  TemplateValue operator ! () {
    return not();
  }
  TemplateValue operator != (TemplateValue& value) {
    return diff(value);
  }
  TemplateValue operator == (TemplateValue& value) {
    return comp(value);
  }
  TemplateValue operator && (TemplateValue& value) {
    return and(value);
  }
  TemplateValue operator || (TemplateValue& value) {
    return or(value);
  }

public:
  TemplateValue();
  TemplateValue(const char* value);
  TemplateValue(const string& value);
  TemplateValue(int value);
  TemplateValue(__int64 value);
  TemplateValue(const Date64& value);
  TemplateValue(bool value);
  TemplateValue(TemplateVariable* value);
  TemplateValue(TemplateFunction* value);
  TemplateValue(TemplateParam* value);
  TemplateValue(const TemplateValue& value);
  const TemplateValue& operator = (const TemplateValue& copy);
  TemplateValue& operator = (TemplateValue& copy);

  ~TemplateValue();

public:
  string getString();
  bool getBool();
  int getInt();
  __int64 getInt64();
  Date64 getDate();

public:
  TemplateValue plus(TemplateValue& value);
  TemplateValue minus(TemplateValue& value);
  TemplateValue not();
  TemplateValue comp(TemplateValue& value);
  TemplateValue and(TemplateValue& value);
  TemplateValue or(TemplateValue& value);
  TemplateValue diff(TemplateValue& value);

protected:
  enTypes _type;
};

#include "TemplateParser.h"
#include "TemplateToken.h"

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
  void add(TemplateValue& value, enOperators nextOperator, bool not);
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
#include "TemplateParser.h"

/*
class oTemplateValue: public SharedPtr<TemplateValue> {
public:
  oTemplateValue(const oTemplateValue& value): SharedPtr<TemplateValue>(value) { }
  oTemplateValue(TemplateValue& value): SharedPtr<TemplateValue>(value) { }
  oTemplateValue(TemplateValue* value = 0): SharedPtr<TemplateValue>(value) { }

  oTemplateValue operator + (oTemplateValue& value) {
    return plus(value);
  }
  oTemplateValue operator - (oTemplateValue& value) {
    return minus(value);
  }
  oTemplateValue operator ! () {
    return not();
  }
  oTemplateValue operator != (oTemplateValue& value) {
    return diff(value);
  }
  oTemplateValue operator == (oTemplateValue& value) {
    return comp(value);
  }
  oTemplateValue operator && (oTemplateValue& value) {
    return and(value);
  }
  oTemplateValue operator || (oTemplateValue& value) {
    return or(value);
  }

  oTemplateValue operator + (TemplateValue* value) {
    return plus(oTemplateValue(new TemplateValue(value)));
  }
  oTemplateValue operator - (TemplateValue* value) {
    return minus(oTemplateValue(new TemplateValue(value)));
  }
  oTemplateValue operator != (TemplateValue* value) {
    return diff(oTemplateValue(new TemplateValue(value)));
  }
  oTemplateValue operator == (TemplateValue* value) {
    return comp(oTemplateValue(new TemplateValue(value)));
  }
  oTemplateValue operator && (TemplateValue* value) {
    return and(oTemplateValue(new TemplateValue(value)));
  }
  oTemplateValue operator || (TemplateValue* value) {
    return or(oTemplateValue(new TemplateValue(value)));
  }
public:
  oTemplateValue plus(oTemplateValue& value);
  oTemplateValue minus(oTemplateValue& value);
  oTemplateValue not();
  oTemplateValue comp(oTemplateValue& value);
  oTemplateValue and(oTemplateValue& value);
  oTemplateValue or(oTemplateValue& value);
  oTemplateValue diff(oTemplateValue& value);

  oTemplateValue plus(TemplateValue* value);
  oTemplateValue minus(TemplateValue* value);
  oTemplateValue comp(TemplateValue* value);
  oTemplateValue and(TemplateValue* value);
  oTemplateValue or(TemplateValue* value);
  oTemplateValue diff(TemplateValue* value);
};
*/

class iTemplateVar {
public:
  iTemplateVar(const string& name): _name(name) { }

public:
  virtual TemplateValue get() = 0;

protected:
  string _name;
};

class TemplateVariable: public iTemplateVar {
public:
  TemplateVariable(const string& name): iTemplateVar(name) { }
  TemplateVariable(const TemplateVariable& var);

public:
  TemplateValue get();
};

class TemplateFunction: public iTemplateVar {
public:
  typedef vector<TemplateParam*> tParams;

public:
  TemplateFunction(const string& name): iTemplateVar(name) { }
  TemplateFunction(const TemplateFunction& func);
  ~TemplateFunction();

public:
  void addParam(TemplateParam* param);
  void removeParam(UINT id);
  UINT countParam();
  void clearParam();

  TemplateValue get();

protected:
  tParams _params;
};

#endif // __TEMPLATE_VALUE_H__