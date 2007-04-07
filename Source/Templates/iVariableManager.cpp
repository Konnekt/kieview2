#include "stdafx.h"
#include "iVariableManager.h"

bool iVariableManager::addVariable(const string& name, TemplateValue value, bool attrWrite) {
  if (hasVariable(name)) {
    return false;
  }
  variables[name] = new sVariable(value, attrWrite);
  return true;
}

bool iVariableManager::hasVariable(const string& name) {
  return !name.empty() && variables.find(name) != variables.end();
}

TemplateValue iVariableManager::getVariable(const string& name) {
  if (!hasVariable(name)) {
    return TemplateValue();
  }
  return variables[name]->value;
}

iVariableManager* iVariableManager::find(const string& name) {
  iVariableManager *pVM = this;
  while (pVM != NULL && !pVM->hasVariable(name)) {
    pVM = pVM->_parent;
  }
  return pVM;
}

bool iVariableManager::isWritableVariable(const string& name) {
  if (!hasVariable(name)) {
    return false;
  }
  return variables[name]->attrWrite;
}

bool iVariableManager::setVariable(const string& name, TemplateValue value, bool create) {
  if (!hasVariable(name) && !create) {
    return false;
  }
  if (isWritableVariable(name)) {
    variables[name]->value = value;
  } else {
    addVariable(name, (TemplateValue&)value);
  }
  return true;
}

void iVariableManager::setData(const iVariableManager::tVariableData& data) {
  for (iVariableManager::tVariableData::const_iterator it = data.begin(); it != data.end(); it++) {
    setVariable(it->first, it->second, true);
  }
}

bool iVariableManager::removeVariable(const string& name) {
  if (!hasVariable(name)) {
    return false;
  }
  delete variables[name];

  variables.erase(variables.find(name));
  return true;
}

void iVariableManager::clearVariables() {
  for (tVariables::iterator it = variables.begin(); it != variables.end(); it++) {
    delete it->second;
  }
  variables.clear();
}