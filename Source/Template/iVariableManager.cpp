#include "stdafx.h"
#include "iVariableManager.h"

bool iVariableManager::addVariable(const StringRef& name, TemplateValue value, bool attrWrite) {
  if (hasVariable(name)) {
    return false;
  }
  _variables[name] = new sVariable(value, attrWrite);
  return true;
}

bool iVariableManager::hasVariable(const StringRef& name) {
  return !name.empty() && _variables.find(name) != _variables.end();
}

TemplateValue iVariableManager::getVariable(const StringRef& name) {
  if (!hasVariable(name)) {
    return TemplateValue();
  }
  return _variables[name]->value;
}

iVariableManager* iVariableManager::find(const StringRef& name) {
  iVariableManager *pVM = this;
  while (pVM != NULL && !pVM->hasVariable(name)) {
    pVM = pVM->_parent;
  }
  return pVM;
}

bool iVariableManager::isWritableVariable(const StringRef& name) {
  if (!hasVariable(name)) {
    return false;
  }
  return _variables[name]->attrWrite;
}

bool iVariableManager::setVariable(const StringRef& name, TemplateValue value, bool create) {
  if (!hasVariable(name) && !create) {
    return false;
  }
  if (isWritableVariable(name)) {
    _variables[name]->value = value;
  } else {
    addVariable(name, value);
  }
  return true;
}

void iVariableManager::setData(const iVariableManager::tVariableData& data) {
  for (tVariableData::const_iterator it = data.begin(); it != data.end(); it++) {
    setVariable(it->first, it->second, true);
  }
}

bool iVariableManager::removeVariable(const StringRef& name) {
  if (!hasVariable(name)) {
    return false;
  }
  delete _variables[name];

  _variables.erase(_variables.find(name));
  return true;
}

void iVariableManager::clearVariables() {
  for (tVariables::iterator it = _variables.begin(); it != _variables.end(); it++) {
    delete it->second;
  }
  _variables.clear();
}