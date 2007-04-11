#pragma once

#ifndef __IVARIABLE_MANAGER_H__
#define __IVARIABLE_MANAGER_H__

#include "TemplateValue.h"

class iVariableManager : public iObject {
public:
  struct sVariable {
    TemplateValue value;
    bool attrWrite;

    sVariable(TemplateValue& value, bool attrWrite): value(value), attrWrite(attrWrite) { }
  };

  typedef map<string, TemplateValue> tVariableData;
  typedef map<string, sVariable*> tVariables;

public:
  iVariableManager(iVariableManager* parent = NULL): _parent(parent) { }
  virtual ~iVariableManager() {
    clearVariables();
  }

public:
  virtual bool addVariable(const String& name, TemplateValue value, bool attrWrite = true);
  virtual TemplateValue getVariable(const String& name);
  virtual bool hasVariable(const String& name);

  virtual iVariableManager* find(const String& name);
  virtual bool isWritableVariable(const String& name);
  virtual bool removeVariable(const String& name);

  virtual bool setVariable(const String& name, TemplateValue value, bool create = false);
  virtual void setData(const tVariableData& data);
  virtual void clearVariables();

protected:
  iVariableManager* _parent;
  tVariables _variables;
};

#endif // __IVARIABLE_MANAGER_H__