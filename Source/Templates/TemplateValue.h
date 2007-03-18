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
  struct sWord {
    bool not;
    TemplateValue* value;

    sWord(TemplateValue* value, bool not);
  };

  typedef vector<sWord*> tWords;

  enum enOperators {
    opPlus = 1,
    opMinus = 2,
    opNot = 3,
    opComp = 4,
    opDiff = 5,
    opAnd = 6,
    opOr = 7,
  };

public:
  void addOperator(enOperators operator_);
  void addWord(sWord& word);


protected:
  tWords _words;
};

class TemplateVar {
public:
  typedef vector<TemplateParam*> tParams;

public:
  TemplateVar(const string& name, bool isFunction = false);
  TemplateVar(const TemplateVar& copy);
  const TemplateVar& operator = (const TemplateVar& copy);
  TemplateVar& operator = (TemplateVar& copy);
  ~TemplateVar();

public:
  bool isFunction();
  void copy(TemplateVar& value);
  void addArg(TemplateParam& value);
  void removeArg(UINT id);
  TemplateValue get();

protected:
  string _name;
  tParams _params;
  bool _isFunction;
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
  };

  union {
    string* vString;
    int vInt;
    __int64 vInt64;
    bool vBool;
    Date64* vDate64;
    TemplateVar* vVar;
  };

public:
  TemplateValue& operator + (TemplateValue& value) {
    return plus(value);
  }
  TemplateValue& operator - (TemplateValue& value) {
    return minus(value);
  }
  TemplateValue& operator! () {
    return not();
  }
  TemplateValue& operator != (TemplateValue& value) {
    return diff(value);
  }
  TemplateValue& operator == (TemplateValue& value) {
    return comp(value);
  }
  TemplateValue& operator && (TemplateValue& value) {
    return and(value);
  }
  TemplateValue& operator || (TemplateValue& value) {
    return or(value);
  }

  virtual enTypes getType();
  virtual void clear();
  virtual void copy(TemplateValue& value);

public:
  TemplateValue();
  TemplateValue(const string& value);
  TemplateValue(int value);
  TemplateValue(__int64 value);
  TemplateValue(const Date64& value);
  TemplateValue(bool value);
  TemplateValue(const TemplateVar& value);
  TemplateValue(const TemplateValue& value);
  const TemplateValue& operator = (const TemplateValue& copy);
  TemplateValue& operator = (TemplateValue& copy);

public:
  virtual string getString();
  virtual bool getBool();
  virtual int getInt();
  virtual __int64 getInt64();
  virtual Date64 getDate();

protected:
  virtual TemplateValue plus(TemplateValue& value);
  virtual TemplateValue minus(TemplateValue& value);
  virtual TemplateValue not();
  virtual TemplateValue comp(TemplateValue& value);
  virtual TemplateValue and(TemplateValue& value);
  virtual TemplateValue or(TemplateValue& value);
  virtual TemplateValue diff(TemplateValue& value);

protected:
  enTypes _type;
};


#endif // __TEMPLATE_VALUE_H__