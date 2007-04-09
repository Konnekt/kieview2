#pragma once

#ifndef __TEMPLATE_VALUE_H__
#define __TEMPLATE_VALUE_H__

#include <string>
#include <Stamina/Time64.h>
#include <Stamina/Helpers.h>
#include <Stamina/RegEx.h>

#include "iTemplateVar.h"

using namespace std;
using namespace Stamina;

class TemplateHash;

class TemplateValue {
public:
  enum enTypes {
    tVoid,
    tString,
    tInteger,
    tInteger64,
    tBoolean,
    tDate64,
    tVar,
    tParam,
    tRegExp,
    tHash
  };

  union {
    String* vString;
    int vInt;
    __int64 vInt64;
    bool vBool;
    Date64* vDate64;
    TemplateParam* vParam;
    String* vRegExp;
    TemplateHash* vHash;
  };
  oValueProxy vProxy;

public:
  TemplateValue();
  TemplateValue(const char* value);
  TemplateValue(const StringRef& value);
  TemplateValue(const string& value);
  TemplateValue(int value);
  TemplateValue(__int64 value);
  TemplateValue(const Date64& value);
  TemplateValue(bool value);
  TemplateValue(TemplateParam* value);
  TemplateValue(iValueProxy* value);
  TemplateValue(const TemplateHash& value);
  TemplateValue(const TemplateValue& value);

  ~TemplateValue();

  static TemplateValue regEx(const string& pattern);

public:
  const TemplateValue& operator = (const TemplateValue& copy) {
    if (this == &copy) return *this;
    this->clear();
    this->copy((TemplateValue&) copy);
    return *this;
  }

  TemplateValue& operator = (TemplateValue& copy) {
    if (this == &copy) return *this;
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
  TemplateValue operator [] (const string& key);

public:
  TemplateValue plus(TemplateValue& value);
  TemplateValue minus(TemplateValue& value);
  TemplateValue not();
  TemplateValue comp(TemplateValue& value);
  TemplateValue and(TemplateValue& value);
  TemplateValue or(TemplateValue& value);
  TemplateValue diff(TemplateValue& value);

public:
  String getString();
  bool getBool();
  int getInt();
  __int64 getInt64();
  Date64 getDate();
  TemplateHash& getHash();

  void clear();
  void copy(TemplateValue& value);
  enTypes getType();

protected:
  enTypes _type;
};

#endif // __TEMPLATE_VALUE_H__