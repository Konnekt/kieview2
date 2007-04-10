#include "stdafx.h"

#include "TemplateValue.h"
#include "TemplateValueTypes.h"
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

TemplateValue::TemplateValue(const StringRef& value) {
  vString = new String(value);
  _type = tString;
}

TemplateValue::TemplateValue(const string& value) {
  vString = new String(value);
  _type = tString;
}

TemplateValue::TemplateValue(const char* value) {
  vString = new String(value);
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

TemplateValue::TemplateValue(iValueProxy* value) {
  vProxy = value;
  _type = tVar;
}

TemplateValue::TemplateValue(const TemplateHash& value) {
   vHash = new TemplateHash(value);
  _type = tHash;
}

TemplateValue::TemplateValue(const TemplateArray& value) {
   vArray = new TemplateArray(value);
  _type = tArray;
}

TemplateValue TemplateValue::regEx(const string& pattern) {
  TemplateValue value;
  value.vRegExp = new String(pattern);
  value._type = tRegExp;

  return value;
}

TemplateValue TemplateValue::operator [] (const string& key) {
  if (_type == tHash) {
    return vHash->get(key);
  }
  return TemplateValue();
}

TemplateValue TemplateValue::operator [] (int id) {
  if (_type == tArray) {
    return vArray->get(id);
  }
  return TemplateValue();
}

void TemplateValue::copy(TemplateValue& value) {
  _type = value._type;
  if (_type == tString) {
    vString = new String(value.getString());
  } else if (_type == tBoolean) {
    vBool = value.getBool();
  } else if (_type == tInteger) {
    vInt= value.getInt();
  } else if (_type == tInteger64) {
    vInt64 = value.getInt64();
  } else if (_type == tDate64) {
    vDate64 = new Date64(value.getDate());
  } else if (_type == tVar) {
    vProxy = value.vProxy;
  } else if (_type == tParam) {
    vParam = new TemplateParam(*(value.vParam));
  } else if (_type == tRegExp) {
    vRegExp = new String(value.getString());
  } else if (_type == tHash) {
    vHash = new TemplateHash(value.getHash());
  } else if (_type == tArray) {
    vArray = new TemplateArray(value.getArray());
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
    vProxy = 0;
  } else if (_type == tParam) {
    if (vParam)
      delete vParam;
  } else if (_type == tRegExp) {
    if (vRegExp)
      delete vRegExp;
  } else if (_type == tHash) {
    if (vHash)
      delete vHash;
 } else if (_type == tArray) {
    if (vArray)
      delete vArray;
  }
}

String TemplateValue::getString() {
  if (_type == tString) {
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
    return vProxy->getValue().getString();
  } else if (_type == tParam) {
    return vParam->output().getString();
  } else if (_type == tRegExp) {
    return *vRegExp;
  } else if (_type == tHash) {
    //return vHash->get().getString();
  }
  return "";
}

int TemplateValue::getInt() {
  if (_type == tString) {
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
    return vProxy->getValue().getInt();
  } else if (_type == tParam) {
    return vParam->output().getInt();
  } else if (_type == tHash) {
    //return vHash->get().getInt();
  }
  return 0;
}

__int64 TemplateValue::getInt64() {
  if (_type == tString) {
    // err
  } else if (_type == tInteger64) {
    return vInt64;
  } else if (_type == tDate64) {
    return vDate64->getInt64();
  } else if (_type == tVar) {
    return vProxy->getValue().getInt64();
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
    return vProxy->getValue().getBool();
  } else if (_type == tParam) {
    return vParam->output().getBool();
  } else if (_type == tHash) {
    //return vHash->get().getBool();
  }
  return getInt();
}

TemplateHash& TemplateValue::getHash() {
  if (_type == tHash) {
    return *vHash;
  }
  throw new ExceptionString("TemplateValue is not a Hash");
}

TemplateArray& TemplateValue::getArray() {
  if (_type == tArray) {
    return *vArray;
  }
  throw new ExceptionString("TemplateValue is not a Array");
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
    return vProxy->getValue() + value;
  } else if (_type == tParam) {
    return vParam->output() + value;
  } else if (_type == tRegExp) {
    //err
  } else if (_type == tHash) {
    //return vHash->get() + value;
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
    return vProxy->getValue() - value;
  } else if (_type == tParam) {
    return vParam->output() - value;
  } else if (_type == tRegExp) {
    //err
  } else if (_type == tHash) {
    //return vHash->get() - value;
  }
  return TemplateValue();
}

TemplateValue TemplateValue::comp(TemplateValue& value) {
  if (_type == tRegExp) {
    return (bool) RegEx::doMatch(getString().c_str(), value.getString().c_str());
  } else if (value._type == tRegExp){
    return (bool) RegEx::doMatch(value.getString().c_str(), getString().c_str());
  } else if (_type == tString) {
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
    return vProxy->getValue() == value;
  } else if (_type == tParam) {
    return vParam->output() == value;
  } else if (_type == tHash) {
    //return vHash->get() == value;
  }
  return false;
}

TemplateValue TemplateValue::diff(TemplateValue& value) {
  return !comp(value);
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
konwersja int64 to string
*/