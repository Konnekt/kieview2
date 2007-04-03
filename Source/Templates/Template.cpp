#include "stdafx.h"
#include "Template.h"
#include "TemplateParser.h"
#include "TemplateToken.h"

SharedPtr<TemplateVarController> TemplateVarController::instance = 0;

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
  _token = new iBlockToken();

  _loaded = true;
}

Template::~Template() {
  delete _token;
}

bool Template::loaded() {
  return _loaded;
}

string Template::output() {
  return _token->output();
}


bool TemplateVarController::addVariable(const string& name, oTemplateValue& value, bool attrWrite) {
  if (hasVariable(name)) {
    return false;
  }
  variables[name] = new sVariable(value, attrWrite);
  return true;
}

bool TemplateVarController::hasVariable(const string& name) {
  return !name.empty() && variables.find(name) != variables.end();
}

oTemplateValue TemplateVarController::getVariable(const string& name) {
  if (!hasVariable(name)) {
    return oTemplateValue(new TemplateValue("Var " + name + "does not exist."));
  }
  return variables[name]->value;
}

bool TemplateVarController::isWritableVariable(const string& name) {
  if (!hasVariable(name)) {
    return false;
  }
  return variables[name]->attrWrite;
}

bool TemplateVarController::setVariable(const string& name, const oTemplateValue& value) {
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

bool TemplateVarController::removeVariable(const string& name) {
  if (!hasVariable(name)) {
    return false;
  }
  delete variables[name]->value;
  delete variables[name];

  variables.erase(variables.find(name));
  return true;
}

void TemplateVarController::clearVariables() {
  for (tVariables::iterator it = variables.begin(); it != variables.end(); it++) {
    delete it->second->value;
    delete it->second;
  }
  variables.clear();
}

bool TemplateVarController::addFunction(const string& name, enArgs cArgs, TemplateVarController::fOnCallFunction& func) {
  if (hasFunction(name)) {
    return false;
  }
  functions[name] = new sFunction(cArgs, func);
  return true;
}

oTemplateValue TemplateVarController::callFunction(const string& name) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsZero || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsZero, TemplateValue(), TemplateValue(), TemplateValue(), TemplateValue());
}

oTemplateValue TemplateVarController::callFunction(const string& name, TemplateValue& arg1) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsOne || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsOne, arg1, TemplateValue(), TemplateValue(), TemplateValue());
}

oTemplateValue TemplateVarController::callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsTwo || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsTwo, arg1, arg2, TemplateValue(), TemplateValue());
}

oTemplateValue TemplateVarController::callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsThree || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsThree, arg1, arg2, arg3, TemplateValue());
}

oTemplateValue TemplateVarController::callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3, TemplateValue& arg4) {
  if (!hasFunction(name)) {
    //throw
  }
  if (functions[name]->cArgs != argsFour || functions[name]->cArgs != argsCustom) {
    //throw
  }
  return functions[name]->signal(argsFour, arg1, arg2, arg3, arg4);
}

bool TemplateVarController::hasFunction(const string& name) {
  return !name.empty() && functions.find(name) != functions.end();
}

bool TemplateVarController::removeFunction(const string& name) {
  if (!hasFunction(name)) {
    return false;
  }
  delete functions[name];

  functions.erase(functions.find(name));
  return true;
}

void TemplateVarController::clearFunctions() {
  for (tFunctions::iterator it = functions.begin(); it != functions.end(); it++) {
    delete it->second;
  }
  functions.clear();
}