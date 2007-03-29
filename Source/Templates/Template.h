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

class Template {
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

  struct sVar {
    TemplateValue* value;
    bool attrWrite;

    sVar(TemplateValue* value, bool attrWrite): value(value), attrWrite(attrWrite) { }
  };

  struct sFunction {
    enArgs cArgs;
    sigOnCallFunction signal;

    sFunction(enArgs cArgs, fOnCallFunction& func): cArgs(cArgs) {
      signal.connect(func);
    }
  };

  typedef map<string, sFunction*> tFunctions;
  typedef map<string, sVar*> tVars;

public:
  Template(const string& path);
  ~Template();

public:
  static TemplateValue callFunction(const string& name);
  static TemplateValue callFunction(const string& name, TemplateValue& arg1);
  static TemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2);
  static TemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3);
  static TemplateValue callFunction(const string& name, TemplateValue& arg1, TemplateValue& arg2, TemplateValue& arg3, TemplateValue& arg4);

  static bool addFunction(const string& name, enArgs cArgs, fOnCallFunction& func);
  static bool hasFunction(const string& name);
  static bool removeFunction(const string& name);
  static void removeFunctions();

  static bool addVar(const string& name, TemplateValue& value = TemplateValue(), bool attrWrite = true);
  static TemplateValue getVar(const string& name);
  static bool hasVar(const string& name);
  static bool setVar(const string& name, const TemplateValue& value);
  static bool removeVar(const string& name);
  static void removeVars();

private:
  static tVars vars;
  static tFunctions funcs;

public:
  bool loaded();
  string output();

private:
  iBlockToken* _token;
  bool _loaded;
};

#endif // __TEMPLATE_H__