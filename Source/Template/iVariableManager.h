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
  virtual bool addVariable(const string& name, TemplateValue value, bool attrWrite = false);
  virtual TemplateValue getVariable(const string& name);
  virtual bool hasVariable(const string& name);

  virtual iVariableManager* find(const string& name);
  virtual bool isWritableVariable(const string& name);
  virtual bool removeVariable(const string& name);

  virtual bool setVariable(const string& name, TemplateValue value, bool create = true);
  virtual void setData(const tVariableData& data);
  virtual void clearVariables();

protected:
  iVariableManager* _parent;
  tVariables _variables;
};

#endif // __IVARIABLE_MANAGER_H__