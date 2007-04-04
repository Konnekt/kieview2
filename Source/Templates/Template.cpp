#include "stdafx.h"
#include "Template.h"
#include "TemplateParser.h"
#include "TemplateToken.h"

SharedPtr<GlobalManager> GlobalManager::instance = 0;

Template::Template(const string& path) {
  ifstream tpl(path.c_str());

  string line;
  if (!tpl.is_open()) {
    throw TemplateException("Template does not exist in path: " + path);
  }
  while (!tpl.eof()) {
    getline(tpl, line);
    _data += line;
  }
  tpl.close();
  _token = NULL;

  _loaded = true;
}

Template::~Template() {
  clear();
}

bool Template::loaded() {
  return _loaded;
}

void Template::clear() {
  if (_token) {
    delete _token;
  }
}

TemplateParser* Template::getParser() {
  return _parser;
}

oTemplateValue Template::getVariable(const std::string &name) {
  if (hasVariable(name)) {
    return iVariableManager::getVariable(name);
  }
  return GlobalManager::get()->getVariable(name);
}

string Template::output() {
  return _token->output();
}


bool iVariableManager::addVariable(const string& name, oTemplateValue& value, bool attrWrite) {
  if (hasVariable(name)) {
    return false;
  }
  variables[name] = new sVariable(value, attrWrite);
  return true;
}

bool iVariableManager::hasVariable(const string& name) {
  return !name.empty() && variables.find(name) != variables.end();
}

oTemplateValue iVariableManager::getVariable(const string& name) {
  if (!hasVariable(name)) {
    return oTemplateValue(new TemplateValue("Var " + name + "does not exist."));
  }
  return variables[name]->value;
}

bool iVariableManager::isWritableVariable(const string& name) {
  if (!hasVariable(name)) {
    return false;
  }
  return variables[name]->attrWrite;
}

bool iVariableManager::setVariable(const string& name, const oTemplateValue& value) {
  if (!hasVariable(name)) {
    return false;
  }
  if (isWritableVariable(name)) {
    variables[name]->value = value;
  } else {
    //throw xxx
  }
  return true;
}

bool iVariableManager::removeVariable(const string& name) {
  if (!hasVariable(name)) {
    return false;
  }
  delete variables[name]->value;
  delete variables[name];

  variables.erase(variables.find(name));
  return true;
}

void iVariableManager::clearVariables() {
  for (tVariables::iterator it = variables.begin(); it != variables.end(); it++) {
    delete it->second->value;
    delete it->second;
  }
  variables.clear();
}

bool GlobalManager::addFunction(const string& name, enArgs cArgs, GlobalManager::fOnCallFunction& func) {
  if (hasFunction(name)) {
    return false;
  }
  functions[name] = new sFunction(cArgs, func);
  return true;
}

oTemplateValue GlobalManager::callFunction(const string& name) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsZero || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsZero, TemplateValue(), TemplateValue(), TemplateValue(), TemplateValue());
}

oTemplateValue GlobalManager::callFunction(const string& name, TemplateValue& arg1) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsOne || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsOne, arg1, TemplateValue(), TemplateValue(), TemplateValue());
}

oTemplateValue GlobalManager::callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsTwo || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsTwo, arg1, arg2, TemplateValue(), TemplateValue());
}

oTemplateValue GlobalManager::callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsThree || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsThree, arg1, arg2, arg3, TemplateValue());
}

oTemplateValue GlobalManager::callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3, TemplateValue& arg4) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsFour || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsFour, arg1, arg2, arg3, arg4);
}

bool GlobalManager::hasFunction(const string& name) {
  return !name.empty() && functions.find(name) != functions.end();
}

bool GlobalManager::removeFunction(const string& name) {
  if (!hasFunction(name)) {
    return false;
  }
  delete functions[name];

  functions.erase(functions.find(name));
  return true;
}

void GlobalManager::clearFunctions() {
  for (tFunctions::iterator it = functions.begin(); it != functions.end(); it++) {
    delete it->second;
  }
  functions.clear();
}