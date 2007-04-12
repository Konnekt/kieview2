#pragma once

#ifndef __GLOBALSMANAGER_H__
#define __GLOBALSMANAGER_H__

#include "iVariableManager.h"

class GlobalsManager: public public iVariableManager, SharedObject<iSharedObject>, public signals::trackable {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(GlobalsManager, iSharedObject, Version(0,0,0,1));

public:
  typedef vector<TemplateValue> tFuncArguments;
  typedef function<TemplateValue (const tFuncArguments&)> fOnCallFunction;
  typedef signal<TemplateValue (const tFuncArguments&)> sigOnCallFunction;

  struct sFunction {
    sigOnCallFunction signal;

    sFunction(fOnCallFunction& func) {
      if (!func.empty()) signal.connect(func);
    }
  };

  typedef map<String, sFunction*> tFunctions;

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

  TemplateValue callFunction(const StringRef& name, const tFuncArguments& arguments);

  bool addFunction(const StringRef& name, fOnCallFunction& func);
  bool hasFunction(const StringRef& name);
  bool removeFunction(const StringRef& name);
  void clearFunctions();

protected:
  tFunctions _functions;

private:
  static SharedPtr<GlobalsManager> instance;
};

#endif // __GLOBALSMANAGER_H__