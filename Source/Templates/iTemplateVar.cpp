#include "stdafx.h"

#include "iTemplateVar.h"
#include "iVariableManager.h"
#include "GlobalsManager.h"
#include "TemplateParam.h"
#include "TemplateValueTypes.h"

TemplateVariable::TemplateVariable(const string& name, iVariableManager* lVM): iValueProxy(name), _vm(NULL) {
  if (lVM) _vm = lVM->find(name);
}

TemplateValue TemplateVariable::getValue() {
  if (_vm) {
    return _vm->getVariable(_name);
  }
  return TemplateValue();
}

TemplateValue TemplateHashVariable::getValue() {
  TemplateValue val = TemplateVariable::getValue();
  if (val.getType() == TemplateValue::tHash) {
    return val.getHash().get(_key);
  }
  return TemplateValue();
}

TemplateValue TemplateFunction::getValue() {
  GlobalsManager::tFuncArguments arguments;
  for (tParams::iterator it = _params.begin(); it != _params.end(); it++) {
    arguments.push_back((*it)->output());
  }
  return GlobalsManager::get()->callFunction(_name, arguments);
}

TemplateFunction::TemplateFunction(const TemplateFunction& func): iValueProxy(func._name) {
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
}