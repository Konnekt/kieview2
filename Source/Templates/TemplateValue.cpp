#include "stdafx.h"
#include "TemplateValue.h"

TemplateVar::TemplateVar(const string& name, bool isFunction) {
  _name = name;
  _isFunction = isFunction;
}

TemplateVar::TemplateVar(const TemplateVar& copy) {
  this->copy((TemplateVar&) copy);
}

const TemplateVar& TemplateVar::operator = (const TemplateVar& copy) {
  if (this == &copy) return *this;
  this->copy((TemplateVar&) copy);
  return *this;
}

TemplateVar& TemplateVar::operator = (TemplateVar& copy) {
  if (this == &copy) return *this;
  this->copy(copy);
  return *this;
}

TemplateValue TemplateVar::get() {
  return TemplateValue();
}

TemplateVar::~TemplateVar() {
  for (tParams::iterator it = _params.begin(); it != _params.end(); it++) {
    delete (*it);
  }
}
void TemplateVar::copy(TemplateVar& value) {
  _isFunction = value.isFunction();
  _name = value._name;
  for (tParams::iterator it = value._params.begin(); it != value._params.end(); it++) {
//    _params.push_back(new TemplateParam((*it));
  }
}

bool TemplateVar::isFunction() {
  return _isFunction;
}


TemplateValue::TemplateValue(const TemplateValue& value) {
  this->copy((TemplateValue&) value);
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
    vVar = new TemplateVar(*value.vVar);
  }
}

const TemplateValue& TemplateValue::operator = (const TemplateValue& copy) {
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

TemplateValue::TemplateValue(__int64 value) {
  vInt64 = value;
  _type = tInteger64;
}

TemplateValue::TemplateValue(const Date64& value) {
  vDate64 = new Date64(value);
  _type = tDate64;
}

TemplateValue::TemplateValue(const TemplateVar& value) {
  vVar = new TemplateVar(value);
  _type = tVar;
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
  }
  return 0;
}

__int64 TemplateValue::getInt64() {
  if (_type == tInteger64) {
    return vInt64;
  } else if (_type == tDate64) {
    return vDate64->getInt64();
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
  return getInt();
}

TemplateValue::enTypes TemplateValue::getType() {
  return _type;
}

TemplateValue TemplateValue::plus(TemplateValue& value) {
  if (_type == tString) {
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