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

class GlobalManager: public SharedObject<iSharedObject>, public iVariableManager, public signals::trackable {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(GlobalManager, iSharedObject, Version(0,0,0,1));

public:
  enum enArgs {
    argsZero = 0,
    argsOne = 1,
    argsTwo = 2,
    argsThree = 3,
    argsFour = 4,
    argsCustom = 5,
  };

  typedef function<oTemplateValue (enArgs, TemplateValue&, TemplateValue&, TemplateValue&, TemplateValue&)> fOnCallFunction;
  typedef signal<oTemplateValue (enArgs, TemplateValue&, TemplateValue&, TemplateValue&, TemplateValue&)> sigOnCallFunction;

  struct sFunction {
    enArgs cArgs;
    sigOnCallFunction signal;

    sFunction(enArgs cArgs, fOnCallFunction& func): cArgs(cArgs) {
      signal.connect(func);
    }
  };

  typedef map<string, sFunction*> tFunctions;

public:
  oTemplateValue callFunction(const string& name);
  oTemplateValue callFunction(const string& name, TemplateValue& arg1);
  oTemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2);
  oTemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3);
  oTemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3, TemplateValue& arg4);

  bool addFunction(const string& name, enArgs cArgs, fOnCallFunction& func);
  bool hasFunction(const string& name);
  bool removeFunction(const string& name);
  void clearFunctions();

private:
  GlobalManager() { }
  ~GlobalManager() { }

public:
  static GlobalManager* get() {
    if (!instance.isValid()) {
      instance = new GlobalManager;
    }
    return instance;
  }

private:
  tFunctions functions;

private:
  static SharedPtr<GlobalManager> instance;
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