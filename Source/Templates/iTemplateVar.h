#pragma once

#ifndef __ITEMPLATE_VAR_H__
#define __ITEMPLATE_VAR_H__

class iVariableManager;
class TemplateValue;
class TemplateParam;

/* iTemplateVar */
class iTemplateVar : public SharedObject<iSharedObject> {
public:
  iTemplateVar(const string& name): _name(name) { }

public:
  virtual TemplateValue get() = 0;

protected:
  string _name;
};

typedef SharedPtr<iTemplateVar> oTemplateVar;

/* TemplateVariable */
class TemplateVariable: public iTemplateVar {
public:
  TemplateVariable(const TemplateVariable& var): iTemplateVar(var._name), _vm(var._vm) { }
  TemplateVariable(const string& name, iVariableManager* lVM);

public:
  virtual TemplateValue get();

protected:
  iVariableManager* _vm;
};

/* TemplateHashVariable */
class TemplateHashVariable: public TemplateVariable {

public:
  TemplateHashVariable(const string& name, const string& key, iVariableManager* lVM): TemplateVariable(name, lVM), _key(key) { }
  TemplateHashVariable(const TemplateHashVariable& var): TemplateVariable(var._name, var._vm), _key(var._key) { }

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
  TemplateValue get();

  void addParam(TemplateParam* param);

protected:
  tParams _params;
};

#endif // __ITEMPLATE_VAR_H__