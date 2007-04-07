#include "stdafx.h"
#include "TemplateValue.h"
#include "Template.h"
#include "TemplateVar.h"
#include "TemplateToken.h"

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
          value = value.diff((*it)->not ? !(*it)->value : (*it)->value);
          break;
        case opComp:
          value = value.comp((*it)->not ? !(*it)->value : (*it)->value);
          break;
        case opRegExDiff:
          value = TemplateValue(!RegEx::doMatch((*it)->value.getString().c_str(), value.getString().c_str())); 
          break;
        case opRegExComp:
          value = TemplateValue(RegEx::doMatch((*it)->value.getString().c_str(), value.getString().c_str()));
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

TemplateVariable::TemplateVariable(const TemplateVariable& var): iTemplateVar(var._name) {
}

TemplateValue TemplateVariable::get() {
  return GlobalsManager::get()->getVariable(_name);
}


TemplateValue TemplateFunction::get() {
  GlobalsManager::tFuncArguments arguments;
  for (tParams::iterator it = _params.begin(); it != _params.end(); it++) {
    arguments.push_back((*it)->output());
  }
  return GlobalsManager::get()->callFunction(_name, arguments);
}

TemplateFunction::TemplateFunction(const TemplateFunction& func): iTemplateVar(func._name) {
  for (tParams::const_iterator it = func._params.begin(); it != func._params.end(); it++) {
    addParam(new TemplateParam(*(*it)));
  }
}

void TemplateFunction::addParam(TemplateParam* param) {
  _params.push_back(param);
}

TemplateFunction::~TemplateFunction() {
  for (tParams::iterator it = _params.begin(); it != _params.end(); it++) {
    delete *it;
  }
  _params.clear();
}

TemplateValue::TemplateValue(const TemplateValue& value) {
  this->copy((TemplateValue&)value);
}

void TemplateValue::copy(TemplateValue& value) {
  _type = value._type;
  if(_type == tString) {
    vString = new string(value.getString());
  } else if (_type == tBoolean) {
    vBool = value.getBool();
  } else if (_type == tInteger) {
    vInt= value.getInt();
  } else if (_type == tInteger64) {
    vInt64 = value.getInt64();
  } else if (_type == tDate64) {
    vDate64 = new Date64(value.getDate());
  } else if (_type == tVar) {
    vVar = new TemplateVariable(*(value.vVar));
  } else if (_type == tFunction) {
    vFunction = new TemplateFunction(*(value.vFunction));
  } else if (_type == tParam) {
    vParam = new TemplateParam(*(value.vParam));
  }
}

const TemplateValue& TemplateValue::operator = (const TemplateValue& copy) {
  if (this == &copy) return *this;
  clear();
  this->copy((TemplateValue&) copy);
  return *this;
}

TemplateValue& TemplateValue::operator = (TemplateValue& copy) {
  if (this == (const TemplateValue*)&copy) return *this;
  clear();
  this->copy((TemplateValue&) copy);
  return *this;
}

TemplateValue::TemplateValue() {
  vBool = false;
  _type = tBoolean;
}

TemplateValue::TemplateValue(bool value) {
  vBool = value;
  _type = tBoolean;
}

TemplateValue::TemplateValue(int value) {
  vInt = value;
  _type = tInteger;
}

TemplateValue::TemplateValue(const string& value) {
  vString = new string(value);
  _type = tString;
}

TemplateValue::TemplateValue(const char* value) {
  vString = new string(value);
  _type = tString;
}

TemplateValue::TemplateValue(__int64 value) {
  vInt64 = value;
  _type = tInteger64;
}

TemplateValue::TemplateValue(const Date64& value) {
  vDate64 = new Date64(value);
  _type = tDate64;
}

TemplateValue::TemplateValue(TemplateParam* value) {
  vParam = value;
  _type = tParam;
}

TemplateValue::TemplateValue(TemplateVariable* value) {
  vVar = value;
  _type = tVar;
}

TemplateValue::TemplateValue(TemplateFunction* value) {
  vFunction = value;
  _type = tFunction;
}

void TemplateValue::clear() {
  if (_type == tDate64) {
    if (vDate64)
      delete vDate64;
  } else if (_type == tString) {
    if (vString)
      delete vString;
  } else if (_type == tVar) {
    if (vVar)
      delete vVar;
  } else if (_type == tFunction) {
    if (vFunction)
      delete vFunction;
  } else if (_type == tParam) {
    if (vParam)
      delete vParam;
  }
}

string TemplateValue::getString() {
  if(_type == tString) {
    return *vString;
  } else if (_type == tBoolean) {
    return vBool ? "true" : "false";
  } else if (_type == tInteger) {
    return inttostr(vInt);
  } else if (_type == tInteger64) {
    return stringf("%ll", vInt64);
  } else if (_type == tDate64) {
    return stringf("%ll", vDate64->getInt64());
  } else if (_type == tVar) {
    return vVar->get().getString();
  } else if (_type == tFunction) {
    return vFunction->get().getString();
  } else if (_type == tParam) {
    return vParam->output().getString();
  }
  return "";
}

int TemplateValue::getInt() {
  if(_type == tString) {
    return atoi(vString->c_str());
  } else if (_type == tBoolean) {
    return vBool;
  } else if (_type == tInteger) {
    return vInt;
  } else if (_type == tInteger64) {
    return vInt64;
  } else if (_type == tDate64) {
    return vDate64->getTime64();
  } else if (_type == tVar) {
    return vVar->get().getInt();
  } else if (_type == tFunction) {
    return vFunction->get().getInt();
  } else if (_type == tParam) {
    return vParam->output().getInt();
  }
  return 0;
}

__int64 TemplateValue::getInt64() {
  if(_type == tString) {
    return _atoi64(vString->c_str());
  } else if (_type == tInteger64) {
    return vInt64;
  } else if (_type == tDate64) {
    return vDate64->getInt64();
  } else if (_type == tVar) {
    return vVar->get().getInt64();
  } else if (_type == tFunction) {
    return vFunction->get().getInt64();
  } else if (_type == tParam) {
    return vParam->output().getInt64();
  }
  return getInt();
}

Date64 TemplateValue::getDate() {
  if (_type == tDate64) {
    return *vDate64;
  }
  return getInt64();
}

bool TemplateValue::getBool() {
  if (_type == tString) {
    return vString->size();
  } else if (_type == tVar) {
    return vVar->get().getBool();
  } else if (_type == tFunction) {
    return vFunction->get().getBool();
  } else if (_type == tParam) {
    return vParam->output().getBool();
  }
  return getInt();
}

TemplateValue::enTypes TemplateValue::getType() {
  return _type;
}

TemplateValue TemplateValue::plus(TemplateValue& value) {
  if (_type == tString || value._type == tString) {
    return TemplateValue(getString() + value.getString());
  } else if (_type == tBoolean) {
    return TemplateValue(getBool() + value.getBool()); //err
  } else if (_type == tInteger) {
    return TemplateValue(getInt() + value.getInt());
  } else if (_type == tInteger64) {
    return TemplateValue(getInt64() + value.getInt64());
  } else if (_type == tDate64) {
    return TemplateValue(Date64(Time64(getInt64() + value.getInt64())));
  } else if (_type == tVar) {
    return vVar->get() + value;
  } else if (_type == tVar) {
    return vFunction->get() + value;
  } else if (_type == tParam) {
    return vParam->output() + value;
  }
  return TemplateValue();
}

TemplateValue TemplateValue::minus(TemplateValue& value) {
  if (_type == tString) {
    //err
  } else if (_type == tBoolean) {
    //err
  } else if (_type == tInteger) {
    return TemplateValue(getInt() - value.getInt());
  } else if (_type == tInteger64) {
    return TemplateValue(getInt64() - value.getInt64());
  } else if (_type == tDate64) {
    return TemplateValue(Date64(Time64(getInt64() - value.getInt64())));
  } else if (_type == tVar) {
    return vVar->get() - value;
  } else if (_type == tFunction) {
    return vFunction->get() - value;
  } else if (_type == tParam) {
    return vParam->output() - value;
  }
  return TemplateValue();
}

TemplateValue TemplateValue::comp(TemplateValue& value) {
  if (_type == tString) {
    return TemplateValue(getString() == value.getString());
  } else if (_type == tBoolean) {
    return TemplateValue(getBool() == value.getBool());
  } else if (_type == tInteger) {
    return TemplateValue(getInt() == value.getInt());
  } else if (_type == tInteger64) {
    return TemplateValue(getInt64() == value.getInt64());
  } else if (_type == tDate64) {
    return TemplateValue(getInt64() == value.getInt64());
  } else if (_type == tVar) {
    return vVar->get() == value;
  } else if (_type == tFunction) {
    return vFunction->get() == value;
  } else if (_type == tParam) {
    return vParam->output() == value;
  }
  return TemplateValue();
}

TemplateValue TemplateValue::diff(TemplateValue& value) {
  if (_type == tString) {
    return TemplateValue(getString() != value.getString());
  } else if (_type == tBoolean) {
    return TemplateValue(getBool() != value.getBool());
  } else if (_type == tInteger) {
    return TemplateValue(getInt() != value.getInt());
  } else if (_type == tInteger64) {
    return TemplateValue(getInt64() != value.getInt64());
  } else if (_type == tDate64) {
    return TemplateValue(getInt64() != value.getInt64());
  } else if (_type == tVar) {
    return vVar->get() != value;
  } else if (_type == tFunction) {
    return vFunction->get() != value;
  } else if (_type == tParam) {
    return vParam->output() != value;
  }
  return TemplateValue(true);
}

TemplateValue TemplateValue::and(TemplateValue& value) {
  return TemplateValue(getBool() && value.getBool());
}

TemplateValue TemplateValue::or(TemplateValue& value) {
  return TemplateValue(getBool() || value.getBool());
}

TemplateValue TemplateValue::not() {
  return TemplateValue(!getBool());
}

TemplateValue::~TemplateValue() {
  clear();
}
/*
oTemplateValue oTemplateValue::plus(TemplateValue* value) {
  return get()->plus(value);
}

oTemplateValue oTemplateValue::minus(TemplateValue* value) {
  return get()->minus(value);
}

oTemplateValue oTemplateValue::comp(TemplateValue* value) {
  return get()->comp(value);
}

oTemplateValue oTemplateValue::diff(TemplateValue* value) {
  return get()->diff(value);
}

oTemplateValue oTemplateValue::and(TemplateValue* value) {
  return get()->and(value);
}

oTemplateValue oTemplateValue::or(TemplateValue* value) {
  return get()->or(value);
}

oTemplateValue oTemplateValue::not() {
  return get()->not();
}

*/
/* to do:
konwersja int64 to stirng
*/