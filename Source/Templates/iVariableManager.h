#pragma once

#ifndef __IVARIABLEMANAGER_H__
#define __IVARIABLEMANAGER_H__

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

  /*
  iVariableManager& operator << (TemplateHash& hash) {
    for (tParams::iterator it = hash.values().begin(); it != hash.values().end(); it++) {
      setVariable(it->first, it->second);
    }
  }
  */

public:
  virtual bool addVariable(const string& name, TemplateValue value, bool attrWrite = true);
  virtual TemplateValue getVariable(const string& name);
  virtual bool hasVariable(const string& name);

  virtual iVariableManager* find(const string& name);
  virtual bool isWritableVariable(const string& name);
  virtual bool removeVariable(const string& name);

  virtual bool setVariable(const string& name, TemplateValue value, bool create = false);
  virtual void setData(const tVariableData& data);
  virtual void clearVariables();

protected:
  iVariableManager* _parent;
  tVariables variables;
};

#endif // __IVARIABLEMANAGER_H__