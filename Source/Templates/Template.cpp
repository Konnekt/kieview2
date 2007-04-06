#include "stdafx.h"
#include "Template.h"
#include "TemplateParser.h"
#include "TemplateToken.h"

SharedPtr<GlobalsManager> GlobalsManager::instance = 0;

Template::Template(const string& path) {
  ifstream tpl(path.c_str());

  string line;
  if (!tpl.is_open()) {
    throw TemplateException("Template does not exist in path: " + path);
  }
  while (!tpl.eof()) {
    getline(tpl, line);
    _data += line + '\n';
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
struct Template::sVariable {
  TemplateValue value;
  bool attrWrite;

  sVariable(TemplateValue& value, bool attrWrite): value(value), attrWrite(attrWrite) { }
};

TemplateParser* Template::getParser() {
  return _parser;
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
    return oTemplateValue(new TemplateValue());
  }
  return variables[name]->value;
}

bool iVariableManager::isWritableVariable(const string& name) {
  if (!hasVariable(name)) {
    return false;
  }
  return variables[name]->attrWrite;
}

bool iVariableManager::setVariable(const string& name, const oTemplateValue& value, bool create) {
  if (!hasVariable(name) && !create) {
    return false;
  }
  if (isWritableVariable(name)) {
    variables[name]->value = value;
  } else {
    addVariable(name, (oTemplateValue&)value);
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

bool GlobalsManager::addFunction(const string& name, GlobalsManager::fOnCallFunction& func) {
  if (hasFunction(name)) {
    return false;
  }
  functions[name] = new sFunction(func);
  return true;
}

oTemplateValue GlobalsManager::callFunction(const string& name, const tFuncArguments& arguments) {
  if (!hasFunction(name)) {
    return oTemplateValue(new TemplateValue());
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