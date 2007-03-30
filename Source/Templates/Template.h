#pragma once

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

class Template;

#include <fstream>
#include "TemplateValue.h"
#include "TemplateToken.h"
#include <Stamina/Exception.h>

class TemplateException: public ExceptionString {
public:
  TemplateException(const StringRef& reason): ExceptionString(reason) { }
};

class TemplateVarController: public SharedObject<iSharedObject>, public signals::trackable {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(TemplateVarController, iSharedObject, Version(0,0,0,1));

public:
  enum enArgs {
    argsZero = 0,
    argsOne = 1,
    argsTwo = 2,
    argsThree = 3,
    argsFour = 4,
    argsCustom = 5,
  };

  typedef function<TemplateValue (enArgs, TemplateValue&, TemplateValue&, TemplateValue&, TemplateValue&)> fOnCallFunction;
  typedef signal<TemplateValue (enArgs, TemplateValue&, TemplateValue&, TemplateValue&, TemplateValue&)> sigOnCallFunction;

  struct sVariable {
    TemplateValue* value;
    bool attrWrite;

    sVariable(TemplateValue* value, bool attrWrite): value(value), attrWrite(attrWrite) { }
  };

  struct sFunction {
    enArgs cArgs;
    sigOnCallFunction signal;

    sFunction(enArgs cArgs, fOnCallFunction& func): cArgs(cArgs) {
      signal.connect(func);
    }
  };

  typedef map<string, sFunction*> tFunctions;
  typedef map<string, sVariable*> tVariables;

public:
  TemplateValue callFunction(const string& name);
  TemplateValue callFunction(const string& name, TemplateValue& arg1);
  TemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2);
  TemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3);
  TemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3, TemplateValue& arg4);

  bool addFunction(const string& name, enArgs cArgs, fOnCallFunction& func);
  bool hasFunction(const string& name);
  bool clearFunction(const string& name);
  void clearFunctions();

  bool addVariable(const string& name, TemplateValue& value = TemplateValue(), bool attrWrite = true);
  TemplateValue getVariable(const string& name);
  bool hasVariable(const string& name);
  bool isWritableVariable(const string& name);
  bool setVariable(const string& name, const TemplateValue& value);
  bool clearVariable(const string& name);
  void clearVariables();

private:
  TemplateVarController() { }
  ~TemplateVarController() { }

public:
  static TemplateVarController* get() {
    if (!instance.isValid()) {
      instance = new TemplateVarController;
    }
    return instance;
  }

private:
  tVariables variables;
  tFunctions functions;

private:
  static SharedPtr<TemplateVarController> instance;
};

SharedPtr<TemplateVarController> TemplateVarController::instance = 0;


class Template {
  friend class TemplateParser;

public:
  Template(const string& path);
  ~Template();

public:
  bool loaded();
  void tokenize();
  string output();

private:
  iBlockToken* _token;
  bool _loaded;
};

#endif // __TEMPLATE_H__