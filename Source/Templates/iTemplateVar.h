#pragma once

#ifndef __ITEMPLATE_VAR_H__
#define __ITEMPLATE_VAR_H__

#include "TemplateValue.h"
#include "iVariableManager.h"

/* iTemplateVar */
class iTemplateVar {
public:
  iTemplateVar(const string& name): _name(name) { }

public:
  virtual TemplateValue get() = 0;

protected:
  string _name;
};

/* TemplateVariable */
class TemplateVariable: public iTemplateVar {
public:
  TemplateVariable(const string& name, iVariableManager* lVM);
  TemplateVariable(const TemplateVariable& var);

public:
  virtual TemplateValue get();

protected:
  iVariableManager* _vm;
};

/* TemplateHashVariable */
class TemplateHashVariable: public TemplateVariable {

public:
  TemplateHashVariable(const string& name, const string& key, iVariableManager* lVM): TemplateVariable(name, lVM), _key(key) { }
  TemplateHashVariable(const TemplateHashVariable& var);

public:
  virtual TemplateValue get();

private:
  string _key;
};

/* TemplateFunction */
class TemplateFunction: public iTemplateVar {
public:
  typedef vector<TemplateParam*> tParams;

public:
  TemplateFunction(const string& name): iTemplateVar(name) { }
  TemplateFunction(const TemplateFunction& func);
  ~TemplateFunction();

public:
  void addParam(TemplateParam* param);
  void removeParam(UINT id);
  UINT countParam();
  void clearParam();

  TemplateValue get();

protected:
  tParams _params;
};

#endif // __ITEMPLATE_VAR_H__