#include "stdafx.h"
#include "TemplateParam.h"

TemplateParam::TemplateParam(const TemplateParam& param) {
  for (tArguments::const_iterator it = param._arguments.begin(); it != param._arguments.end(); it++) {
    add((*it)->value, (*it)->nextOperator, (*it)->not);
  }
  _parser = param._parser;
  _block = param._block;
}

void TemplateParam::add(TemplateValue value, enOperators nextOperator, bool not) {
  _arguments.push_back(new sArgument(value, nextOperator, not));
}

void TemplateParam::clear() {
  for (tArguments::iterator it = _arguments.begin(); it != _arguments.end(); it++) {
    delete *it;
  }
  _arguments.clear();
}

UINT TemplateParam::count() {
  return _arguments.size();
}

TemplateValue TemplateParam::output() {
  if (!_arguments.size()) {
    return TemplateValue();
  }

  tArguments::iterator it = _arguments.begin();
  tArguments::iterator itEnd = _arguments.end();

  TemplateValue value = (*it)->not ? !(*it)->value : (*it)->value;
  enOperators op;
  it++;

  while (it != itEnd) {
    op = (*it)->nextOperator;
    switch (op) {
      case opNone:
        break;
      case opPlus:
        value = value.plus((*it)->not ? !(*it)->value : (*it)->value);
        break;
      case opMinus:
        value = value.minus((*it)->not ? !(*it)->value : (*it)->value);
        break;
      case opOr:
        value = value.or((*it)->not ? !(*it)->value : (*it)->value);
        break;
      case opAnd:
        value = value.and((*it)->not ? !(*it)->value : (*it)->value);
        break;
      case opDiff:
      case opRegExDiff:
        value = value.diff((*it)->not ? !(*it)->value : (*it)->value);
        break;
      case opComp:
      case opRegExComp:
        value = value.comp((*it)->not ? !(*it)->value : (*it)->value);
        break;
    }
    it++;
  }
  return value;
}

TemplateParam::~TemplateParam() {
  clear();
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