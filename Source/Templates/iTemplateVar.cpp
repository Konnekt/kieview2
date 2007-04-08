#include "stdafx.h"

#include "iTemplateVar.h"
#include "iVariableManager.h"
#include "GlobalsManager.h"
#include "TemplateParam.h"

TemplateVariable::TemplateVariable(const string& name, iVariableManager* lVM): iTemplateVar(name), _vm(NULL) {
  if (lVM) _vm = lVM->find(name);
}

TemplateValue TemplateVariable::get() {
  if (_vm) {
    return _vm->getVariable(_name);
  }
  return TemplateValue();
}

TemplateValue TemplateHashVariable::get() {
  TemplateValue val = TemplateVariable::get();
  if (val.getType() == TemplateValue::tHash) {
    return val.getHash().get(_key);
  }
  return TemplateValue();
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
}