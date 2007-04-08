#pragma once

#ifndef __TEMPLATE_VALUE_H__
#define __TEMPLATE_VALUE_H__

#include <string>
#include <Stamina/Time64.h>
#include <Stamina/Helpers.h>
#include <Stamina/RegEx.h>

using namespace std;
using namespace Stamina;

class TemplateVariable;
class TemplateFunction;
class TemplateParam;

class TemplateValue {
public:
  enum enTypes {
    tVoid = 0,
    tString = 1,
    tInteger = 2,
    tInteger64 = 3,
    tBoolean = 4,
    tDate64 = 5,
    tVar = 6,
    tFunction = 7,
    tParam = 8,
    tRegExp = 9
  };

  struct sRegExpVal {
    string pattern;
    RegEx regEx;

    sRegExpVal(const string& pattern, const RegEx& regEx): pattern(pattern), regEx(regEx) { }
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
    sRegExpVal* vRegExp;
  };

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
  TemplateValue(const string& pattern, const RegEx& value);
  TemplateValue(const TemplateValue& value);

  ~TemplateValue();

public:
  const TemplateValue& operator = (const TemplateValue& copy) {
    if (this == &copy) return *this;
    this->clear();
    this->copy((TemplateValue&) copy);
    return *this;
  }

  TemplateValue& operator = (TemplateValue& copy) {
    if (this == (TemplateValue*)&copy) return *this;
    this->clear();
    this->copy((TemplateValue&) copy);
    return *this;
  }

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
  TemplateValue plus(TemplateValue& value);
  TemplateValue minus(TemplateValue& value);
  TemplateValue not();
  TemplateValue comp(TemplateValue& value);
  TemplateValue and(TemplateValue& value);
  TemplateValue or(TemplateValue& value);
  TemplateValue diff(TemplateValue& value);

public:
  string getString();
  bool getBool();
  int getInt();
  __int64 getInt64();
  Date64 getDate();

  enTypes getType();
  void clear();
  void copy(TemplateValue& value);

protected:
  enTypes _type;
};

#endif // __TEMPLATE_VALUE_H__