#include "stdafx.h"
#include "GlobalsManager.h"

SharedPtr<GlobalsManager> GlobalsManager::instance = 0;

bool GlobalsManager::addFunction(const string& name, GlobalsManager::fOnCallFunction& func) {
  if (hasFunction(name)) {
    return false;
  }
  functions[name] = new sFunction(func);
  return true;
}

TemplateValue GlobalsManager::callFunction(const string& name, const tFuncArguments& arguments) {
  if (!hasFunction(name)) {
    throw TemplateException("There's no function with name = " + name);
  }
  return functions[name]->signal(arguments);
}

bool GlobalsManager::hasFunction(const string& name) {
  return !name.empty() && functions.find(name) != functions.end();
}

bool GlobalsManager::removeFunction(const string& name) {
  if (!hasFunction(name)) {
    return false;
  }
  delete functions[name];

  functions.erase(functions.find(name));
  return true;
}

void GlobalsManager::clearFunctions() {
  for (tFunctions::iterator it = functions.begin(); it != functions.end(); it++) {
    delete it->second;
  }
  functions.clear();
}