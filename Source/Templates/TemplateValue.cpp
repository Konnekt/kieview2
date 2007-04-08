#include "stdafx.h"

#include "TemplateValue.h"
#include "TemplateParam.h"
#include "iTemplateVar.h"

TemplateValue::TemplateValue(const TemplateValue& value) {
  this->copy((TemplateValue&)value);
}

TemplateValue::TemplateValue() {
  _type = tVoid;
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

TemplateValue::TemplateValue(const string& pattern, const RegEx& value) {
  vRegExp = new sRegExpVal(pattern, value);
  _type = tRegExp;
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
  } else if (_type == tRegExp) {
    vRegExp = new sRegExpVal(value.vRegExp->pattern, value.vRegExp->regEx);
  }
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
  } else if (_type == tRegExp) {
    if (vRegExp)
      delete vRegExp;
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
  } else if (_type == tRegExp) {
    return vRegExp->pattern;
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
    // err
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
  } else if (_type == tInteger64) {
    return getInt64();
  }
  return true;
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
    return getString() + value.getString();
  } else if (_type == tBoolean) {
    //err
  } else if (_type == tInteger) {
    return getInt() + value.getInt();
  } else if (_type == tInteger64) {
    return getInt64() + value.getInt64();
  } else if (_type == tDate64) {
    return Date64(Time64(getInt64() + value.getInt64()));
  } else if (_type == tVar) {
    return vVar->get() + value;
  } else if (_type == tVar) {
    return vFunction->get() + value;
  } else if (_type == tParam) {
    return vParam->output() + value;
  } else if (_type == tRegExp) {
    //err
  }
  return TemplateValue();
}

TemplateValue TemplateValue::minus(TemplateValue& value) {
  if (_type == tString) {
    // todo: usuwanie znakow z value
  } else if (_type == tBoolean) {
    //err
  } else if (_type == tInteger) {
    return getInt() - value.getInt();
  } else if (_type == tInteger64) {
    return getInt64() - value.getInt64();
  } else if (_type == tDate64) {
    return Date64(Time64(getInt64() - value.getInt64()));
  } else if (_type == tVar) {
    return vVar->get() - value;
  } else if (_type == tFunction) {
    return vFunction->get() - value;
  } else if (_type == tParam) {
    return vParam->output() - value;
  } else if (_type == tRegExp) {
    //err
  }
  return TemplateValue();
}

TemplateValue TemplateValue::comp(TemplateValue& value) {
  if (_type == tString) {
    return getString() == value.getString();
  } else if (_type == tBoolean) {
    return getBool() == value.getBool();
  } else if (_type == tInteger) {
    return getInt() == value.getInt();
  } else if (_type == tInteger64) {
    return getInt64() == value.getInt64();
  } else if (_type == tDate64) {
    return getInt64() == value.getInt64();
  } else if (_type == tVar) {
    return vVar->get() == value;
  } else if (_type == tFunction) {
    return vFunction->get() == value;
  } else if (_type == tParam) {
    return vParam->output() == value;
  } else if (_type == tRegExp) {
    return vRegExp->regEx.match(value.getString().c_str());
  }
  return TemplateValue();
}

TemplateValue TemplateValue::diff(TemplateValue& value) {
  if (_type == tString) {
    return getString() != value.getString();
  } else if (_type == tBoolean) {
    return getBool() != value.getBool();
  } else if (_type == tInteger) {
    return getInt() != value.getInt();
  } else if (_type == tInteger64) {
    return getInt64() != value.getInt64();
  } else if (_type == tDate64) {
    return getInt64() != value.getInt64();
  } else if (_type == tVar) {
    return vVar->get() != value;
  } else if (_type == tFunction) {
    return vFunction->get() != value;
  } else if (_type == tParam) {
    return vParam->output() != value;
  } else if (_type == tRegExp) {
    return !vRegExp->regEx.match(value.getString().c_str());
  }
  return true;
}

TemplateValue TemplateValue::and(TemplateValue& value) {
  return getBool() && value.getBool();
}

TemplateValue TemplateValue::or(TemplateValue& value) {
  return getBool() || value.getBool();
}

TemplateValue TemplateValue::not() {
  return !getBool();
}

TemplateValue::~TemplateValue() {
  clear();
}

/* to do:
konwersja int64 to stirng
*/