#pragma once

#ifndef __TEMPLATE_VALUE_H__
#define __TEMPLATE_VALUE_H__

#include <string>
#include <Stamina/Time64.h>
#include <Stamina/helpers.h>

using namespace std;
using namespace Stamina;

class TemplateValue;

class TemplateParam {
public:
  enum enOperators {
    opNone = 0,
    opPlus = 1,
    opMinus = 2,
    opNot = 3,
    opComp = 4,
    opDiff = 5,
    opAnd = 6,
    opOr = 7,
  };

  struct sArgument {
    bool not;
    TemplateValue* value;
    enOperators nextOperator;

    sArgument(TemplateValue* value, enOperators nextOperator, bool not): value(value), nextOperator(nextOperator), not(not) { }
  };

  typedef vector<sArgument*> tArguments;

public:
  void add(TemplateValue* value, enOperators nextOperator, bool not);
  void clear();
  UINT count();
  TemplateValue output();

protected:
  tArguments _arguments;
};

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

public:
  TemplateValue get();
};

class TemplateFunction: public iTemplateVar {
public:
  typedef vector<TemplateParam*> tParams;

public:
  TemplateFunction(const string& name): iTemplateVar(name) { }

public:
  void addParam(TemplateParam* param);
  void removeParam(UINT id);
  UINT countParam();
  void clearParam();

  TemplateValue get();

private:
  tParams _params;
};

class TemplateValue {
public:
  enum enTypes {
    tString = 1,
    tInteger = 2,
    tInteger64 = 3,
    tBoolean = 4,
    tDate64 = 5,
    tVar = 6,
    tParam = 7,
  };

  union {
    string* vString;
    int vInt;
    __int64 vInt64;
    bool vBool;
    Date64* vDate64;
    iTemplateVar* vVar;
    TemplateParam* vParam;
  };

public:
  TemplateValue operator + (TemplateValue& value) {
    return plus(value);
  }
  TemplateValue operator - (TemplateValue& value) {
    return minus(value);
  }
  TemplateValue operator! () {
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

  enTypes getType();
  void clear();
  void copy(TemplateValue& value);

public:
  TemplateValue();
  TemplateValue(const char* value);
  TemplateValue(const string& value);
  TemplateValue(int value);
  TemplateValue(__int64 value);
  TemplateValue(const Date64& value);
  TemplateValue(bool value);
  TemplateValue(iTemplateVar* value);
  TemplateValue(TemplateParam* value);
  TemplateValue(TemplateValue& value);
  const TemplateValue& operator = (const TemplateValue& copy);
  TemplateValue& operator = (TemplateValue& copy);

public:
  string getString();
  bool getBool();
  int getInt();
  __int64 getInt64();
  Date64 getDate();

protected:
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


#endif // __TEMPLATE_VALUE_H__