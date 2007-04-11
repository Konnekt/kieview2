#pragma once

#ifndef __IVARIABLE_MANAGER_H__
#define __IVARIABLE_MANAGER_H__

#include "TemplateValue.h"

class iVariableManager : public iObject {
public:
  struct sVariable {
    TemplateValue value;
    bool attrWrite;

    sVariable(const TemplateValue& value, bool attrWrite): value(value), attrWrite(attrWrite) { }
  };

  typedef map<string, TemplateValue> tVariableData;
  typedef map<string, sVariable*> tVariables;

public:
  iVariableManager(iVariableManager* parent = NULL): _parent(parent) { }
  virtual ~iVariableManager() {
    clearVariables();
  }

public:
  virtual bool addVariable(const StringRef& name, TemplateValue value, bool attrWrite = false);
  virtual TemplateValue getVariable(const StringRef& name);
  virtual bool hasVariable(const StringRef& name);

  virtual iVariableManager* find(const StringRef& name);
  virtual bool isWritableVariable(const StringRef& name);
  virtual bool removeVariable(const StringRef& name);

  virtual bool setVariable(const StringRef& name, TemplateValue value, bool create = true);
  virtual void setData(const tVariableData& data);
  virtual void clearVariables();

protected:
  iVariableManager* _parent;
  tVariables _variables;
};

#endif // __IVARIABLE_MANAGER_H__