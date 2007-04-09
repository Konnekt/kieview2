#pragma once

#ifndef __ITEMPLATE_VAR_H__
#define __ITEMPLATE_VAR_H__

class iVariableManager;
class TemplateValue;
class TemplateParam;

/* iValueProxy */
class iValueProxy : public SharedObject<iSharedObject> {
public:
  iValueProxy(const string& name): _name(name) { }

public:
  virtual TemplateValue getValue() = 0;

protected:
  string _name;
};

typedef SharedPtr<iValueProxy> oValueProxy;

/* TemplateVariable */
class TemplateVariable: public iValueProxy {
public:
  TemplateVariable(const TemplateVariable& var): iValueProxy(var._name), _vm(var._vm) { }
  TemplateVariable(const string& name, iVariableManager* lVM);

public:
  virtual TemplateValue getValue();

protected:
  iVariableManager* _vm;
};

/* TemplateHashVariable */
class TemplateHashVariable: public TemplateVariable {

public:
  TemplateHashVariable(const string& name, const string& key, iVariableManager* lVM): TemplateVariable(name, lVM), _key(key) { }
  TemplateHashVariable(const TemplateHashVariable& var): TemplateVariable(var._name, var._vm), _key(var._key) { }

public:
  TemplateValue getValue();

private:
  string _key;
};

/* TemplateFunction */
class TemplateFunction: public iValueProxy {
public:
  typedef vector<TemplateParam*> tParams;

public:
  TemplateFunction(const string& name): iValueProxy(name) { }
  TemplateFunction(const TemplateFunction& func);
  ~TemplateFunction();

public:
  TemplateValue getValue();

  void addParam(TemplateParam* param);

protected:
  tParams _params;
};

#endif // __ITEMPLATE_VAR_H__