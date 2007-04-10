#include "stdafx.h"
#include "TemplateValueTypes.h"

void TemplateArray::add(TemplateValue value) {
  _array.push_back(value);
}

TemplateValue TemplateArray::get(int id) {
  if (id < count()) {
    return _array[id];
  }
  return TemplateValue();
}

void TemplateArray::set(int id, TemplateValue value) {
  if (id < count()) {
    _array[id] = value;
  }
}

void TemplateArray::remove(int id) {
  if (id >= count()) return;

  tArray::iterator it = _array.begin();
  int i = 0;
  while(i < id) {
    i++;
    it++;
  }
  _array.erase(it);
}

int TemplateArray::count() {
  return _array.size();
}

void TemplateHash::set(const string& name, TemplateValue value) {
  _values[name] = value;
}

TemplateValue TemplateHash::get(const string& name) {
  tValues::iterator it = _values.find(name);
  if (it == _values.end()) {
    if (_values.find("default") == _values.end()) {
      return TemplateValue();
    }
    return _values["default"];
  }
  return it->second;
}