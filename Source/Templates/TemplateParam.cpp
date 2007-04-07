#include "stdafx.h"

#include "TemplateParam.h"

TemplateParam::TemplateParam(TemplateParser* parser, iBlockToken* token): _parser(parser), _block(token) {
}

TemplateParam::TemplateParam(const TemplateParam& param) {
  for (tArguments::const_iterator it = param._arguments.begin(); it != param._arguments.end(); it++) {
    add((*it)->value, (*it)->nextOperator, (*it)->not);
  }
  _parser = param._parser;
  _block = param._block;
}

void TemplateParam::add(TemplateValue& value, enOperators nextOperator, bool not) {
  _arguments.push_back(new sArgument(value, nextOperator, not));
}

void TemplateParam::clear() {
  for (tArguments::iterator it = _arguments.begin(); it != _arguments.end(); it++) {
    //delete (*it)->value;
    delete *it;
  }
  _arguments.clear();
}

UINT TemplateParam::count() {
  return _arguments.size();
}

TemplateValue TemplateParam::output() {
  tArguments::iterator it = _arguments.begin();
  tArguments::iterator itEnd = _arguments.end();

  TemplateValue value;
  if (_arguments.size()) {
    value = TemplateValue((*it)->not ? !(*it)->value : (*it)->value);
    it++;
    enOperators op;
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
  }
  return value;
}

TemplateParam::~TemplateParam() {
  clear();
}