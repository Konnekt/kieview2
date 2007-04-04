#pragma once

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

class Template;
typedef SharedPtr<class Template> oTemplate;

#include <fstream>
#include "TemplateValue.h"
#include "TemplateToken.h"
#include <Stamina/Exception.h>

class TemplateException: public ExceptionString {
public:
  TemplateException(const StringRef& reason): ExceptionString(reason) { }
};

class iVariableManager {
public:
  struct sVariable {
    oTemplateValue value;
    bool attrWrite;

    sVariable(oTemplateValue& value, bool attrWrite): value(value), attrWrite(attrWrite) { }
  };

  typedef map<string, sVariable*> tVariables;

public:
  virtual bool addVariable(const string& name, oTemplateValue& value, bool attrWrite = true);
  virtual oTemplateValue getVariable(const string& name);
  virtual bool hasVariable(const string& name);
  virtual bool isWritableVariable(const string& name);
  virtual bool setVariable(const string& name, const oTemplateValue& value);
  virtual bool removeVariable(const string& name);
  virtual void clearVariables();

protected:
  tVariables variables;
};

class GlobalsManager: public SharedObject<iSharedObject>, public iVariableManager, public signals::trackable {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(GlobalsManager, iSharedObject, Version(0,0,0,1));

public:
  enum enArgs {
    argsZero = 0,
    argsOne = 1,
    argsTwo = 2,
    argsThree = 3,
    argsFour = 4,
    argsCustom = 5,
  };

  typedef vector<oTemplateValue> tFuncArguments;
  typedef function<oTemplateValue (const tFuncArguments& arguments)> fOnCallFunction;
  typedef signal<oTemplateValue (const tFuncArguments& arguments)> sigOnCallFunction;

  struct sFunction {
    sigOnCallFunction signal;

    sFunction(fOnCallFunction& func) {
      signal.connect(func);
    }
  };

  typedef map<string, sFunction*> tFunctions;

public:
  oTemplateValue callFunction(const string& name, const tFuncArguments& arguments);

  bool addFunction(const string& name, fOnCallFunction& func);
  bool hasFunction(const string& name);
  bool removeFunction(const string& name);
  void clearFunctions();

private:
  GlobalsManager() { }
  ~GlobalsManager() { }

public:
  static GlobalsManager* get() {
    if (!instance.isValid()) {
      instance = new GlobalsManager;
    }
    return instance;
  }

private:
  tFunctions functions;

private:
  static SharedPtr<GlobalsManager> instance;
};


class Template: public SharedObject<iSharedObject>, public iVariableManager {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(Template, iSharedObject, Version(0,0,0,1));

  friend class TemplateParser;

public:
  Template(const string& path);
  ~Template();

public:
  TemplateParser* getParser();
  bool loaded();
  string output();
  void clear();

public:
  oTemplateValue getVariable(const string& name);

private:
  TemplateParser* _parser;
  iBlockToken* _token;
  bool _loaded;
  string _data;
};

#endif // __TEMPLATE_H__