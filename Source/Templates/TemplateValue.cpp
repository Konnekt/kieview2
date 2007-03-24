#include "stdafx.h"
#include "TemplateValue.h"
#include "Template.h"

void TemplateParam::add(TemplateValue* value, enOperators nextOperator, bool not) {
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
  tArguments::iterator it = _arguments.begin();
  tArguments::iterator itEnd = _arguments.end();

  TemplateValue val;
  if (_arguments.size()) {
    val = *(*it)->value;
    it++;
    enOperators op = (*it)->nextOperator;

    while (it != itEnd) {
      switch (op) {
        case opNone:
          break;
        case opPlus:
          val = val + ((*it)->not ? !(*(*it)->value) : *(*it)->value);
          break;
        case opMinus:
          val = val - ((*it)->not ? !(*(*it)->value) : *(*it)->value);
          break;
        case opOr:
          val = val || ((*it)->not ? !(*(*it)->value) : *(*it)->value);
          break;
        case opAnd:
          val = val && ((*it)->not ? !(*(*it)->value) : *(*it)->value);
          break;
        case opDiff:
          val = val != ((*it)->not ? !(*(*it)->value) : *(*it)->value);
          break;
        case opComp:
          val = val == ((*it)->not ? !(*(*it)->value) : *(*it)->value);
          break;
      }
      op = (*it)->nextOperator;
      it++;
    }
  }
  return val;
}

TemplateValue TemplateVariable::get() {
  return Template::get(_name);
}

TemplateValue TemplateFunction::get() {
  return TemplateValue();
}

void TemplateFunction::addParam(TemplateParam* param) {
  _params.push_back(param);
}

TemplateValue::TemplateValue(TemplateValue& value) {
  this->copy(value);
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
    vVar = value.vVar;
  }
}

const TemplateValue& TemplateValue::operator = (const TemplateValue& copy) {
  if (this == &copy) return *this;
  this->copy((TemplateValue&) copy);
  return *this;
}

TemplateValue& TemplateValue::operator = (TemplateValue& copy) {
  if (this == &copy) return *this;
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
TemplateValue::TemplateValue(iTemplateVar* value) {
  vVar = value;
  _type = tVar;
}

void TemplateValue::clear() {
  if (_type == tDate64) {
    if (vDate64)
      delete vDate64;
  } else if (_type == tString) {
    if (vString)
      delete vString;
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
    return TemplateValue(this->vVar->get() + value);
  } else if (_type == tParam) {
    return TemplateValue(this->vParam->output() + value);
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
    return TemplateValue(this->vVar->get() - value);
  } else if (_type == tParam) {
    return TemplateValue(this->vParam->output() - value);
  }
  return TemplateValue();
}

TemplateValue TemplateValue::comp(TemplateValue &value) {
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
    return TemplateValue(this->vVar->get() == value);
  } else if (_type == tParam) {
    return TemplateValue(this->vParam->output() == value);
  }
  return TemplateValue();
}

TemplateValue TemplateValue::diff(TemplateValue &value) {
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
    return TemplateValue(this->vVar->get() != value);
  } else if (_type == tParam) {
    return TemplateValue(this->vParam->output() != value);
  }
  return TemplateValue(true);
}

TemplateValue TemplateValue::and(TemplateValue &value) {
  return TemplateValue(getBool() && value.getBool());
}

TemplateValue TemplateValue::or(TemplateValue &value) {
  return TemplateValue(getBool() || value.getBool());
}

TemplateValue TemplateValue::not() {
  return TemplateValue(!getBool());
}

/* to do:
konwersja int64 to stirng
*/