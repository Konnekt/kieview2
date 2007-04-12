#include "stdafx.h"
#include "GlobalsManager.h"

SharedPtr<GlobalsManager> GlobalsManager::instance = 0;

bool GlobalsManager::addFunction(const StringRef& name, GlobalsManager::fOnCallFunction& func) {
  if (hasFunction(name)) {
    return false;
  }
  _functions[name] = new sFunction(func);
  return true;
}

TemplateValue GlobalsManager::callFunction(const StringRef& name, const tFuncArguments& arguments) {
  if (!hasFunction(name)) {
    return TemplateValue();
  }
  return _functions[name]->signal(arguments);
}

bool GlobalsManager::hasFunction(const StringRef& name) {
  return !name.empty() && _functions.find(name) != _functions.end();
}

bool GlobalsManager::removeFunction(const string& name) {
  if (!hasFunction(name)) {
    return false;
  }
  delete _functions[name];

  _functions.erase(_functions.find(name));
  return true;
}

void GlobalsManager::clearFunctions() {
  for (tFunctions::iterator it = _functions.begin(); it != _functions.end(); it++) {
    delete it->second;
  }
  _functions.clear();
}