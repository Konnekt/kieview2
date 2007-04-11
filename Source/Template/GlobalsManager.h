#pragma once

#ifndef __GLOBALSMANAGER_H__
#define __GLOBALSMANAGER_H__

#include "iVariableManager.h"

class GlobalsManager: public SharedObject<iSharedObject>, public iVariableManager, public signals::trackable {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(GlobalsManager, iSharedObject, Version(0,0,0,1));

public:
  typedef vector<TemplateValue> tFunctionArguments;
  typedef function<TemplateValue (const tFuncArguments&)> fOnCallFunction;
  typedef signal<TemplateValue (const tFuncArguments&)> sigOnCallFunction;

  struct sFunction {
    sigOnCallFunction signal;

    sFunction(fOnCallFunction& func) {
      if (!func.empty()) signal.connect(func);
    }
  };

  typedef map<string, sFunction*> tFunctions;

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

  TemplateValue callFunction(const string& name, const tFuncArguments& arguments);

  bool addFunction(const string& name, fOnCallFunction& func);
  bool hasFunction(const string& name);
  bool removeFunction(const string& name);
  void clearFunctions();

protected:
  tFunctions _functions;

private:
  static SharedPtr<GlobalsManager> instance;
};

#endif // __GLOBALSMANAGER_H__