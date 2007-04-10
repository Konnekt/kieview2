#pragma once

#ifndef __TEMPLATE_VALUE_TYPES
#define __TEMPLATE_VALUE_TYPES

#include "TemplateValue.h";

class TemplateArray {
public:
  typedef vector<TemplateValue> tArray;

public:
  TemplateValue operator [] (int id) {
    return get(id);
  }

public:
  void add(TemplateValue value);
  TemplateValue get(int id);
  void set(int id, TemplateValue value);
  void remove(int id);
  int count();

protected:
  tArray _array;
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

#endif // __TEMPLATE_VALUE_TYPES